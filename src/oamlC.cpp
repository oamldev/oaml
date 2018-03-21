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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "oamlCommon.h"

static oamlBase oaml;

extern "C" {

const char* oamlGetVersion() {
	return oaml.GetVersion();
}

oamlRC oamlInitAudioDevice(int sampleRate, int channels) {
	return oaml.InitAudioDevice(sampleRate, channels);
}

oamlRC oamlInit(const char *defsFilename) {
	return oaml.Init(defsFilename);
}

oamlRC oamlReadDefsFile(const char *defsFilename) {
	return oaml.ReadDefsFile(defsFilename);
}

oamlRC oamlInitString(const char *defs) {
	return oaml.InitString(defs);
}

void oamlSetAudioFormat(int sampleRate, int channels, int bytesPerSample, bool floatBuffer) {
	oaml.SetAudioFormat(sampleRate, channels, bytesPerSample, floatBuffer);
}

oamlRC oamlPlayTrack(const char *name) {
	return oaml.PlayTrack(name);
}

oamlRC oamlPlayTrackWithStringRandom(const char *str) {
	return oaml.PlayTrackWithStringRandom(str);
}

oamlRC oamlPlaySfx(const char *name) {
	return oaml.PlaySfx(name);
}

oamlRC oamlPlaySfxEx(const char *name, float vol, float pan) {
	return oaml.PlaySfxEx(name, vol, pan);
}

oamlRC oamlPlaySfx2d(const char *name, int x, int y, int width, int height) {
	return oaml.PlaySfx2d(name, x, y, width, height);
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
