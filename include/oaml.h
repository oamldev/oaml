#ifndef __OAML_H__
#define __OAML_H__

#include <string>
#include <vector>

//
// Definitions
//

#define OAML_VOLUME_MIN		0
#define OAML_VOLUME_MAX		100

#define OAML_VOLUME_DEFAULT	50

enum {
	COND_TYPE_EQUAL		= 0,
	COND_TYPE_GREATER	= 1,
	COND_TYPE_LESS		= 2,
	COND_TYPE_RANGE		= 3
};

typedef struct {
	void*  (*open)  (const char *filename);
	size_t (*read)  (void *ptr, size_t size, size_t nitems, void *fd);
	int    (*seek)  (void *fd, long offset, int whence);
	long   (*tell)  (void *fd);
	int    (*close) (void *fd);
} oamlFileCallbacks;

typedef struct {
	std::string filename;
	int type;
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
} oamlAudioInfo;

typedef struct {
	std::string name;
	int fadeIn;
	int fadeOut;
	int xfadeIn;
	int xfadeOut;
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

class oamlApi {
private:
	oamlBase *oaml;

public:
	oamlApi();
	~oamlApi();

	/** Initilize the Open Adaptive Music Library with the path to 'oaml.defs'
	 *  @return returns 0, or -1 on error
	 */
	int Init(const char *defsFilename);

	/** Initilize the Open Adaptive Music Library with xml definitions on value defs
	 *  @return returns 0, or -1 on error
	 */
	int InitString(const char *defs);

	/** Shutdown the library */
	void Shutdown();

	/** Set the audio format to be used by MixToBuffer */
	void SetAudioFormat(int sampleRate, int channels, int bytesPerSample, bool floatBuffer = false);
	void SetVolume(int vol);
	int GetVolume();

	/** Play a music track by name (recommended) or id
	 *  @return returns 0, or -1 on error
	 */
	int PlayTrack(const char *name);

	/** Play a random music track that contains str in the name
	 *  @return returns 0, or -1 on error
	 */
	int PlayTrackWithStringRandom(const char *str);

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

	/** Returns a pointer to the tracks information */
	oamlTracksInfo *GetTracksInfo();

	/** Add tension that triggers different aspects of the music */
	void AddTension(int value);

	/** Sets a condition that affects the main loop */
	void SetMainLoopCondition(int value);

	/** Set a condition */
	void SetCondition(int id, int value);

	/** Main function to call form the internal game audio manager */
	void MixToBuffer(void *buffer, int size);

	/** Update */
	void Update();

	/** Debugging functions */
	void SetDebugClipping(bool option);
	void SetMeasureDecibels(bool option);
	void SetWriteAudioAtShutdown(bool option);

	/** Enable dynamic compressor for music */
	void EnableDynamicCompressor(bool enable = true, double thresholdDb = -3, double ratio = 4.0);

	/** Set file handling callbacks */
	void SetFileCallbacks(oamlFileCallbacks *cbs);

	/** Returns the 'oaml.defs' filename that was used for initialization */
	const char* GetDefsFile();

	/** Returns a simple text showing the track and audios being played */
	const char* GetPlayingInfo();
};

#endif /* __OAML_H__ */
