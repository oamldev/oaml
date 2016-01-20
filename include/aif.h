#ifndef __AIF_H__
#define __AIF_H__

class aifFile : public audioFile {
private:
	int channels;
	int samplesPerSec;
	int bitsPerSample;
	int totalSamples;

	int chunkSize;
	int status;

	int ReadChunk();
public:
	aifFile(oamlFileCallbacks *cbs);
	~aifFile();

	int GetFormat() const { return 0; }
	int GetChannels() const { return channels; }
	int GetSamplesPerSec() const { return samplesPerSec; }
	int GetBitsPerSample() const { return bitsPerSample; }
	int GetBytesPerSample() const { return bitsPerSample / 8; }
	int GetTotalSamples() const { return totalSamples; }

	int Open(const char *filename);
	int Read(ByteBuffer *buffer, int size);

	void WriteToFile(const char *filename, ByteBuffer *buffer, int channels, unsigned int sampleRate, int bytesPerSample);

	void Close();
};

#endif /* __AIF_H__ */
