//-----------------------------------------------------------------------------
// Copyright (c) 2015-2018 Marcelo Fernandez
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

#ifndef __OAML_H__
#define __OAML_H__

#include <stddef.h>

//
// Definitions
//

#define OAML_VOLUME_MIN		0.f
#define OAML_VOLUME_MAX		1.f

#define OAML_VOLUME_DEFAULT	0.5f

//
typedef enum {
	OAML_CONDTYPE_EQUAL	= 0, // x == value
	OAML_CONDTYPE_GREATER	= 1, // x > value
	OAML_CONDTYPE_LESS	= 2, // x < value
	OAML_CONDTYPE_RANGE	= 3  // x >= value AND x <= value2
} oamlCondType;

// Condition id's 1-9 are reserved for oaml core
typedef enum {
	OAML_CONDID_TENSION	= 1,
	OAML_CONDID_MAIN_LOOP	= 2,
	OAML_CONDID_USER	= 10
} oamlCondId;

// Return codes
typedef enum {
	OAML_OK			= 0,
	OAML_ERROR		= -1,
	OAML_NOT_FOUND		= -2
} oamlRC;

typedef struct {
	void*  (*open)  (const char *filename);
	size_t (*read)  (void *ptr, size_t size, size_t nitems, void *fd);
	int    (*seek)  (void *fd, long offset, int whence);
	long   (*tell)  (void *fd);
	int    (*close) (void *fd);
} oamlFileCallbacks;


#ifndef __cplusplus

#ifdef _MSC_VER
typedef int bool;
#define false 0
#define true 1
#else
#include <stdbool.h>
#endif

const char* oamlGetVersion();
oamlRC oamlInitAudioDevice(int sampleRate, int channels);
oamlRC oamlInit(const char *defsFilename);
oamlRC oamlReadDefsFile(const char *defsFilename);
oamlRC oamlInitString(const char *defs);
void oamlSetAudioFormat(int sampleRate, int channels, int bytesPerSample, bool floatBuffer);
oamlRC oamlPlayTrack(const char *name);
oamlRC oamlPlayTrackWithStringRandom(const char *str);
oamlRC oamlPlayTrackByGroupRandom(const char *group);
oamlRC oamlPlayTrackByGroupAndSubgroupRandom(const char *group, const char *subgroup);
oamlRC oamlPlaySfx(const char *name);
oamlRC oamlPlaySfxEx(const char *name, float vol, float pan);
oamlRC oamlPlaySfx2d(const char *name, int x, int y, int width, int height);
bool oamlIsTrackPlaying(const char *name);
bool oamlIsPlaying();
void oamlStopPlaying();
void oamlPause();
void oamlResume();
void oamlPauseToggle();
bool oamlIsPaused();
void oamlMixToBuffer(void *buffer, int size);
void oamlSetCondition(int id, int value);
void oamlSetVolume(float vol);
float oamlGetVolume();
void oamlAddTension(int value);
void oamlSetTension(int value);
void oamlSetMainLoopCondition(int value);
void oamlUpdate();
void SetDebugClipping(bool option);
void SetWriteAudioAtShutdown(bool option);
void oamlSetFileCallbacks(oamlFileCallbacks *cbs);
void oamlEnableDynamicCompressor(bool enable, double threshold, double ratio);
const char* oamlGetDefsFile();
const char* oamlGetPlayingInfo();
void oamlShutdown();

#else

#include <algorithm>
#include <string>
#include <vector>

typedef struct {
	std::string filename;
	std::string layer;
	int randomChance;
} oamlAudioFileInfo;

typedef struct {
	std::string name;
	int type;
	float volume;
	float bpm;
	int beatsPerBar;
	int bars;
	int minMovementBars;
	int randomChance;
	int playOrder;
	int fadeIn;
	int fadeOut;
	int xfadeIn;
	int xfadeOut;
	int condId;
	int condType;
	int condValue;
	int condValue2;
	std::vector<oamlAudioFileInfo> files;
} oamlAudioInfo;

typedef struct {
	std::string name;
	std::vector<std::string> groups;
	std::vector<std::string> subgroups;
	bool musicTrack;
	bool sfxTrack;
	int fadeIn;
	int fadeOut;
	int xfadeIn;
	int xfadeOut;
	float volume;
	std::vector<oamlAudioInfo> audios;
} oamlTrackInfo;

typedef struct {
	float bpm;
	int beatsPerBar;
	std::vector<oamlTrackInfo> tracks;
} oamlTracksInfo;


//
// Internal declarations
//

class oamlBase;
class oamlStudioApi;

//
// Main class
//

#if defined(oaml_shared_EXPORTS) || defined(OAML_EXPORTS)
class DLLEXPORT oamlApi {
#else
class oamlApi {
#endif
private:
	oamlBase *oaml;
	oamlStudioApi *oamlStudio;

public:
	oamlApi();
	~oamlApi();

