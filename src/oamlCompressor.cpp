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

	chnum = 0;
}

oamlCompressor::~oamlCompressor() {
}

void oamlCompressor::SetThreshold(double db) {
	threshold = pow(10.0, db / 20.0);
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
		gain = float(threshold / (1.0 + ratio * ((env / threshold) - 1.0)));
	} else {
		gain = float(threshold);
	}

	gain = gain * 0.5f + 0.5f;
	for (int i=0; i<chnum; i++) {
		data[i]*= gain;
	}
}
