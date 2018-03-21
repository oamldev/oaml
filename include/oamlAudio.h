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

#ifndef __OAMLAUDIO_H__
#define __OAMLAUDIO_H__

class ByteBuffer;

class oamlAudio {
private:
	oamlBase *base;
	bool verbose;
	oamlFileCallbacks *fcbs;

	std::vector<oamlAudioFile> files;
	std::string name;
	int type;
	int bars;
	float volume;

	unsigned int samplesCount;
	unsigned int samplesPerSec;
	unsigned int samplesToEnd;
	unsigned int totalSamples;
	unsigned int filesSamples;
	unsigned int channelCount;

	float bpm;
	int beatsPerBar;
	int minMovementBars;
	int randomChance;
	int playOrder;

	unsigned int fadeIn;
	unsigned int fadeInSamples;

	unsigned int fadeOut;
	unsigned int fadeOutSamples;
	unsigned int fadeOutCount;

	unsigned int xfadeIn;
	unsigned int xfadeOut;

	int condId;
	int condType;
	int condValue;
	int condValue2;

	bool pickable;

	void UpdateSamplesToEnd();

public:
	oamlAudio(oamlBase *_base, oamlFileCallbacks *cbs, bool _verbose);
	~oamlAudio();

	void SetName(std::string _name) { name = _name; }
	void SetType(int audioType) { type = audioType; }
	void SetVolume(float audioVolume) { volume = audioVolume; }
	void SetBPM(float _bpm);
	void SetBeatsPerBar(int _beatsPerBar);
	void SetBars(int _bars);
	void SetMinMovementBars(int audioMinMovementBars) { minMovementBars = audioMinMovementBars; }
	void SetRandomChance(int audioRandomChance) { randomChance = audioRandomChance; }
	void SetPlayOrder(int _playOrder) { playOrder = _playOrder; }
	void SetFadeIn(unsigned int audioFadeIn) { fadeIn = audioFadeIn; }
	void SetFadeOut(unsigned int audioFadeOut) { fadeOut = audioFadeOut; }
	void SetXFadeIn(unsigned int audioXFadeIn) { xfadeIn = audioXFadeIn; }
	void SetXFadeOut(unsigned int audioXFadeOut) { xfadeOut = audioXFadeOut; }

	void SetCondId(int audioCondId) { condId = audioCondId; }
	void SetCondType(int audioCondType) { condType = audioCondType; }
	void SetCondValue(int audioCondValue) { condValue = audioCondValue; }
	void SetCondValue2(int audioCondValue2) { condValue2 = audioCondValue2; }

	void SetCondition(int id, int type, int value, int value2 = 0);
	bool TestCondition(int id, int value);
	bool HasCondition(int id) { return id == condId; }

	bool HasFinished();
	bool HasFinishedTail(unsigned int pos);

	oamlRC Open();
	oamlRC Load();
	int LoadProgress();
	float ReadFloat();
	float ReadFloat(unsigned int pos);

	void ReadSamples(float *samples, int channels);
	unsigned int ReadSamples(float *samples, int channels, unsigned int pos);

	void DoFadeIn(int msec);
	void DoFadeOut(int msec);

	void ReadInfo(oamlAudioInfo *info);

	void GetAudioFileList(std::vector<std::string>& list);
	bool HasAudioFile(std::string filename);
	void RemoveAudioFile(std::string filename);
	oamlAudioFile *GetAudioFile(std::string filename);

	void AddAudioFile(std::string filename, std::string layer = "", int randomChance = -1);
	std::string GetName() const { return name; }
	float GetVolume() const { return volume; }
	float GetBPM() const { return bpm; }
	int GetBeatsPerBar() const { return beatsPerBar; }
	int GetBars() const { return bars; }
	int GetMinMovementBars() const { return minMovementBars; }
	int GetRandomChance() const { return randomChance; }
	int GetPlayOrder() const { return playOrder; }
	int GetCondId() const { return condId; }
	int GetCondType() const { return condType; }
	int GetCondValue() const { return condValue; }
	int GetCondValue2() const { return condValue2; }
	int GetType() const { return type; }
	unsigned int GetFadeIn() const { return fadeIn; }
	unsigned int GetFadeOut() const { return fadeOut; }
	unsigned int GetXFadeIn() const { return xfadeIn; }
	unsigned int GetXFadeOut() const { return xfadeOut; }

	unsigned int GetBarsSamples(int bars);
	unsigned int GetSamplesCount() const { return samplesCount; }
	unsigned int GetFilesSamples();

	void SetLayerGain(std::string layer, float gain);

	void SetPickable(bool value) { pickable = value; }
	bool IsPickable() const { return pickable; }

	void SaveState(tinyxml2::XMLElement *node);
	void LoadState(tinyxml2::XMLElement *node);

	void FreeMemory();
};

#endif
