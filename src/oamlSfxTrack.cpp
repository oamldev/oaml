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


oamlSfxTrack::oamlSfxTrack(bool _verbose) {
	name = "Sfx";
	verbose = _verbose;
}

oamlSfxTrack::~oamlSfxTrack() {
	ClearAudios(&sfxAudios);
}

void oamlSfxTrack::AddAudio(oamlAudio *audio) {
	ASSERT(audio != NULL);

	sfxAudios.push_back(audio);
}

oamlAudio* oamlSfxTrack::GetAudio(std::string filename) {
	return FindAudio(&sfxAudios, filename);
}

oamlRC oamlSfxTrack::Play(const char *name, float vol, float pan) {
	if (lock > 0) {
		// Play function can't be called while Mix is being run
		return OAML_ERROR;
	}

	for (size_t i=0; i<sfxAudios.size(); i++) {
		oamlAudio *audio = sfxAudios[i];
		if (audio->GetName().compare(name) == 0) {
			// We found our match, open and push it to playingAudios
			audio->Open();

			sfxPlayInfo info = { audio, 0, vol, pan };
			playingAudios.push_back(info);
			return OAML_OK;
		}
	}

	return OAML_NOT_FOUND;
}

void oamlSfxTrack::Mix(float *samples, int channels, bool debugClipping) {
	if (playingAudios.size() == 0)
		return;

	// Prevent Play being called while this function is running
	lock++;

	for (std::vector<sfxPlayInfo>::iterator it=playingAudios.begin(); it!=playingAudios.end(); ++it) {
		float buf[8];

		// Read samples from our sfx to buf
		it->pos = it->audio->ReadSamples(buf, channels, it->pos);

		// Apply the desired volume/panning
		ApplyVolPanTo(buf, channels, it->vol, it->pan);

		// Now finally mix the buf samples into the output samples array
		for (int j=0; j<channels; j++) {
			samples[j] = SafeAdd(samples[j], buf[j], debugClipping);
		}
	}

	for (std::vector<sfxPlayInfo>::iterator it=playingAudios.begin(); it!=playingAudios.end();) {
		// Check if the sfx has finished playing and remove it if so
		if (it->audio->HasFinishedTail(it->pos)) {
			it = playingAudios.erase(it);
		} else {
			++it;
		}
	}

	lock--;
}

bool oamlSfxTrack::IsPlaying() {
	return false;
}

std::string oamlSfxTrack::GetPlayingInfo() {
	std::string info = "";

	return info;
}

void oamlSfxTrack::Stop() {
}

void oamlSfxTrack::ReadInfo(oamlTrackInfo *info) {
	oamlTrack::ReadInfo(info);

	ReadAudiosInfo(&sfxAudios, info);
}

void oamlSfxTrack::FreeMemory() {
	FreeAudiosMemory(&sfxAudios);
}

void oamlSfxTrack::GetAudioList(std::vector<std::string>& list) {
	FillAudiosList(&sfxAudios, list);
}

