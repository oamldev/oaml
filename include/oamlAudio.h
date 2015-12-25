#ifndef __OAMLAUDIO_H__
#define __OAMLAUDIO_H__

#include "wav.h"

class ByteBuffer;

class oamlAudio {
private:
	ByteBuffer *buffer;
	wavHandle *handle;
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

	unsigned int fadeIn;
	unsigned int fadeInSamples;

	unsigned int fadeOut;
	unsigned int fadeOutSamples;
	unsigned int fadeOutStart;

	int condId;
	int condType;
	int condValue;
	int condValue2;

public:
	oamlAudio();
	~oamlAudio();

	void SetFilename(const char *audioFilename) { ASSERT(audioFilename != NULL); strcpy(filename, audioFilename); }
	void SetType(int audioType) { type = audioType; }
	void SetBPM(float audioBpm) { bpm = audioBpm; }
	void SetBeatsPerBar(float audioBeatsPerBar) { beatsPerBar = audioBeatsPerBar; }
	void SetMinMovementBars(int audioMinMovementBars) { minMovementBars = audioMinMovementBars; }
	void SetBars(float audioBars) { bars = audioBars; }
	void SetFadeIn(int audioFadeIn) { fadeIn = audioFadeIn; }
	void SetFadeOut(int audioFadeOut) { fadeOut = audioFadeOut; }

	void SetCondId(int audioCondId) { condId = audioCondId; }
	void SetCondType(int audioCondType) { condType = audioCondType; }
	void SetCondValue(int audioCondValue) { condValue = audioCondValue; }
	void SetCondValue2(int audioCondValue2) { condValue2 = audioCondValue2; }

	void SetCondition(int id, int type, int value, int value2 = 0);
	bool TestCondition(int id, int value);

	bool HasFinished();

	int Open();
	int Read();
	int Read32();

	void DoFadeIn(int msec);
	void DoFadeOut(int msec);

	const char *GetFilename() const { return filename; }
	int GetCondId() const { return condId; }
	int GetType() const { return type; }
};

#endif
