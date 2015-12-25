#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "oamlCommon.h"


oamlAudio::oamlAudio() {
	handle = NULL;

	memset(filename, 0, sizeof(filename));
	type = 0;
	bars = 0;
	bpm = 0;
	beatsPerBar = 0;
	fadeIn = 0;
	fadeOut = 0;

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
	condValue2 = 0;
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

int oamlAudio::Open() {
	printf("%s %s\n", __FUNCTION__, filename);

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
	if (handle == NULL)
		return -1;
	return wavRead(handle, buffer, 4096*bytesPerSample);
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
