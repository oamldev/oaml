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

wavHandle *wavOpen(const char *filename) {
	ASSERT(filename != NULL);

	FILE *fd = fopen(filename, "rb");
	if (fd == NULL) {
		return NULL;
	}

	wavHandle *handle = new wavHandle;
	memset(handle, 0, sizeof(wavHandle));
	handle->fd = fd;

	while (handle->status < 2) {
		wavReadChunk(handle);
	}

	return handle;
}

int wavReadChunk(wavHandle *handle) {
	ASSERT(handle != NULL);

	if (handle->fd == NULL)
		return -1;

	// Read the common header for all wave chunks
	wavHeader header;
	if (fread(&header, 1, sizeof(wavHeader), handle->fd) != sizeof(wavHeader)) {
		fclose(handle->fd);
		handle->fd = NULL;
		return -1;
	}

	switch (header.id) {
		case RIFF_ID:
			int waveId;
			if (fread(&waveId, 1, sizeof(int), handle->fd) != sizeof(int))
				return -1;

			// Check waveId signature for valid file
			if (waveId != WAVE_ID)
				return -1;
			break;

		case FMT_ID:
			fmtHeader fmt;
			if (fread(&fmt, 1, sizeof(fmtHeader), handle->fd) != sizeof(fmtHeader))
				return -1;

			if (header.size > sizeof(fmtHeader)) {
				fseek(handle->fd, header.size - sizeof(fmtHeader), SEEK_CUR);
			}
			handle->format = fmt.formatTag;
			handle->channels = fmt.channels;
			handle->samplesPerSec = fmt.samplesPerSec;
			handle->bitsPerSample = fmt.bitsPerSample;
			handle->status = 1;
			break;

		case DATA_ID:
			handle->chunkSize = header.size;
			handle->status = 2;
			break;

		default:
			fseek(handle->fd, header.size, SEEK_CUR);
			break;
	}

	return 0;
}

int wavRead(wavHandle *handle, ByteBuffer *buffer, int size) {
	unsigned char buf[4096];

	ASSERT(handle != NULL);

	if (handle->fd == NULL)
		return -1;

	int bytesRead = 0;
	while (size > 0) {
		// Are we inside a data chunk?
		if (handle->status == 2) {
			// Let's keep reading data!
			int bytes = size < 4096 ? size : 4096;
			if (handle->chunkSize < bytes)
				bytes = handle->chunkSize;
			int ret = fread(buf, 1, bytes, handle->fd);
			if (ret == 0) {
				handle->status = 3;
				break;
			} else {
				handle->chunkSize-= ret;
				buffer->putBytes(buf, ret);
				bytesRead+= ret;
				size-= ret;
			}
		} else {
			if (wavReadChunk(handle) == -1)
				return -1;
		}
	}

	return bytesRead;
}

void wavWriteToFile(const char *filename, ByteBuffer *buffer, int channels, unsigned int sampleRate, int bytesPerSample) {
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

void wavClose(wavHandle *handle) {
	ASSERT(handle != NULL);

	if (handle->fd != NULL) {
		fclose(handle->fd);
		handle->fd = NULL;
	}
	delete handle;
}
