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
#ifndef _MSC_VER
#include <libgen.h>
#endif
#include <math.h>

#include "oamlCommon.h"


oamlAudio::oamlAudio(oamlBase *_base, oamlFileCallbacks *cbs, bool _verbose) {
	name = "";
	base = _base;
	verbose = _verbose;
	fcbs = cbs;

	type = 0;
	bars = 0;
	volume = 1.f;

	channelCount = 0;
	samplesCount = 0;
	samplesPerSec = 0;
	samplesToEnd = 0;
	totalSamples = 0;
	filesSamples = 0;

	bpm = 0;
	beatsPerBar = 0;
	minMovementBars = 0;
	randomChance = 0;
	playOrder = 0;

	fadeIn = 0;
	fadeInSamples = 0;

	fadeOut = 0;
	fadeOutSamples = 0;
	fadeOutCount = 0;

	xfadeIn = 0;
	xfadeOut = 0;

	condId = 0;
	condType = 0;
	condValue = 0;
	condValue2 = 0;

	pickable = true;
}

oamlAudio::~oamlAudio() {
}

void oamlAudio::SetCondition(int id, int type, int value, int value2) {
	condId = id;
	condType = type;
	condValue = value;
	condValue2 = value2;
}

bool oamlAudio::TestCondition(int id, int value) {
	if (id != condId)
		return false;

	switch (condType) {
		case OAML_CONDTYPE_EQUAL:
			if (value == condValue)
				return true;
			break;

		case OAML_CONDTYPE_GREATER:
			if (value > condValue)
				return true;
			break;

		case OAML_CONDTYPE_LESS:
			if (value < condValue)
				return true;
			break;

		case OAML_CONDTYPE_RANGE:
			if (value >= condValue && value <= condValue2)
				return true;
			break;
	}

	return false;
}

unsigned int oamlAudio::GetBarsSamples(int bars) {
	if (bpm == 0)
		return 0;

	float secs = bars * (60.f / bpm) * beatsPerBar;
	return (unsigned int)(secs * samplesPerSec);
}

void oamlAudio::SetBPM(float _bpm) {
	bpm = _bpm;

	UpdateSamplesToEnd();
}

void oamlAudio::SetBeatsPerBar(int _beatsPerBar) {
	beatsPerBar = _beatsPerBar;

	UpdateSamplesToEnd();
}

void oamlAudio::SetBars(int _bars) {
	bars = _bars;

	UpdateSamplesToEnd();
}

void oamlAudio::UpdateSamplesToEnd() {
	samplesToEnd = GetBarsSamples(bars);
	if (samplesToEnd == 0) {
		samplesToEnd = totalSamples;
	}

	for (std::vector<oamlAudioFile>::iterator file=files.begin(); file<files.end(); ++file) {
		file->SetSamplesToEnd(samplesToEnd);
	}
}

void oamlAudio::SetLayerGain(std::string layer, float gain) {
	for (std::vector<oamlAudioFile>::iterator file=files.begin(); file<files.end(); ++file) {
		if (file->GetLayer().compare(layer) == 0) {
			file->SetGain(gain);
		}
	}
}

oamlRC oamlAudio::Load() {
	oamlRC ret = Open();
	if (ret != OAML_OK) return ret;

	for (std::vector<oamlAudioFile>::iterator file=files.begin(); file<files.end(); ++file) {
		ret = file->Load();
		if (ret != OAML_OK)
			return ret;
	}

	return OAML_OK;
}

int oamlAudio::LoadProgress() {
	int ret = 0;
	for (std::vector<oamlAudioFile>::iterator file=files.begin(); file<files.end(); ++file) {
		int count = file->LoadProgress();
		if (count == -1)
			return -1;

		ret+= count;
	}

	return ret;
}

void oamlAudio::GetAudioFileList(std::vector<std::string>& list) {
	for (std::vector<oamlAudioFile>::iterator file=files.begin(); file<files.end(); ++file) {
		list.push_back(file->GetFilename());
	}
}

