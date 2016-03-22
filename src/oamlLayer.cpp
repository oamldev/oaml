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


oamlLayer::oamlLayer(std::string _filename, std::string _layer, oamlLayerData *_info, oamlFileCallbacks *cbs, bool _verbose) {
	filename = _filename;
	layer = _layer;
	info = _info;
	fcbs = cbs;
	verbose = _verbose;

	handle = NULL;

	bytesPerSample = 0;
	samplesPerSec = 0;
	totalSamples = 0;
	channelCount = 0;

	chance = false;
	lastChance = false;
}

oamlLayer::~oamlLayer() {
	if (handle) {
		delete handle;
		handle = NULL;
	}
}

oamlRC oamlLayer::OpenFile() {
	std::string ext = filename.substr(filename.find_last_of(".") + 1);
	if (ext == "ogg") {
		handle = (audioFile*)new oggFile(fcbs);
	} else if (ext == "aif" || ext == "aiff") {
		handle = (audioFile*)new aifFile(fcbs);
	} else if (ext == "wav" || ext == "wave") {
		handle = new wavFile(fcbs);
	} else {
		fprintf(stderr, "liboaml: Unknown audio format: '%s'\n", GetFilenameStr());
		return OAML_ERROR;
	}

	if (handle->Open(GetFilenameStr()) == -1) {
		fprintf(stderr, "liboaml: Error opening: '%s'\n", GetFilenameStr());
		return OAML_ERROR;
	}

	bytesPerSample = handle->GetBytesPerSample();
	samplesPerSec = handle->GetSamplesPerSec() * handle->GetChannels();
	totalSamples = handle->GetTotalSamples();
	channelCount = handle->GetChannels();

	return OAML_OK;
}

oamlRC oamlLayer::Open() {
	if (verbose) __oamlLog("%s %s\n", __FUNCTION__, GetFilenameStr());
	if (buffer.size() == 0) {
		oamlRC rc = OpenFile();
		if (rc != OAML_OK) return rc;
	}

	if (info && info->randomChance != 100) {
		chance = __oamlRandom(0, 100) > info->randomChance;
	} else {
		lastChance = true;
		chance = true;
	}

	return OAML_OK;
}

oamlRC oamlLayer::Load() {
	int ret;
	do {
		ret = Read();
	} while (ret > 0);

	if (ret == -1) return OAML_ERROR;
	return OAML_OK;
}

int oamlLayer::Read() {
	if (handle == NULL)
		return -1;

	int readSize = 4096*bytesPerSample;
	int ret = handle->Read(&buffer, readSize);
	if (ret < readSize) {
		handle->Close();
	}

	return ret;
}

int oamlLayer::Read32(unsigned int pos) {
	int ret = 0;

	if (pos > totalSamples)
		return 0;

	pos*= bytesPerSample;
	while ((pos+bytesPerSample) > buffer.size()) {
		if (Read() == -1)
			return 0;
	}

	if (bytesPerSample == 3) {
		ret|= ((unsigned int)buffer.get(pos))<<8;
		ret|= ((unsigned int)buffer.get(pos+1))<<16;
		ret|= ((unsigned int)buffer.get(pos+2))<<24;
	} else if (bytesPerSample == 2) {
		ret|= ((unsigned int)buffer.get(pos))<<16;
		ret|= ((unsigned int)buffer.get(pos+1))<<24;
	} else if (bytesPerSample == 1) {
		ret|= ((unsigned int)buffer.get(pos))<<23;
	}

	return ret;
}

float oamlLayer::ReadFloat(unsigned int pos, bool isTail) {
	if (isTail) {
		if (lastChance == false)
			return 0.f;
	} else {
		if (pos == (samplesToEnd-1)) {
			lastChance = chance;
		}

		if (chance == false)
			return 0.f;
	}

	float sample = __oamlInteger24ToFloat(Read32(pos)>>8);
	if (info) {
		sample*= info->gain;
	}
	return sample;
}

void oamlLayer::FreeMemory() {
	if (buffer.size() > 0 || handle != NULL) {
		if (verbose) __oamlLog("%s %s\n", __FUNCTION__, GetFilenameStr());
	}

	buffer.clear();
	buffer.free();

	if (handle) {
		delete handle;
		handle = NULL;
	}
}
