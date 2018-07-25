//-----------------------------------------------------------------------------
// Copyright (c) 2015-2018 Marcelo Fernandez
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
	playCondAudio = -1;

	fadeIn = 0;
	fadeOut = 0;
	xfadeIn = 0;
	xfadeOut = 0;
	playingOrder = 0;
	maxPlayOrder = 0;

	tailPos = 0;

	curAudio = -1;
	tailAudio = -1;
	fadeAudio = -1;
}

oamlMusicTrack::~oamlMusicTrack() {
	ClearAudios(&introAudios);
	ClearAudios(&loopAudios);
	ClearAudios(&randAudios);
	ClearAudios(&condAudios);
}

oamlAudio *oamlMusicTrack::GetAudioByTypeId(int type, int id) {
	switch (type) {
		case 0:
			if (id >= 0 && (size_t)id < introAudios.size()) {
				return introAudios[id];
			}
			break;
		case 1:
			if (id >= 0 && (size_t)id < loopAudios.size()) {
				return loopAudios[id];
			}
			break;
		case 2:
			if (id >= 0 && (size_t)id < randAudios.size()) {
				return randAudios[id];
			}
			break;
		case 3:
			if (id >= 0 && (size_t)id < condAudios.size()) {
				return condAudios[id];
			}
			break;
	}

	return NULL;
}

oamlAudio *oamlMusicTrack::GetCurAudio() {
	return GetAudioByTypeId(curAudio >> 24, curAudio & 0xFFFFFF);
}

oamlAudio *oamlMusicTrack::GetFadeAudio() {
	return GetAudioByTypeId(fadeAudio >> 24, fadeAudio & 0xFFFFFF);
}

oamlAudio *oamlMusicTrack::GetTailAudio() {
	return GetAudioByTypeId(tailAudio >> 24, tailAudio & 0xFFFFFF);
}

void oamlMusicTrack::SetCurAudio(int type, int id) {
	curAudio = (type << 24) | (id & 0xFFFFFF);
}

