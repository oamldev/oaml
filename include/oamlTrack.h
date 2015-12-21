#ifndef __OAMLTRACK_H__
#define __OAMLTRACK_H__

#include "ByteBuffer.h"
#include "oamlAudio.h"

class oamlTrack {
private:
	char name[256];
	int mode;
	float bpm;

	int loopCount;
	int condCount;

	oamlAudio *loopAudios[256];
	oamlAudio *condAudios[256];
	oamlAudio *introAudio;
	oamlAudio *endAudio;

	oamlAudio *curAudio;
	oamlAudio *tailAudio;

	int Random(int min, int max);

public:
	oamlTrack(const char *trackName, int trackMode, float trackBpm);
	~oamlTrack();

	char *GetName() { return name; }

	void AddAudio(oamlAudio *audio);
	void Play();
	void PlayNext();
	int Read(ByteBuffer *buffer, int size);

	void SetCondition(int id, int value);
};

#endif
