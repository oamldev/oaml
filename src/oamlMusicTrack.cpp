//-----------------------------------------------------------------------------
// Copyright (c) 2015-2016 Marcelo Fernandez
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "oamlCommon.h"


oamlMusicTrack::oamlMusicTrack(bool _verbose) {
	verbose = _verbose;
	name = "Track";
	playing = false;
	filesSamples = 0;

	playCondSamples = 0;
	playCondAudio = NULL;

	fadeIn = 0;
	fadeOut = 0;
	xfadeIn = 0;
	xfadeOut = 0;

	tailPos = 0;

	curAudio = NULL;
	tailAudio = NULL;
	fadeAudio = NULL;
}

oamlMusicTrack::~oamlMusicTrack() {
	ClearAudios(&introAudios);
	ClearAudios(&loopAudios);
	ClearAudios(&randAudios);
	ClearAudios(&condAudios);
}

void oamlMusicTrack::AddAudio(oamlAudio *audio) {
	ASSERT(audio != NULL);

	if (GetAudio(audio->GetName())) {
		printf("Duplicated audio name: %s\n", audio->GetName().c_str());
	}

	if (audio->GetType() == 1) {
		introAudios.push_back(audio);
	} else if (audio->GetType() == 4) {
		condAudios.push_back(audio);
	} else if (audio->GetRandomChance() > 0) {
		randAudios.push_back(audio);
	} else {
		loopAudios.push_back(audio);
	}
}

oamlAudio* oamlMusicTrack::GetAudio(std::string filename) {
	oamlAudio *audio;

	audio = FindAudio(&introAudios, filename);
	if (audio) return audio;
	audio = FindAudio(&loopAudios, filename);
	if (audio) return audio;
	audio = FindAudio(&randAudios, filename);
	if (audio) return audio;
	audio = FindAudio(&condAudios, filename);
	if (audio) return audio;

	return NULL;
}

oamlRC oamlMusicTrack::RemoveAudio(std::string filename) {
	if (FindAudioAndRemove(&introAudios, filename) == OAML_OK)
		return OAML_OK;
	if (FindAudioAndRemove(&loopAudios, filename) == OAML_OK)
		return OAML_OK;
	if (FindAudioAndRemove(&randAudios, filename) == OAML_OK)
		return OAML_OK;
	if (FindAudioAndRemove(&condAudios, filename) == OAML_OK)
		return OAML_OK;
	return OAML_NOT_FOUND;
}

