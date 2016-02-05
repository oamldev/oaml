#ifndef __OAMLUNITYPLUGIN_H__
#define __OAMLUNITYPLUGIN_H__

#ifdef _MSC_VER
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

extern "C" {
	DLLEXPORT int oamlInit(const char *defsFilename);
	DLLEXPORT int oamlInitString(const char *defs);
	DLLEXPORT void oamlShutdown();
	DLLEXPORT void oamlSetAudioFormat(int freq, int channels, int bytesPerSample);
	DLLEXPORT void oamlSetVolume(int vol);
	DLLEXPORT int oamlGetVolume();
	DLLEXPORT int oamlPlayTrack(const char *name);
	DLLEXPORT int oamlPlayTrackWithStringRandom(const char *str);
	DLLEXPORT void oamlStopPlaying();
	DLLEXPORT void oamlPause();
	DLLEXPORT void oamlResume();
	DLLEXPORT void oamlPauseToggle();
	DLLEXPORT bool oamlIsPaused();
	DLLEXPORT bool oamlIsTrackPlaying(const char *name);
	DLLEXPORT bool oamlIsPlaying();
	DLLEXPORT oamlTracksInfo *oamlGetTracksInfo();
	DLLEXPORT void oamlAddTension(int value);
	DLLEXPORT void oamlSetMainLoopCondition(int value);
	DLLEXPORT void oamlSetCondition(int id, int value);
	DLLEXPORT void oamlMixToBuffer(void *buffer, int size);
	DLLEXPORT void oamlUpdate();
	DLLEXPORT void oamlSetDebugClipping(bool option);
	DLLEXPORT void oamlSetMeasureDecibels(bool option);
	DLLEXPORT void oamlSetWriteAudioAtShutdown(bool option);
	DLLEXPORT void oamlEnableDynamicCompressor(bool enable = true, double thresholdDb = -3, double ratio = 4.0);
	DLLEXPORT void oamlSetFileCallbacks(oamlFileCallbacks *cbs);
	DLLEXPORT const char* oamlGetDefsFile();
}

#endif
