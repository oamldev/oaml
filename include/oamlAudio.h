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

	unsigned int fadeIn;
	unsigned int fadeInSamples;

	unsigned int fadeOut;
	unsigned int fadeOutSamples;
	unsigned int fadeOutStart;

	int condId;
	int condType;
	int condValue;

public:
	oamlAudio(const char *audioFilename, int audioType, int audioBars, float audioBpm, int audioBeatsPerBar, unsigned int audioFadeIn, unsigned int audioFadeOut);
	~oamlAudio();

	void SetCondition(int id, int type, int value);
	bool TestCondition(int id, int value);

	bool HasFinished();

	int Open();
	int Read();
	int Read32();

	void DoFadeIn(int msec);
	void DoFadeOut(int msec);

	int GetCondId() { return condId; }
	int GetType() { return type; }
};

#endif
