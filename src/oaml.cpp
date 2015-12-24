#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "oamlCommon.h"

oamlData::oamlData() {
	debug = 0;
	tracksN = 0;

	curTrack = NULL;

	dataBuffer = NULL;
	dbuffer = NULL;

	freq = 0;
	channels = 0;
	bytesPerSample = 0;

	volume = 255;

	timeMs = 0;
	tension = 0;
	tensionMs = 0;
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
		int beatsPerBar;
		int xfadeIn;
		int xfadeOut;
		int ret = sscanf(str, "%d %f %d %d %s %d %d", &mode, &bpm, &beatsPerBar, &audioCount, name, &xfadeIn, &xfadeOut);
		if (ret != 7)
			break;

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
			if (sscanf(str, "%d %d %s %d %d %d %d %d", &type, &bars, filename, &fadeIn, &fadeOut, &condId, &condType, &condValue) >= 3) {
				sprintf(fname, "%s%s", pathToMusic, filename);
				oamlAudio *audio = new oamlAudio(fname, type, bars, bpm, beatsPerBar, fadeIn, fadeOut);
				if (condId != -1) {
					audio->SetCondition(condId, condType, condValue);
				}

				track->AddAudio(audio);
			}
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
	ASSERT(name != NULL);

	printf("%s %s\n", __FUNCTION__, name);

	for (int i=0; i<tracksN; i++) {
		if (strcmp(tracks[i]->GetName(), name) == 0) {
			return PlayTrackId(i);
		}
	}

	return -1;
}

bool oamlData::IsTrackPlaying(const char *name) {
	ASSERT(name != NULL);

	for (int i=0; i<tracksN; i++) {
		if (strcmp(tracks[i]->GetName(), name) == 0) {
			return IsTrackPlayingId(i);
		}
	}

	return false;
}

bool oamlData::IsTrackPlayingId(int id) {
	if (id >= tracksN)
		return false;

	return tracks[id]->IsPlaying();
}

bool oamlData::IsPlaying() {
	for (int i=0; i<tracksN; i++) {
		if (tracks[i]->IsPlaying())
			return true;
	}

	return false;
}

void oamlData::StopPlaying() {
	for (int i=0; i<tracksN; i++) {
		if (tracks[i]->IsPlaying()) {
			tracks[i]->Stop();
			break;
		}
	}
}

void oamlData::MixToBuffer(void *buffer, int size) {
	uint64_t ms = GetTimeMs64();

	ASSERT(buffer != NULL);
	ASSERT(size != 0);

	if (dataBuffer == NULL)
		return;

//	printf("%s %d\n", __FUNCTION__, size);
	if (curTrack) {
		curTrack->Read(dataBuffer, size);
	}

//	printf("%s: %d\n", __FUNCTION__, dataBuffer->bytesRemaining());
	if (dataBuffer->bytesRemaining() < 3)
		return;

	int *buffer32 = (int*)buffer;
	for (int i=0; i<size; i++) {
		int sample = dataBuffer->getInt();
		sample = (sample * volume) / 255;
		buffer32[i]+= sample;

		if (dbuffer) {
			dbuffer->putShort((short)sample);
		}
	}

	uint64_t delta = GetTimeMs64() - ms;
	if (delta > 10) {
//		printf("%s %d %lld\n", __FUNCTION__, size, delta);
	}

	Update();
}

void oamlData::SetCondition(int id, int value) {
//	printf("%s %d %d\n", __FUNCTION__, id, value);
	if (curTrack) {
		curTrack->SetCondition(id, value);
	}
}

void oamlData::SetVolume(int vol) {
	volume = vol;

	if (volume < OAML_VOLUME_MIN) volume = OAML_VOLUME_MIN;
	if (volume > OAML_VOLUME_MAX) volume = OAML_VOLUME_MAX;
}

void oamlData::AddTension(int value) {
	tension+= value;
	if (tension >= 100) {
		tension = 100;
	}
}

void oamlData::Update() {
	uint64_t ms = GetTimeMs64();

	// Update each second
	if (ms >= (timeMs + 1000)) {
//		printf("%s %d %lld %d\n", __FUNCTION__, tension, tensionMs - ms, ms >= (tensionMs + 5000));
		// Don't allow sudden changes of tension after it changed back to 0
		if (tension > 0) {
			SetCondition(1, tension);
			tensionMs = ms;
		} else {
			if (ms >= (tensionMs + 5000)) {
				SetCondition(1, tension);
				tensionMs = ms;
			}
		}

		// Lower tension
		if (tension >= 1) {
			tension-= (tension+20)/10;
			if (tension < 0)
				tension = 0;
		}

		timeMs = ms;
	}

/*	if (buffer <= 4096) {
		track->Read(buffer);
	}*/
}

void oamlData::Shutdown() {
	printf("%s\n", __FUNCTION__);
	if (dbuffer) {
		wavWriteToFile("out.wav", dbuffer, 2, 44100, 2);
	}
}
