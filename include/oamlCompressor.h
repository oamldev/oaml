#ifndef __OAMLCOMPRESSOR_H__
#define __OAMLCOMPRESSOR_H__

class oamlCompressor {
private:
	int chnum;

	double threshold;
	double ratio;
	double attackTime;
	double releaseTime;

	double att;
	double rel;
	double env;

public:
	oamlCompressor();
	~oamlCompressor();

	void SetThreshold(double threshold);
	void SetRatio(double ratio);

	void SetAudioFormat(int channels, int sampleRate);
	void ProcessData(float *data);
};

#endif
