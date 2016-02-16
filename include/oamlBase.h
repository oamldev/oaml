//-----------------------------------------------------------------------------
// Copyright (c) 2015-2016 Marcelo Fernandez
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

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
	std::string defsFile;
	std::string playingInfo;

	bool debugClipping;
	bool writeAudioAtShutdown;
	bool useCompressor;
	double avgDecibels;

	std::vector<oamlTrack*> tracks;

	oamlTrack *curTrack;

	ByteBuffer *fullBuffer;

	int sampleRate;
	int channels;
	int bytesPerSample;
	bool floatBuffer;

	int tension;
	uint64_t tensionMs;
	int volume;
	bool pause;

	oamlFileCallbacks *fcbs;

	uint64_t timeMs;

	oamlCompressor compressor;

	oamlTracksInfo tracksInfo;

	void Clear();

	int PlayTrackId(int id);
	bool IsTrackPlayingId(int id);

	void ShowPlayingTracks();
	int ReadDefs(const char *filaname);
	void ReadInternalDefs(const char *filaname);

	int ReadSample(void *buffer, int index);
	void WriteSample(void *buffer, int index, int sample);

	bool IsAudioFormatSupported();

	void Log(const char* fmt, ...);

public:
	oamlBase();
	~oamlBase();

	int Init(const char *defsFilename);
	int InitString(const char *defs);
	void Shutdown();

	void SetDebugClipping(bool option) { debugClipping = option; }
	void SetWriteAudioAtShutdown(bool option) { writeAudioAtShutdown = option; }

	void SetAudioFormat(int audioSampleRate, int audioChannels, int audioBytesPerSample, bool audioFloatBuffer);
	void SetVolume(int vol);
	int GetVolume() const { return volume; }

	int PlayTrack(const char *name);
	int PlayTrackWithStringRandom(const char *str);

	void StopPlaying();
	void Pause();
	void Resume();
	void PauseToggle();

	bool IsPaused() const { return pause; }

	bool IsTrackPlaying(const char *name);
	bool IsPlaying();

	void AddTension(int value);

	void SetMainLoopCondition(int value);

	void SetCondition(int id, int value);

	void MixToBuffer(void *buffer, int size);

	void Update();

	int SafeAdd(int sample1, int sample2);

	void SetFileCallbacks(oamlFileCallbacks *cbs);

	void EnableDynamicCompressor(bool enable, double thresholdDb, double ratio);

	oamlTracksInfo *GetTracksInfo();

	const char* GetDefsFile();
	const char* GetPlayingInfo();
};

#endif /* __OAMLBASE_H__ */
