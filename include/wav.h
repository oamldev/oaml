#ifndef __WAV_H__
#define __WAV_H__

class wavFile {
private:
	FILE *fd;

	int format;
	int channels;
	int samplesPerSec;
	int bitsPerSample;

	int chunkSize;
	int status;

	int ReadChunk();
public:
	wavFile();
	~wavFile();

	int GetFormat() const { return format; }
	int GetChannels() const { return channels; }
	int GetSamplesPerSec() const { return samplesPerSec; }
	int GetBitsPerSample() const { return bitsPerSample; }
	int GetBytesPerSample() const { return bitsPerSample / 8; }
	int GetChunkSize() const { return chunkSize; }

	int Open(const char *filename);
	int Read(ByteBuffer *buffer, int size);

	void WriteToFile(const char *filename, ByteBuffer *buffer, int channels, unsigned int sampleRate, int bytesPerSample);

	void Close();
};

#endif /* __WAV_H__ */
