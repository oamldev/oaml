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

oamlRC oamlStudioApi::TrackNew(std::string name, bool sfxTrack) {
	return oaml->TrackNew(name, sfxTrack);
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

oamlRC oamlStudioApi::AudioNew(std::string trackName, std::string filename, int type) {
	return oaml->AudioNew(trackName, filename, type);
}

oamlRC oamlStudioApi::AudioRemove(std::string trackName, std::string filename) {
	return oaml->AudioRemove(trackName, filename);
}

void oamlStudioApi::AudioSetVolume(std::string trackName, std::string filename, float volume) {
	oaml->AudioSetVolume(trackName, filename, volume);
}

void oamlStudioApi::AudioSetBPM(std::string trackName, std::string filename, float bpm) {
	oaml->AudioSetBPM(trackName, filename, bpm);
}

void oamlStudioApi::AudioSetBeatsPerBar(std::string trackName, std::string filename, int beatsPerBar) {
	oaml->AudioSetBeatsPerBar(trackName, filename, beatsPerBar);
}

void oamlStudioApi::AudioSetBars(std::string trackName, std::string filename, int bars) {
	oaml->AudioSetBars(trackName, filename, bars);
}

void oamlStudioApi::AudioSetMinMovementBars(std::string trackName, std::string filename, int minMovementBars) {
	oaml->AudioSetMinMovementBars(trackName, filename, minMovementBars);
}

void oamlStudioApi::AudioSetRandomChance(std::string trackName, std::string filename, int randomChance) {
	oaml->AudioSetRandomChance(trackName, filename, randomChance);
}

void oamlStudioApi::AudioSetFadeIn(std::string trackName, std::string filename, int fadeIn) {
	oaml->AudioSetFadeIn(trackName, filename, fadeIn);
}

void oamlStudioApi::AudioSetFadeOut(std::string trackName, std::string filename, int fadeOut) {
	oaml->AudioSetFadeOut(trackName, filename, fadeOut);
}

void oamlStudioApi::AudioSetXFadeIn(std::string trackName, std::string filename, int xFadeIn) {
	oaml->AudioSetXFadeIn(trackName, filename, xFadeIn);
}

void oamlStudioApi::AudioSetXFadeOut(std::string trackName, std::string filename, int xFadeOut) {
	oaml->AudioSetXFadeOut(trackName, filename, xFadeOut);
}

void oamlStudioApi::AudioSetCondId(std::string trackName, std::string filename, int condId) {
	oaml->AudioSetCondId(trackName, filename, condId);
}

void oamlStudioApi::AudioSetCondType(std::string trackName, std::string filename, int condType) {
	oaml->AudioSetCondType(trackName, filename, condType);
}

void oamlStudioApi::AudioSetCondValue(std::string trackName, std::string filename, int condValue) {
	oaml->AudioSetCondValue(trackName, filename, condValue);
}

void oamlStudioApi::AudioSetCondValue2(std::string trackName, std::string filename, int condValue2) {
	oaml->AudioSetCondValue2(trackName, filename, condValue2);
}

