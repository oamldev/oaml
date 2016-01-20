#ifndef __OGG_H__
#define __OGG_H__

class oggFile : public audioFile {
private:
	void *vf;

	int format;
	int channels;
	int samplesPerSec;
	int bitsPerSample;
	int totalSamples;

	int currentSection;
public:
	oggFile(oamlFileCallbacks *cbs);
	~oggFile();

	int GetFormat() const { return format; }
	int GetChannels() const { return channels; }
	int GetSamplesPerSec() const { return samplesPerSec; }
	int GetBitsPerSample() const { return bitsPerSample; }
	int GetBytesPerSample() const { return bitsPerSample / 8; }
	int GetTotalSamples() const { return totalSamples; }

	void WriteToFile(const char *filename, ByteBuffer *buffer, int channels, unsigned int sampleRate, int bytesPerSample);

	int Open(const char *filename);
	int Read(ByteBuffer *buffer, int size);

	void Close();
};

#endif /* __OGG_H__ */
