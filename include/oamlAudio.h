#ifndef __OAMLAUDIO_H__
#define __OAMLAUDIO_H__

#include "wav.h"

class ByteBuffer;

class oamlAudio {
private:
	ByteBuffer *buffer;
	audioFile *handle;
	char filename[1024];
	int type;
	int bars;

	unsigned int bytesPerSample;
	unsigned int samplesCount;
	unsigned int samplesPerSec;
	unsigned int samplesToEnd;
	unsigned int totalSamples;

	float bpm;
	int beatsPerBar;
	int minMovementBars;
	int randomChance;

	unsigned int fadeIn;
	unsigned int fadeInSamples;

	unsigned int fadeOut;
	unsigned int fadeOutSamples;
	unsigned int fadeOutStart;

	unsigned int xfadeIn;
	unsigned int xfadeOut;

	int condId;
	int condType;
	int condValue;
	int condValue2;

public:
	oamlAudio();
	~oamlAudio();

	void SetFilename(const char *audioFilename) { ASSERT(audioFilename != NULL); snprintf(filename, 1024, "%s", audioFilename); }
	void SetType(int audioType) { type = audioType; }
	void SetBPM(float audioBpm) { bpm = audioBpm; }
	void SetBeatsPerBar(int audioBeatsPerBar) { beatsPerBar = audioBeatsPerBar; }
	void SetMinMovementBars(int audioMinMovementBars) { minMovementBars = audioMinMovementBars; }
	void SetRandomChance(int audioRandomChance) { randomChance = audioRandomChance; }
	void SetBars(int audioBars) { bars = audioBars; }
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

	unsigned int GetBarsSamples(int bars);

	bool HasFinished();
	bool HasFinishedTail(unsigned int pos);

	int Open();
	int Read();
	int Read32();
	int Read32(unsigned int pos);

	void DoFadeIn(int msec);
	void DoFadeOut(int msec);

	const char *GetFilename() const { return filename; }
	int GetMinMovementBars() const { return minMovementBars; }
	int GetRandomChance() const { return randomChance; }
	int GetCondId() const { return condId; }
	int GetType() const { return type; }
	unsigned int GetFadeIn() const { return fadeIn; }
	unsigned int GetFadeOut() const { return fadeOut; }
	unsigned int GetXFadeIn() const { return xfadeIn; }
	unsigned int GetXFadeOut() const { return xfadeOut; }

	unsigned int GetSamplesCount() const { return samplesCount; }
};

#endif
