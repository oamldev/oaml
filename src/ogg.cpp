#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vorbis/codec.h"
#include "vorbis/vorbisfile.h"

#include "oamlCommon.h"


oggFile::oggFile() {
	fd = NULL;

	format = 0;
	channels = 0;
	samplesPerSec = 0;
	bitsPerSample = 0;
	totalSamples = 0;
}

oggFile::~oggFile() {
	if (fd != NULL) {
		Close();
	}
}

int oggFile::Open(const char *filename) {
	ASSERT(filename != NULL);

	if (fd != NULL) {
		Close();
	}

	OggVorbis_File *vf = new OggVorbis_File;
	if (ov_fopen(filename, vf) < 0) {
		return -1;
	}

	vorbis_info *vi = ov_info(vf, -1);
	if (vi == NULL)
		return -1;

	channels = vi->channels;
	samplesPerSec = vi->rate;
	bitsPerSample = 16;
	totalSamples = (int)ov_pcm_total(vf, -1) * channels;

	fd = (void*)vf;

	return 0;
}

int oggFile::Read(ByteBuffer *buffer, int size) {
	unsigned char buf[4096];

	if (fd == NULL)
		return -1;

	OggVorbis_File *vf = (OggVorbis_File *)fd;

	int bytesRead = 0;
	while (size > 0) {
		// Let's keep reading data!
		int bytes = size < 4096 ? size : 4096;
		int ret = (int)ov_read(vf, (char*)buf, bytes, 0, 2, 1, &currentSection);
		if (ret == 0) {
			break;
		} else {
			buffer->putBytes(buf, ret);
			bytesRead+= ret;
			size-= ret;
		}
	}

	return bytesRead;
}

void oggFile::WriteToFile(const char *filename, ByteBuffer *buffer, int channels, unsigned int sampleRate, int bytesPerSample) {
}

void oggFile::Close() {
	if (fd != NULL) {
		OggVorbis_File *vf = (OggVorbis_File *)fd;
		delete vf;
		fd = NULL;
	}
}
