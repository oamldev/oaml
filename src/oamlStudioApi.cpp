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
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "oamlCommon.h"


oamlStudioApi::oamlStudioApi(oamlBase *_oaml) {
	oaml = _oaml;
}

oamlStudioApi::~oamlStudioApi() {
}

void oamlStudioApi::ProjectNew() {
	oaml->ProjectNew();
}

void oamlStudioApi::ProjectSetBPM(float bpm) {
	oaml->ProjectSetBPM(bpm);
}

void oamlStudioApi::ProjectSetBeatsPerBar(int beatsPerBar) {
	oaml->ProjectSetBeatsPerBar(beatsPerBar);
}

float oamlStudioApi::ProjectGetBPM() {
	return oaml->ProjectGetBPM();
}

int oamlStudioApi::ProjectGetBeatsPerBar() {
	return oaml->ProjectGetBeatsPerBar();
}

oamlRC oamlStudioApi::TrackNew(std::string name, bool sfxTrack) {
	return oaml->TrackNew(name, sfxTrack);
}

oamlRC oamlStudioApi::TrackRemove(std::string name) {
	return oaml->TrackRemove(name);
}

void oamlStudioApi::TrackRename(std::string name, std::string newName) {
	oaml->TrackRename(name, newName);
}

void oamlStudioApi::TrackSetVolume(std::string name, float volume) {
	oaml->TrackSetVolume(name, volume);
}

void oamlStudioApi::TrackSetFadeIn(std::string name, int fadeIn) {
	oaml->TrackSetFadeIn(name, fadeIn);
}

void oamlStudioApi::TrackSetFadeOut(std::string name, int fadeOut) {
	oaml->TrackSetFadeOut(name, fadeOut);
}

void oamlStudioApi::TrackSetXFadeIn(std::string name, int xFadeIn) {
	oaml->TrackSetXFadeIn(name, xFadeIn);
}

void oamlStudioApi::TrackSetXFadeOut(std::string name, int xFadeOut) {
	oaml->TrackSetXFadeOut(name, xFadeOut);
}

bool oamlStudioApi::TrackExists(std::string name) {
	return oaml->TrackExists(name);
}

bool oamlStudioApi::TrackIsSfxTrack(std::string name) {
	return oaml->TrackIsSfxTrack(name);
}

bool oamlStudioApi::TrackIsMusicTrack(std::string name) {
	return oaml->TrackIsMusicTrack(name);
}

void oamlStudioApi::TrackGetAudioList(std::string name, std::vector<std::string>& list) {
	oaml->TrackGetAudioList(name, list);
}

float oamlStudioApi::TrackGetVolume(std::string name) {
	return oaml->TrackGetVolume(name);
}

int oamlStudioApi::TrackGetFadeIn(std::string name) {
	return oaml->TrackGetFadeIn(name);
}

int oamlStudioApi::TrackGetFadeOut(std::string name) {
	return oaml->TrackGetFadeOut(name);
}

int oamlStudioApi::TrackGetXFadeIn(std::string name) {
	return oaml->TrackGetXFadeIn(name);
}

int oamlStudioApi::TrackGetXFadeOut(std::string name) {
	return oaml->TrackGetXFadeOut(name);
}

oamlRC oamlStudioApi::AudioNew(std::string trackName, std::string audioName, int type) {
	return oaml->AudioNew(trackName, audioName, type);
}

oamlRC oamlStudioApi::AudioRemove(std::string trackName, std::string audioName) {
	return oaml->AudioRemove(trackName, audioName);
}

void oamlStudioApi::AudioAddAudioFile(std::string trackName, std::string audioName, std::string filename) {
	oaml->AudioAddAudioFile(trackName, audioName, filename);
}

void oamlStudioApi::AudioSetName(std::string trackName, std::string audioName, std::string name) {
	oaml->AudioSetName(trackName, audioName, name);
}

void oamlStudioApi::AudioSetVolume(std::string trackName, std::string audioName, float volume) {
	oaml->AudioSetVolume(trackName, audioName, volume);
}

void oamlStudioApi::AudioSetBPM(std::string trackName, std::string audioName, float bpm) {
	oaml->AudioSetBPM(trackName, audioName, bpm);
}

void oamlStudioApi::AudioSetBeatsPerBar(std::string trackName, std::string audioName, int beatsPerBar) {
	oaml->AudioSetBeatsPerBar(trackName, audioName, beatsPerBar);
}

void oamlStudioApi::AudioSetBars(std::string trackName, std::string audioName, int bars) {
	oaml->AudioSetBars(trackName, audioName, bars);
}

void oamlStudioApi::AudioSetMinMovementBars(std::string trackName, std::string audioName, int minMovementBars) {
	oaml->AudioSetMinMovementBars(trackName, audioName, minMovementBars);
}

void oamlStudioApi::AudioSetRandomChance(std::string trackName, std::string audioName, int randomChance) {
	oaml->AudioSetRandomChance(trackName, audioName, randomChance);
}

void oamlStudioApi::AudioSetFadeIn(std::string trackName, std::string audioName, int fadeIn) {
	oaml->AudioSetFadeIn(trackName, audioName, fadeIn);
}

void oamlStudioApi::AudioSetFadeOut(std::string trackName, std::string audioName, int fadeOut) {
	oaml->AudioSetFadeOut(trackName, audioName, fadeOut);
}

void oamlStudioApi::AudioSetXFadeIn(std::string trackName, std::string audioName, int xFadeIn) {
	oaml->AudioSetXFadeIn(trackName, audioName, xFadeIn);
}

