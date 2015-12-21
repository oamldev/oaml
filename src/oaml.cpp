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

oamlData::oamlData() {
	tracksN = 0;

	curTrack = NULL;

	dataBuffer = NULL;
	dbuffer = NULL;

	freq = 0;
	channels = 0;
	bytesPerSample = 0;
}

oamlData::~oamlData() {
}

int oamlData::Init(const char *pathToMusic) {
	FILE *f;
	char filename[1024];
	char str[1024];

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
		int xfadeIn;
		int xfadeOut;
		sscanf(str, "%d %f %d %s %d %d", &mode, &bpm, &audioCount, name, &xfadeIn, &xfadeOut);
		oamlTrack *track = new oamlTrack(name, mode, bpm, xfadeIn, xfadeOut);
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

		tracks[tracksN++] = track;
	}
	fclose(f);

	dataBuffer = new ByteBuffer();
	if (debug) {
		dbuffer = new ByteBuffer();
	}

	return 0;
}

void oamlData::SetAudioFormat(int audioFreq, int audioChannels, int audioBytesPerSample) {
	freq = audioFreq;
	channels = audioChannels;
	bytesPerSample = audioBytesPerSample;
}

int oamlData::PlayTrackId(int id) {
	if (id >= tracksN)
		return -1;

	curTrack = tracks[id];
	curTrack->Play();

	return 0;
}

int oamlData::PlayTrack(const char *name) {
	assert(name != NULL);

	printf("%s %s\n", __FUNCTION__, name);

	for (int i=0; i<tracksN; i++) {
		if (strcmp(tracks[i]->GetName(), name) == 0) {
			return PlayTrackId(i);
		}
	}

	return -1;
}

void oamlData::MixToBuffer(void *buffer, int size) {
	uint64_t ms = GetTimeMs64();

	assert(buffer != NULL);
	assert(size != 0);

	if (dataBuffer == NULL)
		return;

//	printf("%s %d\n", __FUNCTION__, size);
	if (curTrack) {
		oamlTrack *track = curTrack;

		track->Read(dataBuffer, size);
	}

//	printf("%s: %d\n", __FUNCTION__, buffer->bytesRemaining());
	if (dataBuffer->bytesRemaining() < 3)
		return;

	if (dbuffer) {
		dbuffer->reserve(dbuffer->size() + size*2);
	}

	int *buffer32 = (int*)buffer;
	for (int i=0; i<size; i++) {
		int sample = dataBuffer->getInt();
		buffer32[i]+= sample;

		if (dbuffer) {
			dbuffer->putShort((short)sample);
		}
	}

	uint64_t delta = GetTimeMs64() - ms;
	if (delta > 10) {
//		printf("%s %d %lld\n", __FUNCTION__, size, delta);
	}
}

void oamlData::SetCondition(int id, int value) {
//	printf("%s %d %d\n", __FUNCTION__, id, value);

	if (curTrack) {
		oamlTrack *track = curTrack;

		track->SetCondition(id, value);
	}
}

void oamlData::Update() {
/*	if (buffer <= 4096) {
		track->Read(buffer);
	}*/
}

void oamlData::Shutdown() {
	if (dbuffer) {
		wavWriteToFile("out.wav", dbuffer, 2, 44100, 2);
	}
}