	const char* GetVersion();

	/** Initilize the Open Adaptive Music Library with the path to 'oaml.defs'
	 *  @return returns OAML_OK on success
	 */
	oamlRC Init(const char *defsFilename);

	/** Read Open Adaptive Music Library definition file
	 *  @return returns OAML_OK on success
	 */
	oamlRC ReadDefsFile(const char *defsFilename);

	/** Initilize the Open Adaptive Music Library with xml definitions on value defs
	 *  @return returns OAML_OK on success
	 */
	oamlRC InitString(const char *defs);

	/** Initialize the audio device through RtAudio
	 *  @return returns OAML_OK on success
	 */
	oamlRC InitAudioDevice(int sampleRate = 44100, int channels = 2);

	/** Shutdown the library */
	void Shutdown();

	/** Set the audio format to be used by MixToBuffer */
	void SetAudioFormat(int sampleRate, int channels, int bytesPerSample, bool floatBuffer = false);
	void SetVolume(float vol);
	float GetVolume();

	/** Play a music track by name (recommended) or id
	 *  @return returns OAML_OK on success
	 */
	oamlRC PlayTrack(const char *name);

	/** Play a random music track that contains str in the name
	 *  @return returns OAML_OK on success
	 */
	oamlRC PlayTrackWithStringRandom(const char *str);

	/** Play a random music track that belongs to a certain group
	 *  @return returns OAML_OK on success
	 */
	oamlRC PlayTrackByGroupRandom(const char *group);

	/** Play a random music track that belongs to a certain group and subgroup
	 *  @return returns OAML_OK on success
	 */
	oamlRC PlayTrackByGroupAndSubgroupRandom(const char *group, const char *subgroup);

	/** Play a sound fx
	 *  @return returns OAML_OK on success
	 */
	oamlRC PlaySfx(const char *name);
	oamlRC PlaySfxEx(const char *name, float vol, float pan);
	oamlRC PlaySfx2d(const char *name, int x, int y, int width, int height);

	/** Load a track into memory cache (blocking)
	 */
	oamlRC LoadTrack(const char *name);

	/** Load a track into memory cache (non-blocking)
	 *  @return returns 0.f to 1.f or -1.f on error
	 */
	float LoadTrackProgress(const char *name);

	/** Stop playing any track currently playing */
	void StopPlaying();

	/** Pause playback */
	void Pause();

	/** Resume playback */
	void Resume();

	/** Pause/resume playback */
	void PauseToggle();

	/** Are we in pause state? */
	bool IsPaused();

	/** Check if a track is playing */
	bool IsTrackPlaying(const char *name);

	/** Check if any track is playing */
	bool IsPlaying();

	/** Add tension that triggers different aspects of the music */
	void AddTension(int value);

	/** Set tension that triggers different aspects of the music */
	void SetTension(int value);

	/** Get the current tension value */
	int GetTension();

	/** Sets a condition that affects the main loop */
	void SetMainLoopCondition(int value);

	/** Set a condition */
	void SetCondition(int id, int value);

	/** Clears current conditions */
	void ClearConditions();

	/** Set gain (0.f - 1.f) of a layer */
	void SetLayerGain(const char *layer, float gain);

	/** Get gain (0.f - 1.f) of a layer */
	float GetLayerGain(const char *layer);

	/** Set random chance (0 - 100) of a layer */
	void SetLayerRandomChance(const char *layer, int rhandomChance);

	/** Get random chance (0 - 100) of a layer */
	int GetLayerRandomChance(const char *layer);

	/** Main function to call form the internal game audio manager */
	void MixToBuffer(void *buffer, int size);

	/** Update */
	void Update();

	/** Debugging functions */
	void SetDebugClipping(bool option);
	void SetWriteAudioAtShutdown(bool option);

	/** Enable dynamic compressor for music */
	void EnableDynamicCompressor(bool enable = true, double thresholdDb = -3, double ratio = 4.0);

	/** Set file handling callbacks */
	void SetFileCallbacks(oamlFileCallbacks *cbs);

	/** Returns the 'oaml.defs' filename that was used for initialization */
	const char* GetDefsFile();

	/** Returns a simple text showing the track and audios being played */
	const char* GetPlayingInfo();

	/** Returns a pointer to the tracks information */
	oamlTracksInfo *GetTracksInfo();

	/** Returns the current playing state as a string, this can be used to resume the exact music state with LoadState */
	std::string SaveState();

	/** Resume the exact music state that was saved using SaveState */
	void LoadState(std::string state);

	/** Returns a pointer to the oamlStudioApi class */
	oamlStudioApi *GetStudioApi();
};


//
// oamlStudioApi, this api is used by the oamlStudio app
//

#if defined(oaml_shared_EXPORTS) || defined(OAML_EXPORTS)
class DLLEXPORT oamlStudioApi {
#else
class oamlStudioApi {
#endif
private:
	oamlBase *oaml;

public:
	oamlStudioApi(oamlBase *_oaml);
	~oamlStudioApi();

