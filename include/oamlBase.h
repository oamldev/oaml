#ifndef __OAMLBASE_H__
#define __OAMLBASE_H__

class oamlBase {
private:
	int debug;
	oamlTrack *tracks[1024];
	int tracksN;

	oamlTrack *curTrack;

	ByteBuffer *dbuffer;

	int freq;
	int channels;
	int bytesPerSample;

	int tension;
	uint64_t tensionMs;
	int volume;

	uint64_t timeMs;

	void ShowPlayingTracks();
	int ReadDefs(const char *filaname, const char *path);

public:
	oamlBase();
	~oamlBase();

	int Init(const char *pathToMusic);
	void Shutdown();

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

	void SetCondition(int id, int value);

	void MixToBuffer(void *buffer, int size);

	void Update();
};

#endif /* __OAMLBASE_H__ */
