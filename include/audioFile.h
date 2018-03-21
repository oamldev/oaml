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

#ifndef __AUDIOFILE_H__
#define __AUDIOFILE_H__

enum {
	AF_FORMAT_SINT8,
	AF_FORMAT_SINT16,
	AF_FORMAT_SINT24,
	AF_FORMAT_SINT32,
	AF_FORMAT_FLOAT32
};

class audioFile {
protected:
	oamlFileCallbacks *fcbs;

	void *fd;

public:

	audioFile(oamlFileCallbacks *cbs);
	virtual ~audioFile();

	virtual int GetFormat() const = 0;
	virtual int GetChannels() const = 0;
	virtual int GetSamplesPerSec() const = 0;
	virtual int GetBytesPerSample() const = 0;
	virtual int GetTotalSamples() const = 0;

	virtual int Open(const char *filename) = 0;
	virtual int Read(char *, int size) = 0;

	virtual void WriteToFile(const char *filename, ByteBuffer *buffer, int channels, unsigned int sampleRate, int bytesPerSample) = 0;

	virtual void Close() = 0;

	oamlFileCallbacks* GetFileCallbacks() const { return fcbs; }
	void* GetFD() const { return fd; }
};

#endif /* __AUDIOFILE_H__ */
