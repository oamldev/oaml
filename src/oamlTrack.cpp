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


oamlTrack::oamlTrack() {
	name = "Track";
	lock = 0;
	volume = 1.f;

	fadeIn = 0;
	fadeOut = 0;
	xfadeIn = 0;
	xfadeOut = 0;

	verbose = false;
}

oamlTrack::~oamlTrack() {
}

void oamlTrack::ClearAudios(std::vector<oamlAudio*> *audios) {
	while (audios->empty() == false) {
		oamlAudio *audio = audios->back();
		audios->pop_back();

		delete audio;
	}
}

oamlAudio* oamlTrack::FindAudio(std::vector<oamlAudio*> *audios, std::string audioName) {
	for (std::vector<oamlAudio*>::iterator it=audios->begin(); it<audios->end(); ++it) {
		oamlAudio *audio = *it;
		if (audio->GetName() == audioName) {
			return audio;
		}
	}

	return NULL;
}

oamlRC oamlTrack::FindAudioAndRemove(std::vector<oamlAudio*> *audios, std::string audioName) {
	for (std::vector<oamlAudio*>::iterator it=audios->begin(); it<audios->end(); ++it) {
		oamlAudio *audio = *it;
		if (audio->GetName() == audioName) {
			audios->erase(it);
			return OAML_OK;
		}
	}

	return OAML_NOT_FOUND;
}

void oamlTrack::FreeAudiosMemory(std::vector<oamlAudio*> *audios) {
	for (std::vector<oamlAudio*>::iterator it=audios->begin(); it<audios->end(); ++it) {
		oamlAudio *audio = *it;
		audio->FreeMemory();
	}
}

void oamlTrack::FillAudiosList(std::vector<oamlAudio*> *audios, std::vector<std::string>& list) {
	for (std::vector<oamlAudio*>::iterator it=audios->begin(); it<audios->end(); ++it) {
		oamlAudio *audio = *it;
		list.push_back(audio->GetName());
	}
}

int oamlTrack::Random(int min, int max) {
	int range = max - min + 1;
	return rand() % range + min;
}

void oamlTrack::ShowPlaying() {
	std::string info;

	info = GetPlayingInfo();
	if (info.length() > 0) {
		__oamlLog("%s\n", info.c_str());
	}
}

void oamlTrack::ApplyVolPanTo(float *samples, int channels, float vol, float pan) {
	if (channels == 2) {
		// Stereo output, apply panning

		if (pan < 0.f) {
			samples[0] = samples[0];
			samples[1] = samples[1] * (1.f + pan);
		} else if (pan > 0.f) {
			samples[0] = samples[0] * (1.f - pan);
			samples[1] = samples[1];
		}
	}

	// Apply volume
	for (int i=0; i<channels; i++) {
		samples[i] = samples[i] * vol;
	}
}

float oamlTrack::SafeAdd(float a, float b, bool debug) {
	float r = a + b;
	bool clipping = false;

	if (r > 1.0f) {
		r = 1.0f - (r - 1.0f);
		clipping = true;
	} else if (r < -1.0f) {
		r = -1.0f - (r + 1.0f);
		clipping = true;
	}

	if (clipping && debug) {
		fprintf(stderr, "oaml: Detected clipping!\n");
		ShowPlaying();
	}

	return r;
}

void oamlTrack::MixAudio(oamlAudio *audio, float *samples, int channels, bool debug) {
	float buf[8];

	audio->ReadSamples(buf, channels);
	for (int i=0; i<channels; i++) {
		samples[i] = SafeAdd(samples[i], buf[i] * volume, debug);
	}
}

unsigned int oamlTrack::MixAudio(oamlAudio *audio, float *samples, int channels, bool debug, unsigned int pos) {
	float buf[8];

	pos = audio->ReadSamples(buf, channels, pos);
	for (int i=0; i<channels; i++) {
		samples[i] = SafeAdd(samples[i], buf[i] * volume, debug);
	}

	return pos;
}

void oamlTrack::ReadInfo(oamlTrackInfo *info) {
	info->name = GetName();
	info->volume = GetVolume();
	info->musicTrack = IsMusicTrack();
	info->sfxTrack = IsSfxTrack();
	info->groups = GetGroups();
	info->subgroups = GetSubgroups();
	info->fadeIn = GetFadeIn();
	info->fadeOut = GetFadeOut();
	info->xfadeIn = GetXFadeIn();
	info->xfadeOut = GetXFadeOut();
}

void oamlTrack::ReadAudiosInfo(std::vector<oamlAudio*> *audios, oamlTrackInfo *info) {
	for (std::vector<oamlAudio*>::iterator it=audios->begin(); it<audios->end(); ++it) {
		oamlAudio *audio = *it;

		oamlAudioInfo ainfo;
		audio->ReadInfo(&ainfo);
		info->audios.push_back(ainfo);
	}
}

int oamlTrack::GetFilesSamplesFor(std::vector<oamlAudio*> *audios) {
	int total = 0;

	for (std::vector<oamlAudio*>::iterator it=audios->begin(); it<audios->end(); ++it) {
		oamlAudio *audio = *it;

		int samples = audio->GetFilesSamples();
		if (samples == -1)
			return -1;

		total+= samples;
	}

	return total;
}

int oamlTrack::LoadProgressFor(std::vector<oamlAudio*> *audios) {
	int ret = 0;

	for (std::vector<oamlAudio*>::iterator it=audios->begin(); it<audios->end(); ++it) {
		oamlAudio *audio = *it;

		int samples = audio->LoadProgress();
		if (samples == -1)
			return -1;

		ret+= samples;
	}

	return ret;
}