bool oamlAudio::HasAudioFile(std::string filename) {
	for (std::vector<oamlAudioFile>::iterator file=files.begin(); file<files.end(); ++file) {
		if (file->GetFilename().compare(filename) == 0) {
			return true;
		}
	}

	return false;
}

void oamlAudio::RemoveAudioFile(std::string filename) {
	for (std::vector<oamlAudioFile>::iterator file=files.begin(); file<files.end(); ++file) {
		if (file->GetFilename().compare(filename) == 0) {
			files.erase(file);
			return;
		}
	}
}

oamlAudioFile* oamlAudio::GetAudioFile(std::string filename) {
	for (std::vector<oamlAudioFile>::iterator file=files.begin(); file<files.end(); ++file) {
		if (file->GetFilename().compare(filename) == 0) {
			return &(*file);
		}
	}

	return NULL;
}

oamlRC oamlAudio::Open() {
	if (verbose) __oamlLog("%s %s\n", __FUNCTION__, GetName().c_str());

	for (std::vector<oamlAudioFile>::iterator file=files.begin(); file<files.end(); ++file) {
		oamlRC ret = file->Open();
		if (ret != OAML_OK)
			return ret;

		if (totalSamples == 0) {
			channelCount = file->GetChannels();
			samplesPerSec = file->GetSamplesPerSec() * file->GetChannels();
			totalSamples = file->GetTotalSamples();
		}
	}

	UpdateSamplesToEnd();

	samplesCount = 0;
	fadeInSamples = 0;
	fadeOutSamples = 0;
	fadeOutCount = 0;

	return OAML_OK;
}

void oamlAudio::DoFadeIn(int msec) {
	fadeInSamples = (unsigned int)((msec / 1000.f) * samplesPerSec);
}

void oamlAudio::DoFadeOut(int msec) {
	fadeOutSamples = (unsigned int)((msec / 1000.f) * samplesPerSec);
	fadeOutCount = fadeOutSamples;
}

bool oamlAudio::HasFinished() {
	// Check if we're fading out
	if (fadeOutSamples) {
		if (fadeOutCount <= 0) {
			// Fade out finished so we're done
			return true;
		}
	}

	// Check if our samples reached our end (based off the number of bars)
	return samplesCount >= samplesToEnd;
}

bool oamlAudio::HasFinishedTail(unsigned int pos) {
	return pos >= totalSamples;
}

float oamlAudio::ReadFloat() {
	float sample = 0.f;

	for (std::vector<oamlAudioFile>::iterator file=files.begin(); file<files.end(); ++file) {
		sample+= file->ReadFloat(samplesCount);
	}

	if (fadeInSamples) {
		if (samplesCount < fadeInSamples) {
			float gain = 1.f - float(fadeInSamples - samplesCount) / float(fadeInSamples);
			sample*= gain;
		} else {
			fadeInSamples = 0;
		}
	}

	if (fadeOutSamples) {
		if (fadeOutCount > 0) {
			sample*= float(fadeOutCount) / float(fadeOutSamples);
			fadeOutCount--;
		} else {
			sample = 0.f;
		}
	}

	samplesCount++;

	return sample * volume;
}

float oamlAudio::ReadFloat(unsigned int pos) {
	float sample = 0.f;

	if (pos > totalSamples)
		return 0;

	for (std::vector<oamlAudioFile>::iterator file=files.begin(); file<files.end(); ++file) {
		sample+= file->ReadFloat(pos, true);
	}

	return sample * volume;
}

void oamlAudio::AddAudioFile(std::string filename, std::string layer, int randomChance) {
	oamlAudioFile file = oamlAudioFile(filename, base, fcbs, verbose);
	file.SetLayer(layer);
	file.SetRandomChance(randomChance);

	files.push_back(file);

	if (name == "") {
		size_t pos = filename.find_last_of(PATH_SEPARATOR);
		if (pos != std::string::npos) {
			name = filename.substr(pos+1);
			size_t pos = name.find_last_of('.');
			if (pos != std::string::npos) {
				name = name.substr(0, pos);
			}
		}
	}
}

