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

#ifndef __OAMLMUSICTRACK_H__
#define __OAMLMUSICTRACK_H__

class ByteBuffer;
class oamlAudio;

class oamlMusicTrack : public oamlTrack {
private:
	bool playing;
	int playingOrder;
	int maxPlayOrder;

	unsigned int filesSamples;
	unsigned int tailPos;

	std::vector<oamlAudio*> loopAudios;
	std::vector<oamlAudio*> randAudios;
	std::vector<oamlAudio*> condAudios;
	std::vector<oamlAudio*> introAudios;

	int curAudio;
	int tailAudio;
	int fadeAudio;
	int playCondAudio;
	int playCondSamples;

	oamlAudio* GetAudioByTypeId(int type, int id);
	oamlAudio* GetCurAudio();
	oamlAudio* GetTailAudio();
	oamlAudio* GetFadeAudio();

	int PickNextAudio();

	void SetCurAudio(int type, int id);

	void PlayNext();
	void PlayCond(int audio);
	void PlayCondWithMovement(int audio);
	void XFadePlay();

	void SaveAudioState(tinyxml2::XMLDocument &doc, tinyxml2::XMLElement *node, const char *nodeName, std::vector<oamlAudio*> *audios);
	void LoadAudioState(tinyxml2::XMLElement *node, std::vector<oamlAudio*> *audios);
	void _SetLayerGain(std::vector<oamlAudio*> *audios, std::string layer, float gain);

public:
	oamlMusicTrack(bool _verbose);
	~oamlMusicTrack();

	void GetAudioList(std::vector<std::string>& list);
	void AddAudio(oamlAudio *audio);
	oamlAudio* GetAudio(std::string filename);
	oamlRC RemoveAudio(std::string filename);
	oamlRC Play(int mainCondValue);
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

	void SetLayerGain(std::string layer, float gain);

	void SaveState(tinyxml2::XMLDocument &doc, tinyxml2::XMLElement *node);
	void LoadState(tinyxml2::XMLElement *node);

	void FreeMemory();
};

#endif
