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


#define	SWAP16(x) ((((x) & 0xff) << 8) | (((x) & 0xff00) >> 8))
#define	SWAP32(x) ((((x) & 0xff) << 24) | (((x) & 0xff00) << 8) | (((x) & 0xff0000) >> 8) | (((x) >> 24) & 0xff))

enum {
	AIFF_ID = 0x46464941,
	FORM_ID = 0x4D524F46,
	COMM_ID = 0x4D4D4F43,
	SSND_ID = 0x444E5353
};

#ifdef _MSC_VER
#pragma pack(push,1)
#define __PACKED
#else
#define __PACKED __attribute__((packed))
#endif

typedef struct {
	int id;
	unsigned int size;
} __PACKED aifHeader;

typedef struct {
	int id;
	unsigned int size;
	int aiff;
} __PACKED formHeader;

typedef struct {
	unsigned int offset;
	unsigned int blockSize;
} __PACKED ssndHeader;

typedef struct {
	unsigned short channels;
	unsigned int sampleFrames;
	unsigned short sampleSize;
	unsigned char sampleRate80[10];
} __PACKED commHeader;

#ifdef _MSC_VER
#pragma pack(pop)
#endif


aifFile::aifFile(oamlFileCallbacks *cbs) : audioFile(cbs) {
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

/*
 * C O N V E R T   F R O M   I E E E   E X T E N D E D  
 */

/* 
 * Copyright (C) 1988-1991 Apple Computer, Inc.
 * All rights reserved.
 *
 * Machine-independent I/O routines for IEEE floating-point numbers.
 *
 * NaN's and infinities are converted to HUGE_VAL or HUGE, which
 * happens to be infinity on IEEE machines.  Unfortunately, it is
 * impossible to preserve NaN's in a machine-independent way.
 * Infinities are, however, preserved on IEEE machines.
 *
 * These routines have been tested on the following machines:
 *    Apple Macintosh, MPW 3.1 C compiler
 *    Apple Macintosh, THINK C compiler
 *    Silicon Graphics IRIS, MIPS compiler
 *    Cray X/MP and Y/MP
 *    Digital Equipment VAX
 *
 *
 * Implemented by Malcolm Slaney and Ken Turkowski.
 *
 * Malcolm Slaney contributions during 1988-1990 include big- and little-
 * endian file I/O, conversion to and from Motorola's extended 80-bit
 * floating-point format, and conversions to and from IEEE single-
 * precision floating-point format.
 *
 * In 1991, Ken Turkowski implemented the conversions to and from
 * IEEE double-precision format, added more precision to the extended
 * conversions, and accommodated conversions involving +/- infinity,
 * NaN's, and denormalized numbers.
 */

#ifndef HUGE_VAL
# define HUGE_VAL HUGE
#endif /*HUGE_VAL*/

# define UnsignedToFloat(u)         (((double)((long)(u - 2147483647L - 1))) + 2147483648.0)

/****************************************************************
 * Extended precision IEEE floating-point conversion routine.
 ****************************************************************/

double ConvertFromIeeeExtended(unsigned char *bytes) {
	double    f;
	int    expon;
	unsigned long hiMant, loMant;

	expon = ((bytes[0] & 0x7F) << 8) | (bytes[1] & 0xFF);
	hiMant =  ((unsigned long)(bytes[2] & 0xFF) << 24)
		| ((unsigned long)(bytes[3] & 0xFF) << 16)
		| ((unsigned long)(bytes[4] & 0xFF) << 8)
		| ((unsigned long)(bytes[5] & 0xFF));
	loMant =  ((unsigned long)(bytes[6] & 0xFF) << 24)
		| ((unsigned long)(bytes[7] & 0xFF) << 16)
		| ((unsigned long)(bytes[8] & 0xFF) << 8)
		| ((unsigned long)(bytes[9] & 0xFF));

	if (expon == 0 && hiMant == 0 && loMant == 0) {
		f = 0;
	} else {
		if (expon == 0x7FFF) {    /* Infinity or NaN */
			f = HUGE_VAL;
		} else {
			expon -= 16383;
			f  = ldexp(UnsignedToFloat(hiMant), expon-=31);
			f += ldexp(UnsignedToFloat(loMant), expon-=32);
		}
	}

	if (bytes[0] & 0x80)
		return -f;
	else
		return f;
}

int aifFile::ReadChunk() {
	if (fd == NULL)
		return -1;

	// Read the common header for all aif chunks
	aifHeader header;
	if (fcbs->read(&header, 1, sizeof(aifHeader), fd) != sizeof(aifHeader)) {
		fcbs->close(fd);
		fd = NULL;
		return -1;
	}

	switch (header.id) {
		case FORM_ID:
			int aifId;
			if (fcbs->read(&aifId, 1, sizeof(int), fd) != sizeof(int))
				return -1;

			// Check aifId signature for valid file
			if (aifId != AIFF_ID) {
				fprintf(stderr, "aif: Invalid signature\n");
				return -1;
			}
			break;

		case COMM_ID:
			commHeader comm;
			if (fcbs->read(&comm, 1, sizeof(commHeader), fd) != sizeof(commHeader))
				return -1;

			channels = SWAP16(comm.channels);
			samplesPerSec = (int)ConvertFromIeeeExtended(comm.sampleRate80);
			bitsPerSample = SWAP16(comm.sampleSize);
			switch (bitsPerSample) {
				case 8: format = AF_FORMAT_SINT8; break;
				case 16: format = AF_FORMAT_SINT16; break;
				case 24: format = AF_FORMAT_SINT24; break;
				case 32: format = AF_FORMAT_SINT32; break;
			}
			status = 1;
			break;

		case SSND_ID:
			ssndHeader ssnd;
			if (fcbs->read(&ssnd, 1, sizeof(ssndHeader), fd) != sizeof(ssndHeader))
				return -1;

			if (ssnd.offset) {
				fcbs->seek(fd, SWAP32(ssnd.offset), SEEK_CUR);
			}

			chunkSize = SWAP32(header.size) - 8;
			totalSamples = chunkSize / (bitsPerSample/8);
			status = 2;
			break;

		default:
			fcbs->seek(fd, SWAP32(header.size), SEEK_CUR);
			break;
	}

	return 0;
}

int aifFile::Read(char *buffer, int size) {
	if (fd == NULL)
		return -1;

	int bytesRead = 0;
	while (size > 0) {
		// Are we inside a ssnd chunk?
		if (status == 2) {
			// Let's keep reading data!
			int ret = fcbs->read(buffer, 1, size, fd);
			if (ret == 0) {
				status = 3;
				break;
			} else {
				chunkSize-= ret;

				if (bitsPerSample == 8) {
					char *cbuf = (char*)buffer;
					for (int i=0; i<ret; i++) {
						cbuf[i] = cbuf[i]+128;
					}
				} else
				if (bitsPerSample == 16) {
					unsigned short *sbuf = (unsigned short *)buffer;
					for (int i=0; i<ret/2; i++) {
						sbuf[i] = SWAP16(sbuf[i]);
					}
				} else
				if (bitsPerSample == 24) {
					for (int i=0; i<ret; i+= 3) {
						unsigned char tmp;
						tmp = buffer[i+0];
						buffer[i+0] = buffer[i+2];
						buffer[i+2] = tmp;
					}
				} else
				if (bitsPerSample == 32) {
					uint32_t *ibuf = (uint32_t *)buffer;
					for (int i=0; i<ret/4; i++) {
						ibuf[i] = SWAP32(ibuf[i]);
					}
				}

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

void aifFile::WriteToFile(const char *, ByteBuffer *, int, unsigned int, int) {
}

void aifFile::Close() {
	if (fd != NULL) {
		fcbs->close(fd);
		fd = NULL;
	}
}

