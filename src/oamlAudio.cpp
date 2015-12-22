#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ByteBuffer.h"
#include "wav.h"
#include "oamlAudio.h"


oamlAudio::oamlAudio(const char *audioFilename, int audioType, int audioBars, float audioBpm, int audioBeatsPerBar, unsigned int audioFadeIn, unsigned int audioFadeOut) {
	buffer = NULL;
	handle = NULL;

	strcpy(filename, audioFilename);
	type = audioType;
	bars = audioBars;
	bpm = audioBpm;
	beatsPerBar = audioBeatsPerBar;
	fadeIn = audioFadeIn;
	fadeOut = audioFadeOut;

	buffer = new ByteBuffer();

	samplesCount = 0;
	samplesPerSec = 0;
	samplesToEnd = 0;
	totalSamples = 0;
	fadeInSamples = 0;
	fadeOutSamples = 0;

	condId = 0;
	condType = 0;
	condValue = 0;
}

oamlAudio::~oamlAudio() {
	delete buffer;
}

void oamlAudio::SetCondition(int id, int type, int value) {
	condId = id;
	condType = type;
	condValue = value;
}

bool oamlAudio::TestCondition(int id, int value) {
	if (id == condId) {
		if (condType == 0) {
			if (value == condValue) {
				return true;
			}
		}
	}

	return false;
}

int oamlAudio::Open() {
	if (handle != NULL) {
		buffer->setReadPos(0);
		samplesCount = 0;
	} else {
		handle = wavOpen(filename);
		if (handle == NULL) {
			return -1;
		}

		samplesPerSec = handle->samplesPerSec * handle->channels;
		bytesPerSample = handle->bitsPerSample / 8;
		totalSamples = handle->chunkSize / bytesPerSample;

		float secs = bars * (60.f / bpm) * beatsPerBar;
		samplesToEnd = secs * samplesPerSec;
	}

	if (fadeIn) {
		fadeInSamples = (fadeIn / 1000.f) * samplesPerSec;
	} else {
		fadeInSamples = 0;
	}

	if (fadeOut) {
		fadeOutSamples = (fadeOut / 1000.f) * samplesPerSec;
		fadeOutStart = samplesToEnd - fadeOutSamples;
	} else {
		fadeOutSamples = 0;
		fadeOutStart = 0;
	}

	return 0;
}

void oamlAudio::DoFadeIn(int msec) {
	fadeInSamples = (msec / 1000.f) * samplesPerSec;
}

void oamlAudio::DoFadeOut(int msec) {
	fadeOutStart = samplesCount;
	fadeOutSamples = (msec / 1000.f) * samplesPerSec;
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

int oamlAudio::Read() {
	return wavRead(handle, buffer, 4096*bytesPerSample);
}

int oamlAudio::Read32() {
	int ret = 0;

	if (buffer->bytesRemaining() < (unsigned int)bytesPerSample) {
		Read();
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
			gain = log10(1.f + 9.f * gain);
			ret = (int)(ret * gain);
		} else {
			fadeInSamples = 0;
		}
	}

	if (fadeOutSamples && samplesCount >= fadeOutStart) {
		unsigned int fadeOutFinish = fadeOutStart + fadeOutSamples;
		if (samplesCount < fadeOutFinish) {
			float gain = float(fadeOutFinish - samplesCount) / float(fadeOutSamples);
			gain = log10(1.f + 9.f * gain);
			ret = (int)(ret * gain);
		} else {
			ret = 0;
		}
	}

	samplesCount++;

	return ret;
}
