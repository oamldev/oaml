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


oamlAudioFile::oamlAudioFile(std::string _filename, oamlBase *_base, oamlFileCallbacks *cbs, bool _verbose) {
	base = _base;
	filename = _filename;
	layer = "";
	randomChance = -1;
	gain = 1.f;
	fcbs = cbs;
	verbose = _verbose;

#ifdef __HAVE_SOXR
	soxr = NULL;
#endif
	handle = NULL;

	format = 0;
	bytesPerSample = 0;
	samplesPerSec = 0;
	totalSamples = 0;
	channelCount = 0;
	samplesToEnd = 0;
	fileBytesPerSample = 0;

	chance = false;
	lastChance = false;
}

oamlAudioFile::~oamlAudioFile() {
	if (handle) {
		delete handle;
		handle = NULL;
	}

#ifdef __HAVE_SOXR
	if (soxr) {
		soxr_delete(soxr);
		soxr = NULL;
	}
#endif
}

oamlRC oamlAudioFile::OpenFile() {
	std::string ext = filename.substr(filename.find_last_of(".") + 1);
	if (ext == "wav" || ext == "wave") {
		handle = new wavFile(fcbs);
	} else if (ext == "aif" || ext == "aiff") {
		handle = (audioFile*)new aifFile(fcbs);
#ifdef __HAVE_OGG
	} else if (ext == "ogg") {
		handle = (audioFile*)new oggFile(fcbs);
#endif
	} else {
		fprintf(stderr, "liboaml: Unknown audio format: '%s'\n", GetFilenameStr());
		return OAML_ERROR;
	}

	if (handle->Open(GetFilenameStr()) == -1) {
		fprintf(stderr, "liboaml: Error opening: '%s'\n", GetFilenameStr());
		return OAML_ERROR;
	}

	format = handle->GetFormat();
	bytesPerSample = handle->GetBytesPerSample();
	samplesPerSec = handle->GetSamplesPerSec();
	totalSamples = handle->GetTotalSamples();
	channelCount = handle->GetChannels();

	fileFormat = format;
	fileBytesPerSample = bytesPerSample;

#ifdef __HAVE_SOXR
	unsigned int sampleRate = base->GetSampleRate();
	if (sampleRate != samplesPerSec) {
		if (soxr == NULL) {
			soxr_io_spec_t spec;

			switch (format) {
				case AF_FORMAT_SINT8:
				case AF_FORMAT_SINT16:
				case AF_FORMAT_SINT24:
					spec.itype = SOXR_INT16_I;
					break;

				case AF_FORMAT_SINT32:
					spec.itype = SOXR_INT32_I;
					break;

				case AF_FORMAT_FLOAT32:
					spec.itype = SOXR_FLOAT32_I;
					break;
			}
			spec.otype = SOXR_INT16_I;
			spec.scale = 1;
			spec.e = NULL;
			spec.flags = 0;

			soxr = soxr_create(double(samplesPerSec), double(sampleRate), channelCount, NULL, &spec, NULL, NULL);
		}

		totalSamples = (double)totalSamples * sampleRate / samplesPerSec;
		samplesToEnd = (double)samplesToEnd * sampleRate / samplesPerSec;
		samplesPerSec = sampleRate;
		format = AF_FORMAT_SINT16;
		bytesPerSample = 2;
	}
#endif

	return OAML_OK;
}

oamlRC oamlAudioFile::Open() {
	if (verbose) __oamlLog("%s %s\n", __FUNCTION__, GetFilenameStr());
	if (buffer.size() == 0) {
		oamlRC rc = OpenFile();
		if (rc != OAML_OK) return rc;
	}

	if (GetRandomChance() != -1) {
		chance = __oamlRandom(0, 100) > GetRandomChance();
	} else {
		lastChance = true;
		chance = true;
	}

	return OAML_OK;
}

oamlRC oamlAudioFile::Load() {
	int ret;
	do {
		ret = Read();
	} while (ret > 0);

	if (ret == -1) return OAML_ERROR;
	return OAML_OK;
}

int oamlAudioFile::LoadProgress() {
	if (handle == NULL) {
		return buffer.size()/bytesPerSample;
	}

	int ret = Read();
	if (ret == -1) {
		return -1;
	}

	return buffer.size()/bytesPerSample;
}

