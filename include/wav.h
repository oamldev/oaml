#ifndef __WAV_H__
#define __WAV_H__

typedef struct {
	FILE *fd;

	int format;
	int channels;
	int samplesPerSec;
	int bitsPerSample;
	int totalSamples;

	int chunkSize;
	int status;
} wavHandle;

wavHandle *wavOpen(const char *filename);
int wavRead(wavHandle *handle, ByteBuffer *buffer, int size);
int wavReadChunk(wavHandle *handle);
void wavWriteToFile(const char *filename, ByteBuffer *buffer, int channels, unsigned int sampleRate, int bytesPerSample);
void wavClose(wavHandle *handle);

#endif /* __WAV_H__ */
