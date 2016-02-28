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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <oaml.h>

static oamlApi oaml;

extern "C" {

int oamlInit(const char *defsFilename) {
	return oaml.Init(defsFilename);
}

int oamlInitString(const char *defs) {
	return oaml.InitString(defs);
}

void oamlSetAudioFormat(int sampleRate, int channels, int bytesPerSample, bool floatBuffer) {
	oaml.SetAudioFormat(sampleRate, channels, bytesPerSample, floatBuffer);
}

int oamlPlayTrack(const char *name) {
	return oaml.PlayTrack(name);
}

int oamlPlayTrackWithStringRandom(const char *str) {
	return oaml.PlayTrackWithStringRandom(str);
}

int oamlPlaySfx(const char *name) {
	return oaml.PlaySfx(name);
}

bool oamlIsTrackPlaying(const char *name) {
	return oaml.IsTrackPlaying(name);
}

bool oamlIsPlaying() {
	return oaml.IsPlaying();
}

void oamlStopPlaying() {
	oaml.StopPlaying();
}

void oamlPause() {
	oaml.Pause();
}

void oamlResume() {
	oaml.Resume();
}

void oamlPauseToggle() {
	oaml.PauseToggle();
}

bool oamlIsPaused() {
	return oaml.IsPaused();
}

void oamlMixToBuffer(void *buffer, int size) {
	oaml.MixToBuffer(buffer, size);
}

void oamlSetCondition(int id, int value) {
	oaml.SetCondition(id, value);
}

void oamlSetVolume(float vol) {
	oaml.SetVolume(vol);
}

float oamlGetVolume() {
	return oaml.GetVolume();
}

void oamlAddTension(int value) {
	oaml.AddTension(value);
}

void oamlSetMainLoopCondition(int value) {
	oaml.SetMainLoopCondition(value);
}

void oamlUpdate() {
	oaml.Update();
}

void oamlSetDebugClipping(bool option) {
	oaml.SetDebugClipping(option);
}

void oamlSetWriteAudioAtShutdown(bool option) {
	oaml.SetWriteAudioAtShutdown(option);
}

void oamlSetFileCallbacks(oamlFileCallbacks *cbs) {
	oaml.SetFileCallbacks(cbs);
}

void oamlEnableDynamicCompressor(bool enable, double threshold, double ratio) {
	oaml.EnableDynamicCompressor(enable, threshold, ratio);
}

const char* oamlGetDefsFile() {
	return oaml.GetDefsFile();
}

const char* oamlGetPlayingInfo() {
	return oaml.GetPlayingInfo();
}

void oamlShutdown() {
	oaml.Shutdown();
}

}
