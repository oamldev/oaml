#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "oamlCommon.h"


oamlTrack::oamlTrack() {
	name = "Track";
	mode = 0;

	playCondSamples = 0;
	playCondAudio = NULL;

	fadeIn = 0;
	fadeOut = 0;
	xfadeIn = 0;
	xfadeOut = 0;

	tailPos = 0;

	introAudio = NULL;
	endAudio = NULL;

	curAudio = NULL;
	tailAudio = NULL;
	fadeAudio = NULL;
}

void oamlTrack::ClearAudios(std::vector<oamlAudio*> *audios) {
	while (audios->empty() == false) {
		oamlAudio *audio = audios->back();
		audios->pop_back();

		delete audio;
	}
}

oamlTrack::~oamlTrack() {
	ClearAudios(&loopAudios);
	ClearAudios(&randAudios);
	ClearAudios(&condAudios);
}

void oamlTrack::AddAudio(oamlAudio *audio) {
	ASSERT(audio != NULL);

	if (audio->GetType() == 1) {
		introAudio = audio;
	} else if (audio->GetType() == 3) {
		endAudio = audio;
	} else if (audio->GetType() == 4) {
		condAudios.push_back(audio);
	} else if (audio->GetRandomChance() > 0) {
		randAudios.push_back(audio);
	} else {
		loopAudios.push_back(audio);
	}
}

void oamlTrack::SetCondition(int id, int value) {
	bool stopCond = false;
	bool playCond = false;

	if (playCondSamples > 0)
		return;

	if (id == CONDITION_MAIN_LOOP) {
		for (size_t i=0; i<loopAudios.size(); i++) {
			oamlAudio *audio = loopAudios[i];
			if (audio->HasCondition(id)) {
				audio->SetPickable(audio->TestCondition(id, value));
			}
		}
		return;
	}

	for (size_t i=0; i<condAudios.size(); i++) {
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
	printf("%s %ld %ld %ld\n", GetNameStr(), loopAudios.size(), randAudios.size(), condAudios.size());
}

oamlAudio* oamlTrack::PickNextAudio() {
	if (randAudios.size() > 0 && (curAudio == NULL || curAudio->GetRandomChance() == 0)) {
		for (size_t i=0; i<randAudios.size(); i++) {
			int chance = randAudios[i]->GetRandomChance();
			if (Random(0, 100) > chance) {
				continue;
			} else {
				return randAudios[i];
			}
		}
	}

	if (loopAudios.size() == 1) {
		return loopAudios[0];
	} else if (loopAudios.size() >= 2) {
		oamlAudio *list[256];
		int count = 0;

		for (size_t i=0; i<loopAudios.size(); i++) {
			oamlAudio *audio = loopAudios[i];
			if (audio->IsPickable())
				list[count++] = audio;
		}

		if (count == 0) {
			return NULL;
		} else if (count == 1) {
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
	std::string info;

	info = GetPlayingInfo();
	printf("%s\n", info.c_str());
}

std::string oamlTrack::GetPlayingInfo() {
	char str[1024];
	std::string info = "";

	if (curAudio == NULL && tailAudio == NULL && fadeAudio == NULL)
		return info;

	info+= GetName() + ":";

	if (curAudio) {
		snprintf(str, 1024, " curAudio = %s (pos=%d)", curAudio->GetFilenameStr(), curAudio->GetSamplesCount());
		info+= curAudio->GetFilename();
		info+= str;
	}

	if (tailAudio) {
		snprintf(str, 1024, " tailAudio = %s (pos=%d)", tailAudio->GetFilenameStr(), tailAudio->GetSamplesCount());
		info+= str;
	}

	if (fadeAudio) {
		snprintf(str, 1024, " fadeAudio = %s (pos=%d)", fadeAudio->GetFilenameStr(), fadeAudio->GetSamplesCount());
		info+= str;
	}

	return info;
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

