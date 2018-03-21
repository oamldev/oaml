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

enum {
	WAVE_ID = 0x45564157,
	RIFF_ID = 0x46464952,
	FMT_ID  = 0x20746d66,
	DATA_ID = 0x61746164,
	CUE_ID  = 0x20657563,
	LIST_ID = 0x5453494c,
	BEXT_ID = 0x74786562
};

typedef struct {
	int id;
	unsigned int size;
} wavHeader;

typedef struct {
	int id;
	unsigned int size;
	int wave;
} riffHeader;

typedef struct {
	unsigned short formatTag;
	unsigned short channels;
	unsigned int samplesPerSec;
	unsigned int avgBytesPerSec;
	unsigned short blockAlign;
	unsigned short bitsPerSample;
} fmtHeader;


wavFile::wavFile(oamlFileCallbacks *cbs) : audioFile(cbs) {
	fcbs = cbs;
	fd = NULL;

	format = 0;
	channels = 0;
	samplesPerSec = 0;
	bitsPerSample = 0;
	totalSamples = 0;

	chunkSize = 0;
	status = 0;
}

wavFile::~wavFile() {
	if (fd != NULL) {
		Close();
	}
}

int wavFile::Open(const char *filename) {
	ASSERT(filename != NULL);

	if (fd != NULL) {
		Close();
	}

	fd = fcbs->open(filename);
	if (fd == NULL) {
		return -1;
	}

	while (status < 2) {
		if (ReadChunk() == -1) {
			return -1;
		}
	}

	return 0;
}

int wavFile::ReadChunk() {
	if (fd == NULL)
		return -1;

	// Read the common header for all wave chunks
	wavHeader header;
	if (fcbs->read(&header, 1, sizeof(wavHeader), fd) != sizeof(wavHeader)) {
		fcbs->close(fd);
		fd = NULL;
		return -1;
	}

	switch (header.id) {
		case RIFF_ID:
			int waveId;
			if (fcbs->read(&waveId, 1, sizeof(int), fd) != sizeof(int))
				return -1;

			// Check waveId signature for valid file
			if (waveId != WAVE_ID)
				return -1;
			break;

		case FMT_ID:
			fmtHeader fmt;
			if (fcbs->read(&fmt, 1, sizeof(fmtHeader), fd) != sizeof(fmtHeader))
				return -1;

			if (header.size > sizeof(fmtHeader)) {
				fcbs->seek(fd, header.size - sizeof(fmtHeader), SEEK_CUR);
			}

			channels = fmt.channels;
			samplesPerSec = fmt.samplesPerSec;
			bitsPerSample = fmt.bitsPerSample;
			status = 1;

			switch (fmt.formatTag) {
				case 1:
					switch (bitsPerSample) {
						case 8: format = AF_FORMAT_SINT8; break;
						case 16: format = AF_FORMAT_SINT16; break;
						case 24: format = AF_FORMAT_SINT24; break;
						case 32: format = AF_FORMAT_SINT32; break;
					}
					break;

				case 3:
					format = AF_FORMAT_FLOAT32;
					break;
			}
			break;

		case DATA_ID:
			chunkSize = header.size;
			totalSamples = chunkSize / (bitsPerSample/8);
			status = 2;
			break;

		default:
			fcbs->seek(fd, header.size, SEEK_CUR);
			break;
	}

	return 0;
}

int wavFile::Read(char *buffer, int size) {
	if (fd == NULL)
		return -1;

	int bytesRead = 0;
	while (size > 0) {
		// Are we inside a data chunk?
		if (status == 2) {
			// Let's keep reading data!
			int ret = fcbs->read(buffer, 1, size, fd);
			if (ret == 0) {
				status = 3;
				break;
			} else {
				chunkSize-= ret;
				buffer+= ret;
				bytesRead+= ret;
				size-= ret;
			}
		} else {
			if (ReadChunk() == -1)
				return -1;
		}
	}

	return bytesRead;
}

void wavFile::WriteToFile(const char *filename, ByteBuffer *buffer, int channels, unsigned int sampleRate, int bytesPerSample) {
	ASSERT(filename != NULL);
	ASSERT(buffer != NULL);

	FILE *f = fopen(filename, "wb");
	if (f == NULL)
		return;

	riffHeader riff = {
		RIFF_ID,
		36 + buffer->bytesRemaining(),
		WAVE_ID,
	};

	wavHeader fmtHead = {
		FMT_ID,
		16,
	};

	fmtHeader fmt = {
		1,
		(unsigned short)(channels),
		sampleRate,
		sampleRate * channels * bytesPerSample,
		(unsigned short)(channels * bytesPerSample),
		(unsigned short)(8 * bytesPerSample),
	};

	wavHeader data = {
		DATA_ID,
		buffer->bytesRemaining(),
	};

	fwrite(&riff, 1, sizeof(riff), f);
	fwrite(&fmtHead, 1, sizeof(fmtHead), f);
	fwrite(&fmt, 1, sizeof(fmt), f);
	fwrite(&data, 1, sizeof(data), f);
	while (buffer->bytesRemaining() > 0) {
		unsigned char byte = buffer->get();
		fwrite(&byte, 1, sizeof(byte), f);
	}

	fclose(f);
}

void wavFile::Close() {
	if (fd != NULL) {
		fcbs->close(fd);
		fd = NULL;
	}
}