	void ProjectNew();
	void ProjectSetBPM(float bpm);
	void ProjectSetBeatsPerBar(int beatsPerBar);

	float ProjectGetBPM();
	int ProjectGetBeatsPerBar();

	oamlRC TrackNew(std::string name, bool sfxTrack = false);
	oamlRC TrackRemove(std::string name);
	void TrackRename(std::string name, std::string newName);
	void TrackSetVolume(std::string name, float volume);
	void TrackSetFadeIn(std::string name, int fadeIn);
	void TrackSetFadeOut(std::string name, int fadeOut);
	void TrackSetXFadeIn(std::string name, int xFadeIn);
	void TrackSetXFadeOut(std::string name, int xFadeOut);

	bool TrackExists(std::string name);
	bool TrackIsSfxTrack(std::string name);
	bool TrackIsMusicTrack(std::string name);
	void TrackGetAudioList(std::string name, std::vector<std::string>& list);
	float TrackGetVolume(std::string name);
	int TrackGetFadeIn(std::string name);
	int TrackGetFadeOut(std::string name);
	int TrackGetXFadeIn(std::string name);
	int TrackGetXFadeOut(std::string name);

	oamlRC AudioNew(std::string trackName, std::string audioName, int type);
	oamlRC AudioRemove(std::string trackName, std::string audioName);
	void AudioAddAudioFile(std::string trackName, std::string audioName, std::string filename);
	void AudioSetName(std::string trackName, std::string audioName, std::string name);
	void AudioSetVolume(std::string trackName, std::string audioName, float volume);
	void AudioSetBPM(std::string trackName, std::string audioName, float bpm);
	void AudioSetBeatsPerBar(std::string trackName, std::string audioName, int beatsPerBar);
	void AudioSetBars(std::string trackName, std::string audioName, int bars);
	void AudioSetMinMovementBars(std::string trackName, std::string audioName, int minMovementBars);
	void AudioSetRandomChance(std::string trackName, std::string audioName, int randomChance);
	void AudioSetFadeIn(std::string trackName, std::string audioName, int fadeIn);
	void AudioSetFadeOut(std::string trackName, std::string audioName, int fadeOut);
	void AudioSetXFadeIn(std::string trackName, std::string audioName, int xFadeIn);
	void AudioSetXFadeOut(std::string trackName, std::string audioName, int xFadeOut);
	void AudioSetCondId(std::string trackName, std::string audioName, int condId);
	void AudioSetCondType(std::string trackName, std::string audioName, int condType);
	void AudioSetCondValue(std::string trackName, std::string audioName, int condValue);
	void AudioSetCondValue2(std::string trackName, std::string audioName, int condValue2);

	bool AudioExists(std::string trackName, std::string audioName);
	void AudioGetAudioFileList(std::string trackName, std::string audioName, std::vector<std::string>& list);
	int AudioGetType(std::string trackName, std::string audioName);
	float AudioGetVolume(std::string trackName, std::string audioName);
	float AudioGetBPM(std::string trackName, std::string audioName);
	int AudioGetBeatsPerBar(std::string trackName, std::string audioName);
	int AudioGetBars(std::string trackName, std::string audioName);
	int AudioGetMinMovementBars(std::string trackName, std::string audioName);
	int AudioGetRandomChance(std::string trackName, std::string audioName);
	int AudioGetFadeIn(std::string trackName, std::string audioName);
	int AudioGetFadeOut(std::string trackName, std::string audioName);
	int AudioGetXFadeIn(std::string trackName, std::string audioName);
	int AudioGetXFadeOut(std::string trackName, std::string audioName);
	int AudioGetCondId(std::string trackName, std::string audioName);
	int AudioGetCondType(std::string trackName, std::string audioName);
	int AudioGetCondValue(std::string trackName, std::string audioName);
	int AudioGetCondValue2(std::string trackName, std::string audioName);

	void AudioFileRemove(std::string trackName, std::string audioName, std::string filename);
	void AudioFileSetLayer(std::string trackName, std::string audioName, std::string filename, std::string layer);
	void AudioFileSetRandomChance(std::string trackName, std::string audioName, std::string filename, int randomChance);

	std::string AudioFileGetLayer(std::string trackName, std::string audioName, std::string filename);
	int AudioFileGetRandomChance(std::string trackName, std::string audioName, std::string filename);

	oamlRC LayerNew(std::string name);
	void LayerList(std::vector<std::string>& list);
	void LayerRename(std::string layerName, std::string name);
	int LayerGetId(std::string layerName);
	int LayerGetRandomChance(std::string layerName);
	float LayerGetGain(std::string layerName);
};

#endif

#endif /* __OAML_H__ */
