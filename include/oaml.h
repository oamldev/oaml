#ifndef __OAML_H__
#define __OAML_H__


//
// Definitions
//

#define OAML_VOLUME_MIN	0
#define OAML_VOLUME_MAX	100

enum {
	COND_TYPE_EQUAL		= 0,
	COND_TYPE_GREATER	= 1,
	COND_TYPE_LESS		= 2,
	COND_TYPE_RANGE		= 3
};

//
// Internal declarations
//

class oamlBase;

//
// Main class
//

class oamlApi {
private:
	oamlBase *oaml;

public:
	oamlApi();
	~oamlApi();

	int Init(const char *pathToMusic);
	void Shutdown();

	void SetDebugClipping(bool option);
	void SetMeasureDecibels(bool option);
	void SetWriteAudioAtShutdown(bool option);

	void SetAudioFormat(int freq, int channels, int bytesPerSample);
	void SetVolume(int vol);
	int GetVolume();

	int PlayTrack(const char *name);
	int PlayTrackId(int id);

	void StopPlaying();

	bool IsTrackPlaying(const char *name);
	bool IsTrackPlayingId(int id);
	bool IsPlaying();

	void AddTension(int value);

	void SetCondition(int id, int value);

	void MixToBuffer(void *buffer, int size);

	void Update();
};

#endif /* __OAML_H__ */
