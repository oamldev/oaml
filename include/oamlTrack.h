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

#ifndef __OAMLTRACK_H__
#define __OAMLTRACK_H__

class ByteBuffer;
class oamlAudio;

class oamlTrack {
private:
	std::string name;
	int mode;

	int fadeIn;
	int fadeOut;
	int xfadeIn;
	int xfadeOut;

	int playCondSamples;

	unsigned int tailPos;

	std::vector<oamlAudio*> loopAudios;
	std::vector<oamlAudio*> randAudios;
	std::vector<oamlAudio*> condAudios;
	oamlAudio *introAudio;
	oamlAudio *endAudio;
	oamlAudio *playCondAudio;

	oamlAudio *curAudio;
	oamlAudio *tailAudio;
	oamlAudio *fadeAudio;

	int Random(int min, int max);
	oamlAudio* PickNextAudio();

	void PlayNext();
	void PlayCond(oamlAudio *audio);
	void PlayCondWithMovement(oamlAudio *audio);
	void XFadePlay();

	void ClearAudios(std::vector<oamlAudio*> *audios);

public:
	oamlTrack();
	~oamlTrack();

	void SetName(std::string trackName) { name = trackName; }
	void SetMode(int trackMode) { mode = trackMode; }
	void SetFadeIn(int trackFadeIn) { fadeIn = trackFadeIn; }
	void SetFadeOut(int trackFadeOut) { fadeOut = trackFadeOut; }
	void SetXFadeIn(int trackXFadeIn) { xfadeIn = trackXFadeIn; }
	void SetXFadeOut(int trackXFadeOut) { xfadeOut = trackXFadeOut; }

	const char *GetNameStr() const { return name.c_str(); }
	std::string GetName() const { return name; }
	int GetFadeIn() const { return fadeIn; }
	int GetFadeOut() const { return fadeOut; }
	int GetXFadeIn() const { return xfadeIn; }
	int GetXFadeOut() const { return xfadeOut; }

	void AddAudio(oamlAudio *audio);
	void Play();
	void Stop();

	bool IsPlaying();
	void ShowPlaying();
	void ShowInfo();
	std::string GetPlayingInfo();

	int Mix32(int sample, oamlBase *oaml);

	void SetCondition(int id, int value);
};

#endif