void oamlMusicTrack::SetCondition(int id, int value) {
	bool stopCond = false;
	bool playCond = false;

	if (playCondSamples > 0)
		return;

	if (id == OAML_CONDID_MAIN_LOOP) {
		for (size_t i=0; i<loopAudios.size(); i++) {
			oamlAudio *audio = loopAudios[i];
			if (audio->HasCondition(id)) {
				audio->SetPickable(audio->TestCondition(id, value));
			}
		}

		if (playing == true && curAudio == NULL) {
			// If track is currently playing but no audio is actually playing this to play one
			PlayNext();
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

void oamlMusicTrack::PlayCondWithMovement(oamlAudio *audio) {
	playCondAudio = audio;
	playCondSamples = curAudio->GetBarsSamples(curAudio->GetMinMovementBars());
	if (playCondSamples == 0)
		return;

	playCondSamples = (playCondSamples + curAudio->GetBarsSamples(curAudio->GetSamplesCount() / playCondSamples) * curAudio->GetMinMovementBars()) - curAudio->GetSamplesCount();
//	printf("%s %d\n", __FUNCTION__, playCondSamples);
}

void oamlMusicTrack::PlayCond(oamlAudio *audio) {
	fadeAudio = curAudio;
	curAudio = audio;
	if (curAudio == NULL) {
		PlayNext();
	} else {
		curAudio->Open();
		XFadePlay();
	}
}

oamlRC oamlMusicTrack::Play() {
	int doFade = 0;

	if (lock > 0) {
		return OAML_ERROR;
	}

	if (verbose) __oamlLog("%s %s\n", __FUNCTION__, GetNameStr());
	fadeAudio = NULL;

	if (curAudio == NULL) {
		doFade = 1;
	}

	SetCondition(OAML_CONDID_MAIN_LOOP, 0);

	if (introAudios.size() >= 1) {
		if (introAudios.size() == 1) {
			curAudio = introAudios[0];
		} else {
			int i = Random(0, introAudios.size()-1);
			curAudio = introAudios[i];
		}
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

	playing = true;

	return OAML_OK;
}

oamlAudio* oamlMusicTrack::PickNextAudio() {
	if (verbose) __oamlLog("%s %s\n", __FUNCTION__, GetNameStr());

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

void oamlMusicTrack::PlayNext() {
	if (verbose) __oamlLog("%s %s\n", __FUNCTION__, GetNameStr());
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
	} else {
		fadeAudio = NULL;
	}
}

void oamlMusicTrack::XFadePlay() {
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

void oamlMusicTrack::Mix(float *samples, int channels, bool debugClipping) {
	if (curAudio == NULL && tailAudio == NULL && fadeAudio == NULL)
		return;

	lock++;

	if (curAudio) {
		MixAudio(curAudio, samples, channels, debugClipping);
	}

	if (tailAudio) {
		tailPos = MixAudio(tailAudio, samples, channels, debugClipping, tailPos);
		if (tailAudio->HasFinishedTail(tailPos))
			tailAudio = NULL;
	}

	if (fadeAudio) {
		MixAudio(fadeAudio, samples, channels, debugClipping);
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

	if (curAudio == NULL && tailAudio == NULL && fadeAudio == NULL) {
		FreeMemory();
	}

	lock--;
}

bool oamlMusicTrack::IsPlaying() {
	return playing;
}

std::string oamlMusicTrack::GetPlayingInfo() {
	char str[1024];
	std::string info = "";

	if (curAudio == NULL && tailAudio == NULL && fadeAudio == NULL) {
		if (playing == true) {
			return "Playing track but no available audio, missing condition?";
		}

		return info;
	}

	info+= GetName() + ":";

	if (curAudio) {
		snprintf(str, 1024, " curAudio = %s (pos=%d)", curAudio->GetName().c_str(), curAudio->GetSamplesCount());
		info+= str;
	}

	if (tailAudio) {
		snprintf(str, 1024, " tailAudio = %s (pos=%d)", tailAudio->GetName().c_str(), tailAudio->GetSamplesCount());
		info+= str;
	}

	if (fadeAudio) {
		snprintf(str, 1024, " fadeAudio = %s (pos=%d)", fadeAudio->GetName().c_str(), fadeAudio->GetSamplesCount());
		info+= str;
	}

	return info;
}

void oamlMusicTrack::Stop() {
	if (curAudio) {
		if (fadeOut) {
			fadeAudio = curAudio;
			fadeAudio->DoFadeOut(fadeOut);
		}
		curAudio = NULL;
	}

	tailAudio = NULL;
	playing = false;

	if (curAudio == NULL && tailAudio == NULL && fadeAudio == NULL) {
		FreeMemory();
	}
}

oamlRC oamlMusicTrack::Load() {
	for (std::vector<oamlAudio*>::iterator it=loopAudios.begin(); it<loopAudios.end(); ++it) {
		oamlAudio *audio = *it;
		oamlRC ret = audio->Load();
		if (ret != OAML_OK) return ret;
	}

	return OAML_OK;
}

float oamlMusicTrack::LoadProgress() {
	int samples;

	if (filesSamples == 0) {
		// Calculate the total size of the audio loops to read

		samples = GetFilesSamplesFor(&introAudios);
		if (samples == -1)
			return -1.f;
		filesSamples+= samples;

		samples = GetFilesSamplesFor(&loopAudios);
		if (samples == -1)
			return -1.f;
		filesSamples+= samples;

		samples = GetFilesSamplesFor(&randAudios);
		if (samples == -1)
			return -1.f;
		filesSamples+= samples;

		samples = GetFilesSamplesFor(&condAudios);
		if (samples == -1)
			return -1.f;
		filesSamples+= samples;
	}

	int totalSamplesRead = 0;

	samples = LoadProgressFor(&introAudios);
	if (samples == -1) {
		return -1.f;
	}
	totalSamplesRead+= samples;

	samples = LoadProgressFor(&loopAudios);
	if (samples == -1) {
		return -1.f;
	}
	totalSamplesRead+= samples;

	samples = LoadProgressFor(&randAudios);
	if (samples == -1) {
		return -1.f;
	}
	totalSamplesRead+= samples;

	samples = LoadProgressFor(&condAudios);
	if (samples == -1) {
		return -1.f;
	}
	totalSamplesRead+= samples;

	return float(double(totalSamplesRead) / double(filesSamples));
}

void oamlMusicTrack::ReadInfo(oamlTrackInfo *info) {
	oamlTrack::ReadInfo(info);

	ReadAudiosInfo(&introAudios, info);
	ReadAudiosInfo(&loopAudios, info);
	ReadAudiosInfo(&randAudios, info);
	ReadAudiosInfo(&condAudios, info);
}

void oamlMusicTrack::FreeMemory() {
	FreeAudiosMemory(&introAudios);
	FreeAudiosMemory(&loopAudios);
	FreeAudiosMemory(&randAudios);
	FreeAudiosMemory(&condAudios);
}

void oamlMusicTrack::GetAudioList(std::vector<std::string>& list) {
	FillAudiosList(&introAudios, list);
	FillAudiosList(&loopAudios, list);
	FillAudiosList(&randAudios, list);
	FillAudiosList(&condAudios, list);
}

