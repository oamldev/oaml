#ifndef __OAML_H__
#define __OAML_H__

class ByteBuffer;
class oamlTrack;

class oamlData {
private:
	int debug;
	oamlTrack *tracks[1024];
	int tracksN;

	oamlTrack *curTrack;

	ByteBuffer *dataBuffer;
	ByteBuffer *dbuffer;

	int freq;
	int channels;
	int bytesPerSample;

public:
	oamlData();
	~oamlData();

	int Init(const char *pathToMusic);
	void Shutdown();

	void SetAudioFormat(int audioFreq, int audioChannels, int audioBytesPerSample);
	int PlayTrack(const char *name);
	int PlayTrackId(int id);
	void SetCondition(int id, int value);
	void MixToBuffer(void *buffer, int size);
	void Update();
};

#endif /* __OAML_H__ */