void oamlStudioApi::AudioSetXFadeOut(std::string trackName, std::string audioName, int xFadeOut) {
	oaml->AudioSetXFadeOut(trackName, audioName, xFadeOut);
}

void oamlStudioApi::AudioSetCondId(std::string trackName, std::string audioName, int condId) {
	oaml->AudioSetCondId(trackName, audioName, condId);
}

void oamlStudioApi::AudioSetCondType(std::string trackName, std::string audioName, int condType) {
	oaml->AudioSetCondType(trackName, audioName, condType);
}

void oamlStudioApi::AudioSetCondValue(std::string trackName, std::string audioName, int condValue) {
	oaml->AudioSetCondValue(trackName, audioName, condValue);
}

void oamlStudioApi::AudioSetCondValue2(std::string trackName, std::string audioName, int condValue2) {
	oaml->AudioSetCondValue2(trackName, audioName, condValue2);
}

bool oamlStudioApi::AudioExists(std::string trackName, std::string audioName) {
	return oaml->AudioExists(trackName, audioName);
}

int oamlStudioApi::AudioGetType(std::string trackName, std::string audioName) {
	return oaml->AudioGetType(trackName, audioName);
}

void oamlStudioApi::AudioGetAudioFileList(std::string trackName, std::string audioName, std::vector<std::string>& list) {
	oaml->AudioGetAudioFileList(trackName, audioName, list);
}

float oamlStudioApi::AudioGetVolume(std::string trackName, std::string audioName) {
	return oaml->AudioGetVolume(trackName, audioName);
}

float oamlStudioApi::AudioGetBPM(std::string trackName, std::string audioName) {
	return oaml->AudioGetBPM(trackName, audioName);
}

int oamlStudioApi::AudioGetBeatsPerBar(std::string trackName, std::string audioName) {
	return oaml->AudioGetBeatsPerBar(trackName, audioName);
}

int oamlStudioApi::AudioGetBars(std::string trackName, std::string audioName) {
	return oaml->AudioGetBars(trackName, audioName);
}

int oamlStudioApi::AudioGetMinMovementBars(std::string trackName, std::string audioName) {
	return oaml->AudioGetMinMovementBars(trackName, audioName);
}

int oamlStudioApi::AudioGetRandomChance(std::string trackName, std::string audioName) {
	return oaml->AudioGetRandomChance(trackName, audioName);
}

int oamlStudioApi::AudioGetFadeIn(std::string trackName, std::string audioName) {
	return oaml->AudioGetFadeIn(trackName, audioName);
}

int oamlStudioApi::AudioGetFadeOut(std::string trackName, std::string audioName) {
	return oaml->AudioGetFadeOut(trackName, audioName);
}

int oamlStudioApi::AudioGetXFadeIn(std::string trackName, std::string audioName) {
	return oaml->AudioGetXFadeIn(trackName, audioName);
}

int oamlStudioApi::AudioGetXFadeOut(std::string trackName, std::string audioName) {
	return oaml->AudioGetXFadeOut(trackName, audioName);
}

int oamlStudioApi::AudioGetCondId(std::string trackName, std::string audioName) {
	return oaml->AudioGetCondId(trackName, audioName);
}

int oamlStudioApi::AudioGetCondType(std::string trackName, std::string audioName) {
	return oaml->AudioGetCondType(trackName, audioName);
}

int oamlStudioApi::AudioGetCondValue(std::string trackName, std::string audioName) {
	return oaml->AudioGetCondValue(trackName, audioName);
}

int oamlStudioApi::AudioGetCondValue2(std::string trackName, std::string audioName) {
	return oaml->AudioGetCondValue2(trackName, audioName);
}

void oamlStudioApi::AudioFileRemove(std::string trackName, std::string audioName, std::string filename) {
	oaml->AudioFileRemove(trackName, audioName, filename);
}

void oamlStudioApi::AudioFileSetLayer(std::string trackName, std::string audioName, std::string filename, std::string layer) {
	oaml->AudioFileSetLayer(trackName, audioName, filename, layer);
}

void oamlStudioApi::AudioFileSetRandomChance(std::string trackName, std::string audioName, std::string filename, int randomChance) {
	oaml->AudioFileSetRandomChance(trackName, audioName, filename, randomChance);
}

std::string oamlStudioApi::AudioFileGetLayer(std::string trackName, std::string audioName, std::string filename) {
	return oaml->AudioFileGetLayer(trackName, audioName, filename);
}

int oamlStudioApi::AudioFileGetRandomChance(std::string trackName, std::string audioName, std::string filename) {
	return oaml->AudioFileGetRandomChance(trackName, audioName, filename);
}

oamlRC oamlStudioApi::LayerNew(std::string name) {
	return oaml->LayerNew(name);
}

void oamlStudioApi::LayerList(std::vector<std::string>& list) {
	oaml->LayerList(list);
}

void oamlStudioApi::LayerRename(std::string layerName, std::string name) {
	return oaml->LayerRename(layerName, name);
}

int oamlStudioApi::LayerGetId(std::string layerName) {
	return oaml->LayerGetId(layerName);
}

int oamlStudioApi::LayerGetRandomChance(std::string layerName) {
	return oaml->LayerGetRandomChance(layerName);
}

float oamlStudioApi::LayerGetGain(std::string layerName) {
	return oaml->LayerGetGain(layerName);
}

