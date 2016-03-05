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
protected:
	std::string name;
	std::string group;
	std::string subgroup;

	int fadeIn;
	int fadeOut;
	int xfadeIn;
	int xfadeOut;

	int Random(int min, int max);

	void ApplyVolPanTo(float *samlpes, int channels, float vol, float pan);
	float SafeAdd(float a, float b, bool debug);
	void MixAudio(oamlAudio *audio, float *samples, int channels, bool debug);
	unsigned int MixAudio(oamlAudio *audio, float *samples, int channels, bool debug, unsigned int pos);

	void ClearAudios(std::vector<oamlAudio*> *audios);

public:
	oamlTrack();
	virtual ~oamlTrack();

	void SetName(std::string trackName) { name = trackName; }
	void SetGroup(std::string trackGroup) { group = trackGroup; }
	void SetSubgroup(std::string trackSubgroup) { subgroup = trackSubgroup; }
	void SetFadeIn(int trackFadeIn) { fadeIn = trackFadeIn; }
	void SetFadeOut(int trackFadeOut) { fadeOut = trackFadeOut; }
	void SetXFadeIn(int trackXFadeIn) { xfadeIn = trackXFadeIn; }
	void SetXFadeOut(int trackXFadeOut) { xfadeOut = trackXFadeOut; }

	const char *GetNameStr() const { return name.c_str(); }
	std::string GetName() const { return name; }
	std::string GetGroup() const { return group; }
	std::string GetSubgroup() const { return subgroup; }
	int GetFadeIn() const { return fadeIn; }
	int GetFadeOut() const { return fadeOut; }
	int GetXFadeIn() const { return xfadeIn; }
	int GetXFadeOut() const { return xfadeOut; }

	virtual void AddAudio(oamlAudio *) { }
	virtual void Play() { }
	virtual int Play(const char *) { return -1; }
	virtual int Play(const char *, float, float) { return -1; }
	virtual void Stop() { }

	virtual bool IsPlaying() { return false; }
	void ShowPlaying();
	virtual void ShowInfo() { }
	virtual std::string GetPlayingInfo() { return ""; }

	virtual void Mix(float *, int, bool) { }

	virtual void SetCondition(int, int) { }

	virtual bool IsMusicTrack() const { return false; }
	virtual bool IsSfxTrack() const { return false; }
};

#endif
