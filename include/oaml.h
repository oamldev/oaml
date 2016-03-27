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
	std::string name;
	int randomChance;
} oamlLayerInfo;

typedef struct {
	int type;
	float volume;
	float bpm;
	int beatsPerBar;
	int bars;
	int minMovementBars;
	int randomChance;
	int fadeIn;
	int fadeOut;
	int xfadeIn;
	int xfadeOut;
	int condId;
	int condType;
	int condValue;
	int condValue2;
	std::vector<oamlLayerInfo> layers;
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
	std::vector<oamlTrackInfo> tracks;
} oamlTracksInfo;


//
// Internal declarations
//

class oamlBase;

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

public:
	oamlApi();
	~oamlApi();

	/** Initilize the Open Adaptive Music Library with the path to 'oaml.defs'
	 *  @return returns 0, or -1 on error
	 */
	oamlRC Init(const char *defsFilename);

	/** Read Open Adaptive Music Library definition file
	 *  @return returns 0, or -1 on error
	 */
	oamlRC ReadDefsFile(const char *defsFilename);

	/** Initilize the Open Adaptive Music Library with xml definitions on value defs
	 *  @return returns 0, or -1 on error
	 */
	oamlRC InitString(const char *defs);

	/** Shutdown the library */
	void Shutdown();

	/** Set the audio format to be used by MixToBuffer */
	void SetAudioFormat(int sampleRate, int channels, int bytesPerSample, bool floatBuffer = false);
	void SetVolume(float vol);
	float GetVolume();

	/** Play a music track by name (recommended) or id
	 *  @return returns 0, or -1 on error
	 */
	oamlRC PlayTrack(const char *name);

	/** Play a random music track that contains str in the name
	 *  @return returns 0, or -1 on error
	 */
	oamlRC PlayTrackWithStringRandom(const char *str);

	/** Play a random music track that belongs to a certain group
	 *  @return returns 0, or -1 on error
	 */
	oamlRC PlayTrackByGroupRandom(const char *group);

	/** Play a random music track that belongs to a certain group and subgroup
	 *  @return returns 0, or -1 on error
	 */
	oamlRC PlayTrackByGroupAndSubgroupRandom(const char *group, const char *subgroup);

	/** Play a sound fx
	 *  @return returns 0, or -1 on error
	 */
	oamlRC PlaySfx(const char *name);
	oamlRC PlaySfxEx(const char *name, float vol, float pan);
	oamlRC PlaySfx2d(const char *name, int x, int y, int width, int height);

	/** Load a track into memory cache */
	oamlRC LoadTrack(const char *name);

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

	/** Sets a condition that affects the main loop */
	void SetMainLoopCondition(int value);

	/** Set a condition */
	void SetCondition(int id, int value);

	/** Set gain (0.f - 1.f) of a layer */
	void SetLayerGain(const char *layer, float gain);

	/** Set random chance (0 - 100) of a layer */
	void SetLayerRandomChance(const char *layer, int rhandomChance);

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

	/** TracksInfo must be enabled before loading definitions file to store oamlTracksInfo data */
	void EnableTracksInfo(bool option);

	/** Returns a pointer to the tracks information */
	oamlTracksInfo *GetTracksInfo();
};

#endif

#endif /* __OAML_H__ */
