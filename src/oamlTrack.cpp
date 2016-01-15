#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "oamlCommon.h"


oamlTrack::oamlTrack() {
	memset(name, 0, sizeof(name));
	mode = 0;

	playCondSamples = 0;
	playCondAudio = NULL;

	fadeIn = 0;
	fadeOut = 0;
	xfadeIn = 0;
	xfadeOut = 0;

	loopCount = 0;
	condCount = 0;
	randCount = 0;

	tailPos = 0;

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
	} else if (audio->GetRandomChance() > 0) {
		randAudios[randCount++] = audio;
	} else {
		loopAudios[loopCount++] = audio;
	}
}

void oamlTrack::SetCondition(int id, int value) {
	bool stopCond = false;
	bool playCond = false;

	if (playCondSamples > 0)
		return;

	if (id == CONDITION_MAIN_LOOP) {
		for (int i=0; i<loopCount; i++) {
			oamlAudio *audio = loopAudios[i];
			audio->SetPickable(audio->TestCondition(0, value));
		}
		return;
	}

	for (int i=0; i<condCount; i++) {
		oamlAudio *audio = condAudios[i];
		if (audio->GetCondId() != id)
			continue;

		if (curAudio != audio) {
			// Audio isn't being played right now
			if (audio->TestCondition(id, value) == true) {
				// Condition is true, so let's play the audio
				if (curAudio == NULL || curAudio->GetMinMovementBars() == 0) {
					PlayCond(audio);
				} else {
					PlayCondWithMovement(audio);
				}

				playCond = true;
			}
		} else {
			// Audio is being played right now
			if (audio->TestCondition(id, value) == false) {
				stopCond = true;
			}
		}
	}

	if (stopCond == true && playCond == false) {
		// No condition is being played now, let's go back to the main loop
		if (curAudio == NULL || curAudio->GetMinMovementBars() == 0) {
			PlayCond(NULL);
		} else {
			PlayCondWithMovement(NULL);
		}
	}
}

void oamlTrack::PlayCondWithMovement(oamlAudio *audio) {
	playCondAudio = audio;
	playCondSamples = curAudio->GetBarsSamples(curAudio->GetMinMovementBars());
	if (playCondSamples == 0)
		return;

	playCondSamples = (playCondSamples + curAudio->GetBarsSamples(curAudio->GetSamplesCount() / playCondSamples) * curAudio->GetMinMovementBars()) - curAudio->GetSamplesCount();
//	printf("%s %d\n", __FUNCTION__, playCondSamples);
}

void oamlTrack::PlayCond(oamlAudio *audio) {
	fadeAudio = curAudio;
	curAudio = audio;
	if (curAudio == NULL) {
		PlayNext();
	} else {
		curAudio->Open();
		XFadePlay();
	}
}

void oamlTrack::Play() {
	int doFade = 0;

//	printf("%s %s\n", __FUNCTION__, name);
	fadeAudio = NULL;

	if (curAudio == NULL) {
		doFade = 1;
	}

	if (introAudio) {
		curAudio = introAudio;
		curAudio->Open();
	} else {
		PlayNext();
	}

	if (doFade && curAudio) {
		// First check the fade in property for the audio and then the track fade in property
		if (curAudio->GetFadeIn()) {
			curAudio->DoFadeIn(curAudio->GetFadeIn());
		} else if (fadeIn) {
			curAudio->DoFadeIn(fadeIn);
		}
	}
}

int oamlTrack::Random(int min, int max) {
	int range = max - min + 1;
	return rand() % range + min;
}

void oamlTrack::ShowInfo() {
	printf("%s %d %d %d\n", name, loopCount, randCount, condCount);
}

oamlAudio* oamlTrack::PickNextAudio() {
	if (randCount > 0 && (curAudio == NULL || curAudio->GetRandomChance() == 0)) {
		for (int i=0; i<randCount; i++) {
			int chance = randAudios[i]->GetRandomChance();
			if (Random(0, 100) > chance) {
				continue;
			} else {
				return randAudios[i];
			}
		}
	}

	if (loopCount == 1) {
		return loopAudios[0];
	} else if (loopCount >= 2) {
		oamlAudio *list[256];
		int count = 0;

		for (int i=0; i<loopCount; i++) {
			oamlAudio *audio = loopAudios[i];
			if (audio->IsPickable())
				list[count++] = audio;
		}

		if (count == 1) {
			return list[0];
		} else {
			int r = Random(0, count-1);
			while (curAudio == list[r]) {
				r = Random(0, count-1);
			}

			return list[r];
		}
	}

	return NULL;
}

