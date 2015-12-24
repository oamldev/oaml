#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "oamlCommon.h"


oamlTrack::oamlTrack(const char *trackName, int trackMode, float trackBpm, int trackXfadeIn, int trackXfadeOut) {
	ASSERT(trackName != NULL);

	strcpy(name, trackName);
	mode = trackMode;
	bpm = trackBpm;

	xfadeIn = trackXfadeIn;
	xfadeOut = trackXfadeOut;

	loopCount = 0;
	condCount = 0;

	introAudio = NULL;
	endAudio = NULL;

	curAudio = NULL;
	tailAudio = NULL;
	fadeAudio = NULL;
}

oamlTrack::~oamlTrack() {
}

void oamlTrack::AddAudio(oamlAudio *audio) {
	ASSERT(audio != NULL);

	if (audio->GetType() == 1) {
		introAudio = audio;
	} else if (audio->GetType() == 3) {
		endAudio = audio;
	} else if (audio->GetType() == 4) {
		condAudios[condCount++] = audio;
	} else {
		loopAudios[loopCount++] = audio;
	}
}

void oamlTrack::SetCondition(int id, int value) {
	for (int i=0; i<condCount; i++) {
		oamlAudio *audio = condAudios[i];
		if (curAudio != audio) {
			// Audio isn't being played right now
			if (audio->TestCondition(id, value) == true) {
				// Condition is true, so let's play the audio
				tailAudio = curAudio;
				curAudio = audio;
				curAudio->Open();

				XFadePlay();
			}
		} else {
			// Audio is being played right now
			if (audio->TestCondition(id, value) == false) {
				// Condition is false, so let's stop the audio
				tailAudio = curAudio;
				curAudio = NULL;
				PlayNext();

				XFadePlay();
			}
		}
	}
}

void oamlTrack::Play() {
	if (introAudio) {
		curAudio = introAudio;
		curAudio->Open();
	} else {
		PlayNext();
	}
}

int oamlTrack::Random(int min, int max) {
	int range = max - min + 1;
	return rand() % range + min;
}

void oamlTrack::PlayNext() {
	if (curAudio) {
		if (curAudio->GetType() == 4) {
			curAudio->Open();
			tailAudio = NULL;
			return;
		}
	}

	if (loopCount == 1) {
		curAudio = loopAudios[0];
	} else if (loopCount == 2) {
		if (curAudio == loopAudios[0]) {
			curAudio = loopAudios[1];
		} else {
			curAudio = loopAudios[0];
		}
	} else if (loopCount >= 3) {
		int r = Random(0, loopCount-1);
		while (curAudio == loopAudios[r]) {
			r = Random(0, loopCount-1);
		}

		curAudio = loopAudios[r];
	}

	if (curAudio)
		curAudio->Open();
}

void oamlTrack::XFadePlay() {
	if (curAudio)
		curAudio->DoFadeIn(xfadeIn);
	if (tailAudio)
		tailAudio->DoFadeOut(xfadeOut);
}

int oamlTrack::Read(ByteBuffer *buffer, int size) {
	if (curAudio == NULL && tailAudio == NULL && fadeAudio == NULL)
		return 0;

	buffer->clear();
	for (int i=0; i<size; i++) {
		int sample = 0;

		if (curAudio) {
			sample+= curAudio->Read32();
		}

		if (tailAudio) {
			sample+= tailAudio->Read32();
		}

		if (fadeAudio) {
			sample+= fadeAudio->Read32();
		}
		sample>>= 16;
		if (sample > 32767) sample = 32767;
		if (sample < -32768) sample = -32768;

		buffer->putInt(sample);

		if (curAudio && curAudio->HasFinished()) {
			tailAudio = curAudio;

			PlayNext();
		}

		if (fadeAudio && fadeAudio->HasFinished()) {
			fadeAudio = NULL;
		}
	}

	return size;
}

bool oamlTrack::IsPlaying() {
	return (curAudio != NULL || tailAudio != NULL);
}

void oamlTrack::Stop() {
	if (curAudio) {
		fadeAudio = curAudio;
		fadeAudio->DoFadeOut(1000);
		curAudio = NULL;
	}

	tailAudio = NULL;
}
