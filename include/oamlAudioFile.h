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

#ifndef __OAMLAUDIOFILE_H__
#define __OAMLAUDIOFILE_H__

#ifdef __HAVE_SOXR
#include <soxr.h>
#endif

class ByteBuffer;

class oamlAudioFile {
private:
	oamlBase *base;
	bool verbose;
	oamlFileCallbacks *fcbs;

#ifdef __HAVE_SOXR
	soxr_t soxr;
#endif

	ByteBuffer buffer;
	audioFile *handle;
	std::string filename;
	std::string layer;
	int randomChance;
	float gain;

	int format;
	unsigned int bytesPerSample;
	unsigned int samplesPerSec;
	unsigned int totalSamples;
	unsigned int channelCount;
	unsigned int samplesToEnd;

	int fileFormat;
	unsigned int fileBytesPerSample;

	bool chance;
	bool lastChance;

	oamlRC OpenFile();

	int Read();
	int Read32(unsigned int pos);

public:
	oamlAudioFile(std::string _filename, oamlBase *_base, oamlFileCallbacks *cbs, bool _verbose);
	~oamlAudioFile();

	void SetFilename(std::string _filename) { filename = _filename; }
	void SetLayer(std::string _layer) { layer = _layer; }
	void SetRandomChance(int _randomChance) { randomChance = _randomChance; }
	void SetGain(float _gain) { gain = _gain; }

	std::string GetFilename() const { return filename; }
	const char *GetFilenameStr() const { return filename.c_str(); }
	std::string GetLayer() const { return layer; }
	int GetRandomChance() { return randomChance; }
	float GetGain() { return gain; }

	oamlRC Open();
	oamlRC Load();
	int LoadProgress();
	float ReadFloat(unsigned int pos, bool isTail = false);

	unsigned int GetChannels() const { return channelCount; }
	unsigned int GetTotalSamples() const { return totalSamples; }
	unsigned int GetSamplesPerSec() const { return samplesPerSec; }
	void SetSamplesToEnd(unsigned int samples) { samplesToEnd = samples; }

	void FreeMemory();
};

#endif
