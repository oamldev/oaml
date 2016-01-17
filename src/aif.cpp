#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "oamlCommon.h"


#define	SWAP16(x) (((x) & 0xff) << 8 | ((x) & 0xff00) >> 8)
#define	SWAP32(x) (((x) & 0xff) << 24 | ((x) & 0xff00) << 8 | ((x) & 0xff0000) >> 8 | ((x) >> 24) & 0xff)

enum {
	AIFF_ID = 0x46464941,
	FORM_ID = 0x4D524F46,
	COMM_ID = 0x4D4D4F43,
	SSND_ID = 0x444E5353
};

typedef struct {
	int id;
	unsigned int size;
} aifHeader;

typedef struct {
	int id;
	unsigned int size;
	int aiff;
} formHeader;

typedef struct {
	unsigned int offset;
	unsigned int blockSize;
} ssndHeader;

typedef struct {
	unsigned short channels;
	unsigned int sampleFrames;
	unsigned short sampleSize;
	unsigned char sampleRate80[10];
}  __attribute__((packed)) commHeader;


aifFile::aifFile() {
	fd = NULL;

	channels = 0;
	samplesPerSec = 0;
	bitsPerSample = 0;
	totalSamples = 0;

	chunkSize = 0;
	status = 0;
}

aifFile::~aifFile() {
	if (fd != NULL) {
		Close();
	}
}

int aifFile::Open(const char *filename) {
	ASSERT(filename != NULL);

	if (fd != NULL) {
		Close();
	}

	fd = fopen(filename, "rb");
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

int aifFile::ReadChunk() {
	if (fd == NULL)
		return -1;

	// Read the common header for all aif chunks
	aifHeader header;
	if (fread(&header, 1, sizeof(aifHeader), fd) != sizeof(aifHeader)) {
		fclose(fd);
		fd = NULL;
		return -1;
	}

	switch (header.id) {
		case FORM_ID:
			int aifId;
			if (fread(&aifId, 1, sizeof(int), fd) != sizeof(int))
				return -1;

			// Check aifId signature for valid file
			if (aifId != AIFF_ID)
				return -1;
			break;

		case COMM_ID:
			commHeader comm;
			if (fread(&comm, 1, sizeof(commHeader), fd) != sizeof(commHeader))
				return -1;

			channels = SWAP16(comm.channels);
			samplesPerSec = 44100;
			bitsPerSample = SWAP16(comm.sampleSize);
			status = 1;
			break;

		case SSND_ID:
			ssndHeader ssnd;
			if (fread(&ssnd, 1, sizeof(ssndHeader), fd) != sizeof(ssndHeader))
				return -1;

			if (ssnd.offset) {
				fseek(fd, SWAP32(ssnd.offset), SEEK_CUR);
			}

			chunkSize = SWAP32(header.size) - 8;
			totalSamples = chunkSize / (bitsPerSample/8);
			status = 2;
			break;

		default:
			fseek(fd, SWAP32(header.size), SEEK_CUR);
			break;
	}

	return 0;
}

int aifFile::Read(ByteBuffer *buffer, int size) {
	unsigned char buf[4096];

	if (fd == NULL)
		return -1;

	int bytesRead = 0;
	while (size > 0) {
		// Are we inside a data chunk?
		if (status == 2) {
			// Let's keep reading data!
			int bytes = size < 4096 ? size : 4096;
			if (chunkSize < bytes)
				bytes = chunkSize;
			int ret = fread(buf, 1, bytes, fd);
			if (ret == 0) {
				status = 3;
				break;
			} else {
				chunkSize-= ret;
				if (bitsPerSample == 16) {
					for (int i=0; i<ret; i+= 2) {
						unsigned char tmp;
						tmp = buf[i+0];
						buf[i+0] = buf[i+1];
						buf[i+1] = tmp;
					}
				}

				buffer->putBytes(buf, ret);
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

void aifFile::WriteToFile(const char *filename, ByteBuffer *buffer, int channels, unsigned int sampleRate, int bytesPerSample) {
}

void aifFile::Close() {
	if (fd != NULL) {
		fclose(fd);
		fd = NULL;
	}
}
