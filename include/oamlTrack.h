#ifndef __OAMLTRACK_H__
#define __OAMLTRACK_H__

class ByteBuffer;
class oamlAudio;

class oamlTrack {
private:
	char name[256];
	int mode;
	float bpm;

	int loopCount;
	int condCount;

	int fadeIn;
	int fadeOut;
	int xfadeIn;
	int xfadeOut;

	oamlAudio *loopAudios[256];
	oamlAudio *condAudios[256];
	oamlAudio *introAudio;
	oamlAudio *endAudio;

	oamlAudio *curAudio;
	oamlAudio *tailAudio;
	oamlAudio *fadeAudio;

	int Random(int min, int max);

public:
	oamlTrack(const char *trackName, int trackMode, float trackBpm, int trackFadeIn, int trackFadeOut, int trackXfadeIn, int trackXfadeOut);
	~oamlTrack();

	char *GetName() { return name; }

	void AddAudio(oamlAudio *audio);
	void Play();
	void PlayNext();
	void XFadePlay();

	void Stop();

	bool IsPlaying();

	int Read32();

	void SetCondition(int id, int value);
};

#endif
