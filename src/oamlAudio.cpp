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
#include <libgen.h>
#include <math.h>

#include "oamlCommon.h"


oamlAudio::oamlAudio(oamlFileCallbacks *cbs) {
	fcbs = cbs;

	buffer = new ByteBuffer();
	handle = NULL;
	filename = "";
	type = 0;
	bars = 0;

	bytesPerSample = 0;
	samplesCount = 0;
	samplesPerSec = 0;
	samplesToEnd = 0;
	totalSamples = 0;

	bpm = 0;
	beatsPerBar = 0;
	minMovementBars = 0;
	randomChance = 0;

	fadeIn = 0;
	fadeInSamples = 0;

	fadeOut = 0;
	fadeOutSamples = 0;
	fadeOutStart = 0;

	xfadeIn = 0;
	xfadeOut = 0;

	condId = 0;
	condType = 0;
	condValue = 0;
	condValue2 = 0;

	pickable = true;
}

oamlAudio::~oamlAudio() {
	delete buffer;
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
		case COND_TYPE_EQUAL:
			if (value == condValue)
				return true;
			break;

		case COND_TYPE_GREATER:
			if (value > condValue)
				return true;
			break;

		case COND_TYPE_LESS:
			if (value < condValue)
				return true;
			break;

		case COND_TYPE_RANGE:
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

int oamlAudio::Open() {
//	printf("%s %s\n", __FUNCTION__, GetFilenameStr());

	if (buffer->size() > 0) {
		buffer->setReadPos(0);
		samplesCount = 0;
	} else {
		std::string ext = filename.substr(filename.find_last_of(".") + 1);
		if (ext == "ogg") {
			handle = (audioFile*)new oggFile(fcbs);
		} else if (ext == "aif" || ext == "aiff") {
			handle = (audioFile*)new aifFile(fcbs);
		} else if (ext == "wav" || ext == "wave") {
			handle = new wavFile(fcbs);
		} else {
			fprintf(stderr, "liboaml: Unknown audio format: '%s'\n", GetFilenameStr());
			return -1;
		}

		if (handle->Open(GetFilenameStr()) == -1) {
			fprintf(stderr, "liboaml: Error opening: '%s'\n", GetFilenameStr());
			return -1;
		}

		samplesPerSec = handle->GetSamplesPerSec() * handle->GetChannels();
		bytesPerSample = handle->GetBytesPerSample();
		totalSamples = handle->GetTotalSamples();
		channelCount = handle->GetChannels();

		samplesToEnd = GetBarsSamples(bars);
		if (samplesToEnd == 0) {
			samplesToEnd = handle->GetTotalSamples();
		}
	}

	fadeInSamples = 0;

	if (fadeOut) {
		fadeOutSamples = (unsigned int)((fadeOut / 1000.f) * samplesPerSec);
		fadeOutStart = samplesToEnd - fadeOutSamples;
	} else {
		fadeOutSamples = 0;
		fadeOutStart = 0;
	}

	return 0;
}

void oamlAudio::DoFadeIn(int msec) {
	fadeInSamples = (unsigned int)((msec / 1000.f) * samplesPerSec);
}

void oamlAudio::DoFadeOut(int msec) {
	fadeOutStart = samplesCount;
	fadeOutSamples = (unsigned int)((msec / 1000.f) * samplesPerSec);
}

bool oamlAudio::HasFinished() {
	// Check if we're fading out
	if (fadeOutSamples) {
		unsigned int fadeOutFinish = fadeOutStart + fadeOutSamples;
		if (samplesCount >= fadeOutFinish) {
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

int oamlAudio::Read() {
	if (handle == NULL)
		return -1;

	int readSize = 4096*bytesPerSample;
	int ret = handle->Read(buffer, readSize);
	if (ret < readSize) {
		handle->Close();
	}

	return ret;
}

int oamlAudio::Read32() {
	int ret = 0;

	if (buffer->bytesRemaining() < (unsigned int)bytesPerSample) {
		if (Read() == -1)
			return 0;
	}

	if (bytesPerSample == 3) {
		ret|= ((unsigned int)buffer->get())<<8;
		ret|= ((unsigned int)buffer->get())<<16;
		ret|= ((unsigned int)buffer->get())<<24;
	} else if (bytesPerSample == 2) {
		ret|= ((unsigned int)buffer->get())<<16;
		ret|= ((unsigned int)buffer->get())<<24;
	} else if (bytesPerSample == 1) {
		ret|= ((unsigned int)buffer->get())<<23;
	}

	if (fadeInSamples) {
		if (samplesCount < fadeInSamples) {
			float gain = 1.f - float(fadeInSamples - samplesCount) / float(fadeInSamples);
//			gain = log10(1.f + 9.f * gain);
			ret = (int)(ret * gain);
		} else {
			fadeInSamples = 0;
		}
	}

	if (fadeOutSamples && samplesCount >= fadeOutStart) {
		unsigned int fadeOutFinish = fadeOutStart + fadeOutSamples;
		if (samplesCount < fadeOutFinish) {
			float gain = float(fadeOutFinish - samplesCount) / float(fadeOutSamples);
//			gain = log10(1.f + 9.f * gain);
			ret = (int)(ret * gain);
		} else {
			ret = 0;
		}
	}

	samplesCount++;

	return ret;
}

int oamlAudio::Read32(unsigned int pos) {
	int ret = 0;

	if (pos > totalSamples)
		return 0;

	pos*= bytesPerSample;
	while ((pos+bytesPerSample) > buffer->size()) {
		if (Read() == -1)
			return 0;
	}

	if (bytesPerSample == 3) {
		ret|= ((unsigned int)buffer->get(pos))<<8;
		ret|= ((unsigned int)buffer->get(pos+1))<<16;
		ret|= ((unsigned int)buffer->get(pos+2))<<24;
	} else if (bytesPerSample == 2) {
		ret|= ((unsigned int)buffer->get(pos))<<16;
		ret|= ((unsigned int)buffer->get(pos+1))<<24;
	} else if (bytesPerSample == 1) {
		ret|= ((unsigned int)buffer->get(pos))<<23;
	}

	return ret;
}

float oamlAudio::ReadFloat() {
	return __oamlInteger24ToFloat(Read32() >> 8);
}

float oamlAudio::ReadFloat(unsigned int pos) {
	return __oamlInteger24ToFloat(Read32(pos) >> 8);
}

void oamlAudio::SetFilename(std::string audioFilename) {
	filename = audioFilename;
	size_t pos = filename.find_last_of(PATH_SEPARATOR);
	if (pos != std::string::npos) {
		name = filename.substr(pos+1);
		size_t pos = name.find_last_of('.');
		if (pos != std::string::npos) {
			name = name.substr(0, pos);
		}
	}
}

float oamlAudio::SafeAdd(float a, float b, bool debug) {
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
	}

	return r;
}

void oamlAudio::Mix(float *samples, int channels, bool debugClipping) {
	if (channelCount == 1) {
		float sample = ReadFloat();

		for (int i=0; i<channels; i++) {
			samples[i] = SafeAdd(samples[i], sample, debugClipping);
		}
	} else if (channelCount == 2) {
		if (channels == 1) {
			float left = ReadFloat();
			float right = ReadFloat();
			float sample = left * 0.5f + right + 0.5f;

			samples[0] = SafeAdd(samples[0], sample, debugClipping);
		} else if (channels == 2) {
			for (int i=0; i<channels; i++) {
				samples[i] = SafeAdd(samples[i], ReadFloat(), debugClipping);
			}
		}
	}
}

unsigned int oamlAudio::Mix(float *samples, int channels, bool debugClipping, unsigned int pos) {
	if (channelCount == 1) {
		float sample = ReadFloat(pos++);

		for (int i=0; i<channels; i++) {
			samples[i] = SafeAdd(samples[i], sample, debugClipping);
		}
	} else if (channelCount == 2) {
		if (channels == 1) {
			float left = ReadFloat(pos++);
			float right = ReadFloat(pos++);
			float sample = left * 0.5f + right + 0.5f;

			samples[0] = SafeAdd(samples[0], sample, debugClipping);
		} else if (channels == 2) {
			for (int i=0; i<channels; i++) {
				samples[i] = SafeAdd(samples[i], ReadFloat(pos++), debugClipping);
			}
		}
	}

	return pos;
}
