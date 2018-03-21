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

#include "vorbis/codec.h"
#include "vorbis/vorbisfile.h"

#include "oamlCommon.h"


static size_t oggFile_read(void *ptr, size_t size, size_t nmemb, void *datasource) {
	oggFile *ogg = (oggFile*)datasource;
	return ogg->GetFileCallbacks()->read(ptr, size, nmemb, ogg->GetFD());
}

static int oggFile_seek(void *datasource, ogg_int64_t offset, int whence) {
	oggFile *ogg = (oggFile*)datasource;
	return ogg->GetFileCallbacks()->seek(ogg->GetFD(), offset, whence);
}

int oggFile_close(void *datasource) {
	oggFile *ogg = (oggFile*)datasource;
	return ogg->GetFileCallbacks()->close(ogg->GetFD());
}

long oggFile_tell(void *datasource) {
	oggFile *ogg = (oggFile*)datasource;
	return ogg->GetFileCallbacks()->tell(ogg->GetFD());
}

oggFile::oggFile(oamlFileCallbacks *cbs) : audioFile(cbs) {
	fcbs = cbs;
	fd = NULL;
	vf = NULL;

	channels = 0;
	samplesPerSec = 0;
	bitsPerSample = 0;
	totalSamples = 0;

	currentSection = 0;
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

	fd = fcbs->open(filename);
	if (fd == NULL) {
		printf("Error opening '%s'\n", filename);
		return -1;
	}

	OggVorbis_File *ovf = new OggVorbis_File;

	ov_callbacks ogg_callbacks = {
		oggFile_read,
		oggFile_seek,
		oggFile_close,
		oggFile_tell
	};

	if (ov_open_callbacks((void*)this, ovf, NULL, 0, ogg_callbacks) < 0) {
		printf("Error opening '%s'\n", filename);
		return -1;
	}

	vorbis_info *vi = ov_info(ovf, -1);
	if (vi == NULL) {
		printf("Error reading info '%s'\n", filename);
		return -1;
	}

	channels = vi->channels;
	samplesPerSec = vi->rate;
	bitsPerSample = 16;
	totalSamples = (int)ov_pcm_total(ovf, -1) * channels;

	vf = (void*)ovf;

	return 0;
}

int oggFile::Read(char *buffer, int size) {
	if (vf == NULL)
		return -1;

	OggVorbis_File *ovf = (OggVorbis_File *)vf;

	return ov_read(ovf, buffer, size, 0, 2, 1, &currentSection);
}

void oggFile::WriteToFile(const char *, ByteBuffer *, int, unsigned int, int) {
}

void oggFile::Close() {
	if (vf != NULL) {
		OggVorbis_File *ovf = (OggVorbis_File *)vf;
		ov_clear(ovf);
		delete ovf;
		vf = NULL;
	}
}
