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
	DLLEXPORT const char* oamlGetPlayingInfo();
}

#endif
