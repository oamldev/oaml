#ifndef __OAMLBASE_H__
#define __OAMLBASE_H__

//
// Reserved condition id's
//
enum {
	CONDITION_TENSION		= 1,
	CONDITION_MAIN_LOOP		= 2,
	CONDITION_USER			= 10
};


class oamlBase {
private:
	bool debugClipping;
	bool writeAudioAtShutdown;
	bool measureDecibels;
	double avgDecibels;

	oamlTrack *tracks[1024];
	int tracksN;

	oamlTrack *curTrack;

	ByteBuffer *fullBuffer;

	int freq;
	int channels;
	int bytesPerSample;

	int tension;
	uint64_t tensionMs;
	int volume;

	oamlFileCallbacks *fcbs;

	uint64_t timeMs;

	void ShowPlayingTracks();
	int ReadDefs(const char *filaname, const char *path);
	void ReadInternalDefs(const char *filaname);

	int ReadSample(void *buffer, int index);
	void WriteSample(void *buffer, int index, int sample);

public:
	oamlBase();
	~oamlBase();

	int Init(const char *pathToMusic);
	void Shutdown();

	void SetDebugClipping(bool option) { debugClipping = option; }
	void SetMeasureDecibels(bool option) { measureDecibels = option; }
	void SetWriteAudioAtShutdown(bool option) { writeAudioAtShutdown = option; }

	void SetAudioFormat(int audioFreq, int audioChannels, int audioBytesPerSample);
	void SetVolume(int vol);
	int GetVolume() const { return volume; }

	int PlayTrack(const char *name);
	int PlayTrackId(int id);

	void StopPlaying();

	bool IsTrackPlaying(const char *name);
	bool IsTrackPlayingId(int id);
	bool IsPlaying();

	void AddTension(int value);

	void SetMainLoopCondition(int value);

	void SetCondition(int id, int value);

	void MixToBuffer(void *buffer, int size);

	void Update();

	int SafeAdd(int sample1, int sample2);

	void SetFileCallbacks(oamlFileCallbacks *cbs);
};

#endif /* __OAMLBASE_H__ */