int oamlAudioFile::Read() {
	if (handle == NULL)
		return -1;

	char buf[4096];
	int frames = 4096 / fileBytesPerSample / channelCount;
	int readSize = frames * fileBytesPerSample * channelCount;
	int bytes = handle->Read(buf, readSize);
	if (bytes <= 0) {
		handle->Close();
		delete handle;
		handle = NULL;
	}

#ifdef __HAVE_SOXR
	if (soxr != NULL) {
		// Re-sample through libsoxr
		char obuf[4096*4];

		size_t ilen = bytes / fileBytesPerSample / channelCount;
		size_t idone = 0;
		size_t olen = 4096*4 / bytesPerSample / channelCount;
		size_t odone = 0;
		soxr_error_t err;

		switch (fileFormat) {
			case AF_FORMAT_SINT8:
				// 8-bit is not directly supported by libsoxr, convert the buffer to 16-bit and then process
				{
					int16_t buf16[4096];
					for (int i=0; i<bytes; i++) {
						buf16[i] = (int16_t)buf[i] << 8;
					}

					err = soxr_process(soxr, (char*)buf16, ilen, &idone, obuf, olen, &odone);
				}
				break;

			case AF_FORMAT_SINT24:
				// 24-bit is not directly supported by libsoxr, convert the buffer to 16-bit and then process
				{
					int16_t buf16[4096];
					for (int i=0; i<bytes/3; i++) {
						buf16[i] = (((int16_t)buf[i*3+2] & 0xFF) << 8) | ((int16_t)buf[i*3+1] & 0xFF);
					}

					err = soxr_process(soxr, (char*)buf16, ilen, &idone, obuf, olen, &odone);
				}
				break;

			default:
				// Input format is supported by libsoxr, process out buffers
				err = soxr_process(soxr, buf, ilen, &idone, obuf, olen, &odone);
				break;
		}

		if (err != NULL) {
			fprintf(stderr, "liboaml: Error on soxr_process\n");
			return -1;
		} else if (odone > 0) {
			buffer.putBytes((uint8_t*)obuf, odone * bytesPerSample * channelCount);
		}

		return odone;
	}
#endif

	if (bytes > 0) {
		buffer.putBytes((uint8_t*)buf, bytes);
	}

	return bytes;
}

int oamlAudioFile::Read32(unsigned int pos) {
	int ret = 0;

	if (pos > totalSamples)
		return 0;

	pos*= bytesPerSample;
	while ((pos+bytesPerSample) > buffer.size()) {
		if (Read() == -1)
			return 0;
	}

	switch (format) {
		case AF_FORMAT_FLOAT32:
			ret|= ((uint32_t)buffer.get(pos));
			ret|= ((uint32_t)buffer.get(pos+1))<<8;
			ret|= ((uint32_t)buffer.get(pos+2))<<16;
			ret|= ((uint32_t)buffer.get(pos+3))<<24;
			ret = uint32_t(*(float*)&ret * 32768.f) << 16;
			break;

		case AF_FORMAT_SINT32:
			break;

		case AF_FORMAT_SINT24:
			ret|= ((uint32_t)buffer.get(pos))<<8;
			ret|= ((uint32_t)buffer.get(pos+1))<<16;
			ret|= ((uint32_t)buffer.get(pos+2))<<24;
			break;

		case AF_FORMAT_SINT16:
			ret|= ((uint32_t)buffer.get(pos))<<16;
			ret|= ((uint32_t)buffer.get(pos+1))<<24;
			break;

		case AF_FORMAT_SINT8:
			ret|= ((uint32_t)buffer.get(pos))<<23;
			break;
	}

	return ret;
}

float oamlAudioFile::ReadFloat(unsigned int pos, bool isTail) {
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
	if (GetGain() != 1.f) {
		sample*= GetGain();
	}
	return sample;
}

void oamlAudioFile::FreeMemory() {
	if (buffer.size() > 0 || handle != NULL) {
		if (verbose) __oamlLog("%s %s\n", __FUNCTION__, GetFilenameStr());
	}

	buffer.clear();
	buffer.free();

	if (handle) {
		delete handle;
		handle = NULL;
	}

	format = 0;
	bytesPerSample = 0;
	samplesPerSec = 0;
	totalSamples = 0;
	channelCount = 0;
}
