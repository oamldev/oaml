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
	}

	XFadePlay();
}

void oamlTrack::Play() {
	int doFade = 0;

//	printf("%s %s\n", __FUNCTION__, name);

	if (curAudio == NULL && fadeIn) {
		doFade = 1;
	}

	if (introAudio) {
		curAudio = introAudio;
		curAudio->Open();
	} else {
		PlayNext();
	}

	if (doFade && curAudio) {
		curAudio->DoFadeIn(fadeIn);
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

	if (randCount > 0) {
		for (int i=0; i<randCount; i++) {
			int chance = randAudios[i]->GetRandomChance();
			if (Random(0, 100) > chance) {
				continue;
			} else {
				curAudio = randAudios[i];
				curAudio->Open();
				return;
			}
		}
	}

	if (loopCount == 1) {
		curAudio = loopAudios[0];
	} else if (loopCount >= 2) {
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
	if (fadeAudio) {
		if (xfadeOut)
			fadeAudio->DoFadeOut(xfadeOut);
		else
			fadeAudio = NULL;
	}
}

int oamlTrack::Read32() {
	if (curAudio == NULL && tailAudio == NULL && fadeAudio == NULL)
		return 0;

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

	if (curAudio && curAudio->HasFinished()) {
		tailAudio = curAudio;

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
		sprintf(str, "%s curAudio = %s", str, curAudio->GetFilename());
	}

	if (tailAudio) {
		sprintf(str, "%s tailAudio = %s", str, tailAudio->GetFilename());
	}

	if (fadeAudio) {
		sprintf(str, "%s fadeAudio = %s", str, fadeAudio->GetFilename());
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
