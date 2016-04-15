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


#include "tinyxml2.h"


class oamlBase {
private:
	std::string defsFile;
	std::string playingInfo;

	bool enableTracksInfo;
	bool verbose;
	bool debugClipping;
	bool writeAudioAtShutdown;
	bool useCompressor;
	bool updateTension;

	std::vector<oamlTrack*> musicTracks;
	std::vector<oamlTrack*> sfxTracks;
	std::vector<oamlLayerData*> layers;

	oamlTrack *curTrack;

	ByteBuffer *fullBuffer;

	int sampleRate;
	int channels;
	int bytesPerSample;
	bool floatBuffer;

	int tension;
	uint64_t tensionMs;
	float volume;
	bool pause;

	oamlFileCallbacks *fcbs;

	uint64_t timeMs;

	oamlCompressor compressor;

	oamlTracksInfo tracksInfo;

	void Clear();

	oamlRC PlayTrackId(int id);
	bool IsTrackPlayingId(int id);

	void ShowPlayingTracks();
	oamlRC ReadAudioDefs(tinyxml2::XMLElement *el, oamlTrack *track, oamlTrackInfo *tinfo);
	oamlRC ReadTrackDefs(tinyxml2::XMLElement *el);
	oamlRC ReadDefs(const char *buf, int size);
	void ReadInternalDefs(const char *filaname);

	int ReadSample(void *buffer, int index);
	void WriteSample(void *buffer, int index, int sample);

	bool IsAudioFormatSupported();

	void AddLayer(const char *layer);
	int GetLayerId(const char *layer);
	oamlLayerData *GetLayer(const char *layer);

	void UpdateTension(uint64_t ms);

public:
	oamlBase();
	~oamlBase();

	oamlRC Init(const char *defsFilename);
	oamlRC ReadDefsFile(const char *defsFilename);
	oamlRC InitString(const char *defs);
	void Shutdown();

	void SetVerbose(bool option) { verbose = option; }
	void SetDebugClipping(bool option) { debugClipping = option; }
	void SetWriteAudioAtShutdown(bool option) { writeAudioAtShutdown = option; }

	void SetAudioFormat(int audioSampleRate, int audioChannels, int audioBytesPerSample, bool audioFloatBuffer);
	void SetVolume(float vol);
	float GetVolume() const { return volume; }

	oamlRC PlayTrack(const char *name);
	oamlRC PlayTrackWithStringRandom(const char *str);
	oamlRC PlayTrackByGroupRandom(const char *group);
	oamlRC PlayTrackByGroupAndSubgroupRandom(const char *group, const char *subgroup);
	oamlRC PlaySfx(const char *name);
	oamlRC PlaySfxEx(const char *name, float vol, float pan);
	oamlRC PlaySfx2d(const char *name, int x, int y, int width, int height);

	oamlRC LoadTrack(const char *name);

	void StopPlaying();
	void Pause();
	void Resume();
	void PauseToggle();

	bool IsPaused() const { return pause; }

	bool IsTrackPlaying(const char *name);
	bool IsPlaying();

	void AddTension(int value);
	void SetTension(int value);

	void SetMainLoopCondition(int value);

	void SetCondition(int id, int value);

	void SetLayerGain(const char *layer, float gain);
	void SetLayerRandomChance(const char *layer, int randomChance);

	void MixToBuffer(void *buffer, int size);

	void Update();

	int SafeAdd(int sample1, int sample2);

	void SetFileCallbacks(oamlFileCallbacks *cbs);

	void EnableDynamicCompressor(bool enable, double thresholdDb, double ratio);

	void EnableTracksInfo(bool option);
	oamlTracksInfo *GetTracksInfo();

	const char* GetDefsFile();
	const char* GetPlayingInfo();
};

#endif /* __OAMLBASE_H__ */