void oamlMusicTrack::AddAudio(oamlAudio *audio) {
	ASSERT(audio != NULL);

	if (GetAudio(audio->GetName())) {
		fprintf(stderr, "oaml: Warning, duplicated audio name: %s\n", audio->GetName().c_str());
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

		if (playing == true && curAudio == -1) {
			// If track is currently playing but no audio is actually playing this to play one
			PlayNext();
		}
		return;
	}

	oamlAudio *pcurAudio = GetCurAudio();
	for (size_t i=0; i<condAudios.size(); i++) {
		oamlAudio *audio = condAudios[i];
		if (audio->GetCondId() != id)
			continue;

		if (pcurAudio != audio) {
			// Audio isn't being played right now
			if (audio->TestCondition(id, value) == true) {
				// Condition is true, so let's play the audio
				if (pcurAudio == NULL || pcurAudio->GetMinMovementBars() == 0) {
					PlayCond((3 << 24) | i);
				} else {
					PlayCondWithMovement((3 << 24) | i);
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
		if (pcurAudio == NULL || pcurAudio->GetMinMovementBars() == 0) {
			PlayCond(-1);
		} else {
			PlayCondWithMovement(-1);
		}
	}
}

void oamlMusicTrack::PlayCondWithMovement(int audio) {
	oamlAudio *pcurAudio = GetCurAudio();
	if (pcurAudio == NULL)
		return;

	playCondAudio = audio;
	playCondSamples = pcurAudio->GetBarsSamples(pcurAudio->GetMinMovementBars());
	if (playCondSamples == 0)
		return;

	playCondSamples = (playCondSamples + pcurAudio->GetBarsSamples(pcurAudio->GetSamplesCount() / playCondSamples) * pcurAudio->GetMinMovementBars()) - pcurAudio->GetSamplesCount();
//	printf("%s %d\n", __FUNCTION__, playCondSamples);
}

void oamlMusicTrack::PlayCond(int audio) {
	fadeAudio = curAudio;
	curAudio = audio;

	oamlAudio *pcurAudio = GetCurAudio();
	if (pcurAudio == NULL) {
		PlayNext();
	} else {
		pcurAudio->Open();
		XFadePlay();
	}
}

oamlRC oamlMusicTrack::Play(int mainCondValue) {
	int doFade = 0;

	if (lock > 0) {
		return OAML_ERROR;
	}

	if (verbose) __oamlLog("%s %s\n", __FUNCTION__, GetNameStr());
	fadeAudio = -1;

	if (curAudio == -1) {
		doFade = 1;
	}

	printf("mainCondValue=%d\n", mainCondValue);
	SetCondition(OAML_CONDID_MAIN_LOOP, mainCondValue);

	playingOrder = 0;
	maxPlayOrder = 0;

	// Check if we need to use the playOrder audio property
	size_t count = 0;
	for (std::vector<oamlAudio*>::iterator it=loopAudios.begin(); it<loopAudios.end(); ++it) {
		oamlAudio *audio = *it;
		int playOrder = audio->GetPlayOrder();
		if (playOrder != 0) {
			if (playOrder > maxPlayOrder) {
				maxPlayOrder = playOrder;
			}
			count++;
		}
	}

	if (count == loopAudios.size()) {
		// All of our loop audios have the playOrder property assigned, enable its use
		playingOrder = 1;
	}

	if (introAudios.size() >= 1) {
		if (introAudios.size() == 1) {
			SetCurAudio(0, 0);
		} else {
			int i = Random(0, introAudios.size()-1);
			SetCurAudio(0, i);
		}

		oamlAudio *audio = GetCurAudio();
		if (audio) {
			audio->Open();
		}
	} else {
		PlayNext();
	}

	oamlAudio *pcurAudio = GetCurAudio();
	if (doFade && pcurAudio != NULL) {
		// First check the fade in property for the audio and then the track fade in property
		if (pcurAudio->GetFadeIn()) {
			pcurAudio->DoFadeIn(pcurAudio->GetFadeIn());
		} else if (fadeIn) {
			pcurAudio->DoFadeIn(fadeIn);
		}
	}

	playing = true;

	return OAML_OK;
}

int oamlMusicTrack::PickNextAudio() {
	if (verbose) __oamlLog("%s %s\n", __FUNCTION__, GetNameStr());

	oamlAudio *pcurAudio = GetCurAudio();
	if (randAudios.size() > 0 && (pcurAudio == NULL || pcurAudio->GetRandomChance() == 0)) {
		for (size_t i=0; i<randAudios.size(); i++) {
			int chance = randAudios[i]->GetRandomChance();
			if (Random(0, 100) > chance) {
				continue;
			} else {
				return (3 << 24) | (i & 0xFFFFFF);
			}
		}
	}

	if (loopAudios.size() == 1) {
		return (1 << 24) | (0 & 0xFFFFFF);
	} else if (loopAudios.size() >= 2) {
		std::vector<int> list;

		for (size_t i=0; i<loopAudios.size(); i++) {
			oamlAudio *audio = loopAudios[i];
			if (audio->IsPickable()) {
				if (playingOrder != 0 && audio->GetPlayOrder() != playingOrder) {
					continue;
				}
				list.push_back((1 << 24) | (i & 0xFFFFFF));
			}
		}

		if (playingOrder != 0) {
			playingOrder++;
			if (playingOrder > maxPlayOrder) {
				playingOrder = 1;
			}
		}

		if (list.size() == 0) {
			return -1;
		} else if (list.size() == 1) {
			return list[0];
		} else {
			int r = Random(0, list.size()-1);
			while (curAudio == list[r]) {
				r = Random(0, list.size()-1);
			}

			return list[r];
		}
	}

	return -1;
}

void oamlMusicTrack::PlayNext() {
	if (verbose) __oamlLog("%s %s\n", __FUNCTION__, GetNameStr());

	oamlAudio *pcurAudio = GetCurAudio();
	if (pcurAudio) {
		if (pcurAudio->GetType() == 4) {
			tailAudio = curAudio;
			tailPos = pcurAudio->GetSamplesCount();

			pcurAudio->Open();
			return;
		}
	}

	if (fadeAudio == -1)
		fadeAudio = curAudio;

	curAudio = PickNextAudio();
	pcurAudio = GetCurAudio();
	if (pcurAudio)
		pcurAudio->Open();

	if (fadeAudio != curAudio) {
		XFadePlay();
	} else {
		fadeAudio = -1;
	}
}

void oamlMusicTrack::XFadePlay() {
	oamlAudio *pcurAudio = GetCurAudio();
	oamlAudio *pfadeAudio = GetFadeAudio();
	if (pcurAudio) {
		// First check the fade in property for the audio and then the track fade in property
		if (pcurAudio->GetXFadeIn()) {
			pcurAudio->DoFadeIn(pcurAudio->GetXFadeIn());
		} else if (pfadeAudio && pfadeAudio->GetXFadeIn()) {
			pcurAudio->DoFadeIn(pfadeAudio->GetXFadeIn());
		} else if (xfadeIn) {
			pcurAudio->DoFadeIn(xfadeIn);
		}
	}

	if (pfadeAudio) {
		if (pcurAudio && pcurAudio->GetXFadeOut()) {
			pfadeAudio->DoFadeOut(pcurAudio->GetXFadeOut());
		} else if (pfadeAudio && pfadeAudio->GetXFadeOut()) {
			pfadeAudio->DoFadeOut(pfadeAudio->GetXFadeOut());
		} else if (xfadeOut) {
			pfadeAudio->DoFadeOut(xfadeOut);
		} else {
			fadeAudio = -1;
		}
	}
}

void oamlMusicTrack::Mix(float *samples, int channels, bool debugClipping) {
	if (curAudio == -1 && tailAudio == -1 && fadeAudio == -1)
		return;

	lock++;

	oamlAudio *pcurAudio = GetCurAudio();
	if (pcurAudio) {
		MixAudio(pcurAudio, samples, channels, debugClipping);
	}

	oamlAudio *ptailAudio = GetTailAudio();
	if (ptailAudio) {
		tailPos = MixAudio(ptailAudio, samples, channels, debugClipping, tailPos);
		if (ptailAudio->HasFinishedTail(tailPos))
			tailAudio = -1;
	}

	oamlAudio *pfadeAudio = GetFadeAudio();
	if (pfadeAudio) {
		MixAudio(pfadeAudio, samples, channels, debugClipping);
	}

	if (pcurAudio && pcurAudio->HasFinished()) {
		tailAudio = curAudio;
		tailPos = pcurAudio->GetSamplesCount();

		PlayNext();
	}

	if (pfadeAudio && pfadeAudio->HasFinished()) {
		fadeAudio = -1;
	}

	if (playCondSamples > 0) {
		playCondSamples--;
		if (playCondSamples == 0) {
			PlayCond(playCondAudio);
		}
	}

	if (curAudio == -1 && tailAudio == -1 && fadeAudio == -1) {
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

	if (curAudio == -1 && tailAudio == -1 && fadeAudio == -1) {
		if (playing == true) {
			return "Playing track but no available audio, missing condition?";
		}

		return info;
	}

	info+= GetName() + ":";

	oamlAudio *audio = GetCurAudio();
	if (audio) {
		snprintf(str, 1024, " curAudio = %s (pos=%d)", audio->GetName().c_str(), audio->GetSamplesCount());
		info+= str;
	}

	audio = GetTailAudio();
	if (audio) {
		snprintf(str, 1024, " tailAudio = %s (pos=%d)", audio->GetName().c_str(), audio->GetSamplesCount());
		info+= str;
	}

	audio = GetFadeAudio();
	if (audio) {
		snprintf(str, 1024, " fadeAudio = %s (pos=%d)", audio->GetName().c_str(), audio->GetSamplesCount());
		info+= str;
	}

	return info;
}

void oamlMusicTrack::Stop() {
	if (curAudio != -1) {
		if (fadeOut) {
			fadeAudio = curAudio;
			oamlAudio *audio = GetFadeAudio();
			if (audio) {
				audio->DoFadeOut(fadeOut);
			}
		}
		curAudio = -1;
	}

	tailAudio = -1;
	playing = false;

	if (curAudio == -1 && tailAudio == -1 && fadeAudio == -1) {
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

	if (filesSamples == 0) {
		return -1.f;
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

void oamlMusicTrack::SaveAudioState(tinyxml2::XMLDocument &doc, tinyxml2::XMLElement *node, const char *nodeName, std::vector<oamlAudio*> *audios) {
	for (std::vector<oamlAudio*>::iterator it=audios->begin(); it<audios->end(); ++it) {
		oamlAudio *audio = *it;

		tinyxml2::XMLElement *elem = doc.NewElement(nodeName);
		audio->SaveState(elem);
		node->InsertEndChild(elem);
	}
}

void oamlMusicTrack::SaveState(tinyxml2::XMLDocument &doc, tinyxml2::XMLElement *node) {
	node->SetAttribute("name", GetNameStr());
	node->SetAttribute("playing", playing);
	node->SetAttribute("playingOrder", playingOrder);
	node->SetAttribute("tailPos", tailPos);
	node->SetAttribute("curAudio", curAudio);
	node->SetAttribute("fadeAudio", fadeAudio);
	node->SetAttribute("tailAudio", tailAudio);
	node->SetAttribute("playCondAudio", playCondAudio);
	node->SetAttribute("playCondSamples", playCondSamples);

	if (introAudios.size() > 0) {
		SaveAudioState(doc, node, "introAudio", &introAudios);
	}
	if (loopAudios.size() > 0) {
		SaveAudioState(doc, node, "loopAudio", &loopAudios);
	}
	if (randAudios.size() > 0) {
		SaveAudioState(doc, node, "randAudio", &randAudios);
	}
	if (condAudios.size() > 0) {
		SaveAudioState(doc, node, "condAudio", &condAudios);
	}
}

void oamlMusicTrack::LoadAudioState(tinyxml2::XMLElement *node, std::vector<oamlAudio*> *audios) {
	for (std::vector<oamlAudio*>::iterator it=audios->begin(); it<audios->end(); ++it) {
		oamlAudio *audio = *it;
		if (strcmp(node->Attribute("name"), audio->GetName().c_str()) == 0) {
			audio->LoadState(node);
			break;
		}
	}
}

void oamlMusicTrack::LoadState(tinyxml2::XMLElement *node) {
	const char *attr = node->Attribute("playing");
	if (attr && strcmp(attr, "true") == 0) {
		playing = true;
	} else {
		playing = false;
	}

	playingOrder = node->IntAttribute("playingOrder");
	tailPos = node->IntAttribute("tailPos");
	curAudio = node->IntAttribute("curAudio");
	fadeAudio = node->IntAttribute("fadeAudio");
	tailAudio = node->IntAttribute("tailAudio");
	playCondAudio = node->IntAttribute("playCondAudio");
	playCondSamples = node->IntAttribute("playCondSamples");

	tinyxml2::XMLElement *el = node->FirstChildElement();
	while (el != NULL) {
		if (strcmp(el->Name(), "introAudio") == 0) {
			LoadAudioState(el, &introAudios);
		} else if (strcmp(el->Name(), "loopAudio") == 0) {
			LoadAudioState(el, &loopAudios);
		} else if (strcmp(el->Name(), "randAudio") == 0) {
			LoadAudioState(el, &randAudios);
		} else if (strcmp(el->Name(), "condAudio") == 0) {
			LoadAudioState(el, &condAudios);
		}

		el = el->NextSiblingElement();
	}
}

void oamlMusicTrack::FreeMemory() {
	FreeAudiosMemory(&introAudios);
	FreeAudiosMemory(&loopAudios);
	FreeAudiosMemory(&randAudios);
	FreeAudiosMemory(&condAudios);

	filesSamples = 0;
}

void oamlMusicTrack::GetAudioList(std::vector<std::string>& list) {
	FillAudiosList(&introAudios, list);
	FillAudiosList(&loopAudios, list);
	FillAudiosList(&randAudios, list);
	FillAudiosList(&condAudios, list);
}

void oamlMusicTrack::_SetLayerGain(std::vector<oamlAudio*> *audios, std::string layer, float gain) {
	for (std::vector<oamlAudio*>::iterator it=audios->begin(); it<audios->end(); ++it) {
		oamlAudio *audio = *it;
		audio->SetLayerGain(layer, gain);
	}
}

void oamlMusicTrack::SetLayerGain(std::string layer, float gain) {
	_SetLayerGain(&introAudios, layer, gain);
	_SetLayerGain(&loopAudios, layer, gain);
	_SetLayerGain(&randAudios, layer, gain);
	_SetLayerGain(&condAudios, layer, gain);
}
