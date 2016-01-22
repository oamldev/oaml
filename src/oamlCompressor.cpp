#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "oamlCommon.h"


oamlCompressor::oamlCompressor() {
	SetThreshold(-3);
	SetRatio(4.0);

	attackTime = 10.0;	// 10ms
	releaseTime = 200.0;	// 200ms

	att = 0.0;
	rel = 0.0;
	env = 0.0;
}

oamlCompressor::~oamlCompressor() {
}

void oamlCompressor::SetThreshold(double db) {
	threshold = pow(10.0f, db / 20.0f);
}

void oamlCompressor::SetRatio(double value) {
	ratio = 1.0 - (1.0 / value);
}

void oamlCompressor::SetAudioFormat(int channels, int sampleRate) {
	chnum = channels;

	att = exp(-1 / (attackTime * 0.001 * sampleRate));
	rel = exp(-1 / (releaseTime * 0.001 * sampleRate));
}

void oamlCompressor::ProcessData(float *data) {
	float peak = 0;
	for (int i=0; i<chnum; i++) {
		float val = fabs(data[i]);
		if (val > peak) peak = val;
	}

	if (peak > env) {
		env = att * (env - peak) + peak;
	} else {
		env = rel * (env - peak) + peak;
	}

	float gain;
	if (env > threshold) {
		gain = threshold / (1.0 + ratio * ((env / threshold) - 1.0));
	} else {
		gain = threshold;
	}

	gain = gain * 0.5 + 0.5;
	for (int i=0; i<chnum; i++) {
		data[i]*= gain;
	}
}
