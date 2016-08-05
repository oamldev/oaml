//-----------------------------------------------------------------------------
// Copyright (c) 2015-2016 Marcelo Fernandez
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

#ifndef __OAMLMUSICTRACK_H__
#define __OAMLMUSICTRACK_H__

class ByteBuffer;
class oamlAudio;

class oamlMusicTrack : public oamlTrack {
private:
	bool playing;
	int playCondSamples;

	unsigned int filesSamples;
	unsigned int tailPos;

	std::vector<oamlAudio*> loopAudios;
	std::vector<oamlAudio*> randAudios;
	std::vector<oamlAudio*> condAudios;
	std::vector<oamlAudio*> introAudios;
	oamlAudio *playCondAudio;

	oamlAudio *curAudio;
	oamlAudio *tailAudio;
	oamlAudio *fadeAudio;

	oamlAudio* PickNextAudio();

	void PlayNext();
	void PlayCond(oamlAudio *audio);
	void PlayCondWithMovement(oamlAudio *audio);
	void XFadePlay();

public:
	oamlMusicTrack(bool _verbose);
	~oamlMusicTrack();

	void GetAudioList(std::vector<std::string>& list);
	void AddAudio(oamlAudio *audio);
	oamlAudio* GetAudio(std::string filename);
	oamlRC RemoveAudio(std::string filename);
	oamlRC Play();
	oamlRC Load();
	float LoadProgress();
	void Stop();

	bool IsPlaying();
	void ShowPlaying();
	std::string GetPlayingInfo();

	void Mix(float *samples, int channels, bool debugClipping);

	void SetCondition(int id, int value);

	bool IsMusicTrack() const { return true; }

	void ReadInfo(oamlTrackInfo *info);

	void FreeMemory();
};

#endif
