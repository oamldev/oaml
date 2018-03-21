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

#ifndef __OAMLSFXTRACK_H__
#define __OAMLSFXTRACK_H__

class ByteBuffer;
class oamlAudio;

typedef struct {
	oamlAudio *audio;
	int pos;
	float vol;
	float pan;
} sfxPlayInfo;

class oamlSfxTrack : public oamlTrack {
private:
	std::vector<oamlAudio*> sfxAudios;
	std::vector<sfxPlayInfo> playingAudios;

public:
	oamlSfxTrack(bool _verbose);
	~oamlSfxTrack();

	void GetAudioList(std::vector<std::string>& list);
	void AddAudio(oamlAudio *audio);
	oamlAudio* GetAudio(std::string filename);
	oamlRC Play(const char *name, float vol, float pan);
	void Stop();

	bool IsPlaying();
	std::string GetPlayingInfo();

	void Mix(float *samples, int channels, bool debugClipping);

	bool IsSfxTrack() const { return true; }

	void ReadInfo(oamlTrackInfo *info);

	void FreeMemory();
};

#endif
