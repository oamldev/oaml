#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ByteBuffer.h"
#include "wav.h"
#include "oaml.h"
#include "oamlAudio.h"
#include "oamlTrack.h"
#include "gettime.h"

int debug=0;

static struct {
	char *path;
	oamlTrack *tracks[1024];
	int tracksN;

	oamlTrack *curTrack;

	ByteBuffer *buffer;
	ByteBuffer *dbuffer;

	int freq;
	int channels;
	int bytes;
} oamlData;

int oamlInit(const char *pathToMusic) {
	FILE *f;
	char filename[1024];
	char str[1024];

	memset(&oamlData, 0, sizeof(oamlData));
	oamlData.path = strdup(pathToMusic);
	sprintf(filename, "%soaml.defs", pathToMusic);

	f = fopen(filename, "r");
	printf("%s: %s\n", __FUNCTION__, filename);
	if (f == NULL) return -1;

	while (1) {
		if (fgets(str, 1024, f) == NULL) break;

		char name[1024];
		int mode;
		int audioCount;
		float bpm;
		sscanf(str, "%d %f %d %s", &mode, &bpm, &audioCount, name);
		oamlTrack *track = new oamlTrack(name, mode, bpm);
		for (int i=0; i<audioCount; i++) {
			if (fgets(str, 1024, f) == NULL) break;

			char fname[1024];
			int bars;
			int type;
			int fadeIn = 0;
			int fadeOut = 0;
			int condId = -1;
			int condType = 0;
			int condValue = 0;
			sscanf(str, "%d %d %s %d %d %d %d %d", &type, &bars, filename, &fadeIn, &fadeOut, &condId, &condType, &condValue);
			sprintf(fname, "%s%s", pathToMusic, filename);
			oamlAudio *audio = new oamlAudio(fname, type, bars, bpm, fadeIn, fadeOut);
			if (condId != -1) {
				audio->SetCondition(condId, condType, condValue);
			}

			track->AddAudio(audio);
		}

		oamlData.tracks[oamlData.tracksN++] = track;
	}
	fclose(f);

	oamlData.buffer = new ByteBuffer();
	if (debug) {
		oamlData.dbuffer = new ByteBuffer();
	}

	return 0;
}

void oamlSetAudioFormat(int freq, int channels, int bytes) {
	oamlData.freq = freq;
	oamlData.channels = channels;
	oamlData.bytes = bytes;
}

int oamlPlayTrackId(int id) {
	if (id >= oamlData.tracksN)
		return -1;

	oamlData.curTrack = oamlData.tracks[id];
	oamlData.curTrack->Play();

	return 0;
}

int oamlPlayTrack(const char *name) {
	assert(name != NULL);

	printf("%s %s\n", __FUNCTION__, name);

	for (int i=0; i<oamlData.tracksN; i++) {
		if (strcmp(oamlData.tracks[i]->GetName(), name) == 0) {
			return oamlPlayTrackId(i);
		}
	}

	return -1;
}

void oamlMixToBuffer(void *buffer, int size) {
	uint64_t ms = GetTimeMs64();

	assert(buffer != NULL);
	assert(size != 0);

	if (oamlData.buffer == NULL)
		return;

//	printf("%s %d\n", __FUNCTION__, size);
	if (oamlData.curTrack) {
		oamlTrack *track = oamlData.curTrack;

		track->Read(oamlData.buffer, size);
	}

//	printf("%s: %d\n", __FUNCTION__, oamlData.buffer->bytesRemaining());
	if (oamlData.buffer->bytesRemaining() < 3)
		return;

//	oamlData.dbuffer->reserve(oamlData.dbuffer->size() + size*2);

	int *buffer32 = (int*)buffer;
	for (int i=0; i<size; i++) {
		int sample = oamlData.buffer->getInt();
		buffer32[i]+= sample;

		if (oamlData.dbuffer) {
			oamlData.dbuffer->putShort((short)sample);
		}
	}

	uint64_t delta = GetTimeMs64() - ms;
	if (delta > 10) {
//		printf("%s %d %lld\n", __FUNCTION__, size, delta);
	}
}

void oamlSetCondition(int id, int value) {
//	printf("%s %d %d\n", __FUNCTION__, id, value);

	if (oamlData.curTrack) {
		oamlTrack *track = oamlData.curTrack;

		track->SetCondition(id, value);
	}
}

void oamlUpdate() {
/*	if (oamlData.buffer <= 4096) {
		track->Read(oamlData.buffer);
	}*/
}

void oamlShutdown() {
	if (oamlData.dbuffer) {
		wavWriteToFile("out.wav", oamlData.dbuffer, 2, 44100, 2);
	}

	if (oamlData.path) {
		free(oamlData.path);
		oamlData.path = NULL;
	}
}
