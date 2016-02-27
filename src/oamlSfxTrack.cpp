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


oamlSfxTrack::oamlSfxTrack() {
}

oamlSfxTrack::~oamlSfxTrack() {
	ClearAudios(&sfxAudios);
}

void oamlSfxTrack::AddAudio(oamlAudio *audio) {
	ASSERT(audio != NULL);

	sfxAudios.push_back(audio);
}

int oamlSfxTrack::Play(const char *name) {
	for (size_t i=0; i<sfxAudios.size(); i++) {
		oamlAudio *audio = sfxAudios[i];
		if (audio->GetName().compare(name) == 0) {
			audio->Open();

			sfxPlayInfo info = { audio, 0 };
			playingAudios.push_back(info);
			return 0;
		}
	}

	return -1;
}

void oamlSfxTrack::Mix(float *samples, int channels, bool debugClipping) {
	for (size_t i=0; i<playingAudios.size(); i++) {
		sfxPlayInfo *info = &playingAudios[i];
		info->pos = info->audio->Mix(samples, channels, debugClipping, info->pos);
	}

	for (size_t i=0; i<playingAudios.size(); i++) {
		sfxPlayInfo *info = &playingAudios[i];
		if (info->audio->HasFinishedTail(info->pos)) {
			playingAudios.erase(playingAudios.begin()+i);
		}
	}
}

bool oamlSfxTrack::IsPlaying() {
	return false;
}

void oamlSfxTrack::ShowInfo() {
}

std::string oamlSfxTrack::GetPlayingInfo() {
	std::string info = "";

	return info;
}

void oamlSfxTrack::Stop() {
}