void oamlAudio::ReadSamples(float *samples, int channels) {
	if (channelCount == 1) {
		// Mono audio to mono/stereo output
		float sample = ReadFloat();

		for (int i=0; i<channels; i++) {
			samples[i] = sample;
		}
	} else if (channelCount == 2) {
		if (channels == 1) {
			// Stereo audio to mono output
			float left = ReadFloat();
			float right = ReadFloat();

			samples[0] = left * 0.5f + right + 0.5f;
		} else if (channels == 2) {
			// Stereo audio to stereo output
			for (int i=0; i<channels; i++) {
				samples[i] = ReadFloat();
			}
		}
	}
}

unsigned int oamlAudio::ReadSamples(float *samples, int channels, unsigned int pos) {
	if (channelCount == 1) {
		float sample = ReadFloat(pos++);

		for (int i=0; i<channels; i++) {
			samples[i] = sample;
		}
	} else if (channelCount == 2) {
		if (channels == 1) {
			float left = ReadFloat(pos++);
			float right = ReadFloat(pos++);

			samples[0] = left * 0.5f + right + 0.5f;
		} else if (channels == 2) {
			for (int i=0; i<channels; i++) {
				samples[i] = ReadFloat(pos++);
			}
		}
	}

	return pos;
}

void oamlAudio::FreeMemory() {
	for (std::vector<oamlAudioFile>::iterator layer=files.begin(); layer<files.end(); ++layer) {
		layer->FreeMemory();
	}

	samplesCount = 0;
	samplesPerSec = 0;
	samplesToEnd = 0;
	totalSamples = 0;
	filesSamples = 0;
}

unsigned int oamlAudio::GetFilesSamples() {
	if (filesSamples == 0) {
		oamlRC ret = Open();
		if (ret != OAML_OK) return ret;

		for (std::vector<oamlAudioFile>::iterator file=files.begin(); file<files.end(); ++file) {
			filesSamples+= file->GetTotalSamples();
		}
	}

	return filesSamples;
}

void oamlAudio::SaveState(tinyxml2::XMLElement *node) {
	node->SetAttribute("name", GetName().c_str());
	node->SetAttribute("samplesCount", samplesCount);
	node->SetAttribute("fadeInSamples", fadeInSamples);
	node->SetAttribute("fadeOutSamples", fadeOutSamples);
	node->SetAttribute("fadeOutCount", fadeOutCount);
	node->SetAttribute("isOpen", samplesToEnd != 0);
}

void oamlAudio::LoadState(tinyxml2::XMLElement *node) {
	if (node->IntAttribute("isOpen")) {
		Open();
	}

	samplesCount = node->IntAttribute("samplesCount");
	fadeInSamples = node->IntAttribute("fadeInSamples");
	fadeOutSamples = node->IntAttribute("fadeOutSamples");
	fadeOutCount = node->IntAttribute("fadeOutCount");
}

void oamlAudio::ReadInfo(oamlAudioInfo *info) {
	info->name = GetName();
	info->type = GetType();
	info->volume = GetVolume();
	info->bars = GetBars();
	info->bpm = GetBPM();
	info->beatsPerBar = GetBeatsPerBar();
	info->minMovementBars = GetMinMovementBars();
	info->randomChance = GetRandomChance();
	info->playOrder = GetPlayOrder();
	info->fadeIn = GetFadeIn();
	info->fadeOut = GetFadeOut();
	info->xfadeIn = GetXFadeIn();
	info->xfadeOut = GetXFadeOut();
	info->condId = GetCondId();
	info->condType = GetCondType();
	info->condValue = GetCondValue();
	info->condValue2 = GetCondValue2();

	for (std::vector<oamlAudioFile>::iterator file=files.begin(); file<files.end(); ++file) {
		oamlAudioFileInfo afinfo;

		afinfo.filename = file->GetFilename();
		afinfo.layer = file->GetLayer();
		afinfo.randomChance = file->GetRandomChance();

		info->files.push_back(afinfo);
	}
}