void oamlTrack::PlayNext() {
	if (curAudio) {
		if (curAudio->GetType() == 4) {
			tailAudio = curAudio;
			tailPos = curAudio->GetSamplesCount();

			curAudio->Open();
			return;
		}
	}

	if (fadeAudio == NULL)
		fadeAudio = curAudio;

	curAudio = PickNextAudio();
	if (curAudio)
		curAudio->Open();

	if (fadeAudio != curAudio) {
		XFadePlay();
	}else {
		fadeAudio = NULL;
	}
}

void oamlTrack::XFadePlay() {
	if (curAudio) {
		// First check the fade in property for the audio and then the track fade in property
		if (curAudio->GetXFadeIn()) {
			curAudio->DoFadeIn(curAudio->GetXFadeIn());
		} else if (fadeAudio && fadeAudio->GetXFadeIn()) {
			curAudio->DoFadeIn(fadeAudio->GetXFadeIn());
		} else if (xfadeIn) {
			curAudio->DoFadeIn(xfadeIn);
		}
	}

	if (fadeAudio) {
		if (curAudio && curAudio->GetXFadeOut()) {
			fadeAudio->DoFadeOut(curAudio->GetXFadeOut());
		} else if (fadeAudio && fadeAudio->GetXFadeOut()) {
			fadeAudio->DoFadeOut(fadeAudio->GetXFadeOut());
		} else if (xfadeOut) {
			fadeAudio->DoFadeOut(xfadeOut);
		} else {
			fadeAudio = NULL;
		}
	}
}

int oamlTrack::Mix32(int sample, oamlBase *oaml) {
	if (curAudio == NULL && tailAudio == NULL && fadeAudio == NULL)
		return sample;

	if (curAudio) {
		sample = oaml->SafeAdd(sample, curAudio->Read32());
	}

	if (tailAudio) {
		sample = oaml->SafeAdd(sample, tailAudio->Read32(tailPos++));
		if (tailAudio->HasFinishedTail(tailPos))
			tailAudio = NULL;
	}

	if (fadeAudio) {
		sample = oaml->SafeAdd(sample, fadeAudio->Read32());
	}

	if (curAudio && curAudio->HasFinished()) {
		tailAudio = curAudio;
		tailPos = curAudio->GetSamplesCount();

		PlayNext();
	}

	if (fadeAudio && fadeAudio->HasFinished()) {
		fadeAudio = NULL;
	}

	if (playCondSamples > 0) {
		playCondSamples--;
		if (playCondSamples == 0) {
			PlayCond(playCondAudio);
		}
	}

	return sample;
}

bool oamlTrack::IsPlaying() {
	return (curAudio != NULL || tailAudio != NULL);
}

void oamlTrack::ShowPlaying() {
	char str[1024] = "";

	if (curAudio) {
		snprintf(str, 1024, "%s curAudio = %s (pos=%d)", str, curAudio->GetFilename(), curAudio->GetSamplesCount());
	}

	if (tailAudio) {
		snprintf(str, 1024, "%s tailAudio = %s (pos=%d)", str, tailAudio->GetFilename(), tailAudio->GetSamplesCount());
	}

	if (fadeAudio) {
		snprintf(str, 1024, "%s fadeAudio = %s (pos=%d)", str, fadeAudio->GetFilename(), fadeAudio->GetSamplesCount());
	}

	if (strlen(str) > 0) {
		printf("%s: %s\n", GetName(), str);
	}
}

void oamlTrack::Stop() {
	if (curAudio) {
		if (fadeOut) {
			fadeAudio = curAudio;
			fadeAudio->DoFadeOut(fadeOut);
		}
		curAudio = NULL;
	}

	tailAudio = NULL;
}

