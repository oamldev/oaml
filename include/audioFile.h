#ifndef __AUDIOFILE_H__
#define __AUDIOFILE_H__

class audioFile {
protected:
	oamlFileCallbacks *fcbs;

	void *fd;

public:

	audioFile(oamlFileCallbacks *cbs);
	virtual ~audioFile();

	virtual int GetFormat() const = 0;
	virtual int GetChannels() const = 0;
	virtual int GetSamplesPerSec() const = 0;
	virtual int GetBytesPerSample() const = 0;
	virtual int GetTotalSamples() const = 0;

	virtual int Open(const char *filename) = 0;
	virtual int Read(ByteBuffer *buffer, int size) = 0;

	virtual void WriteToFile(const char *filename, ByteBuffer *buffer, int channels, unsigned int sampleRate, int bytesPerSample) = 0;

	virtual void Close() = 0;

	oamlFileCallbacks* GetFileCallbacks() const { return fcbs; }
	void* GetFD() const { return fd; }
};

#endif /* __AUDIOFILE_H__ */
