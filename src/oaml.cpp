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

#include "oamlCommon.h"

oamlApi::oamlApi() {
	oaml = new oamlBase();
}

oamlApi::~oamlApi() {
	delete oaml;
}

int oamlApi::Init(const char *defsFilename) {
	return oaml->Init(defsFilename);
}

int oamlApi::InitString(const char *defs) {
	return oaml->InitString(defs);
}

void oamlApi::SetAudioFormat(int sampleRate, int channels, int bytesPerSample, bool floatBuffer) {
	oaml->SetAudioFormat(sampleRate, channels, bytesPerSample, floatBuffer);
}

int oamlApi::PlayTrack(const char *name) {
	return oaml->PlayTrack(name);
}

int oamlApi::PlayTrackWithStringRandom(const char *str) {
	return oaml->PlayTrackWithStringRandom(str);
}

int oamlApi::PlaySfx(const char *name) {
	return oaml->PlaySfx(name);
}

bool oamlApi::IsTrackPlaying(const char *name) {
	return oaml->IsTrackPlaying(name);
}

bool oamlApi::IsPlaying() {
	return oaml->IsPlaying();
}

void oamlApi::StopPlaying() {
	oaml->StopPlaying();
}

void oamlApi::Pause() {
	oaml->Pause();
}

void oamlApi::Resume() {
	oaml->Resume();
}

void oamlApi::PauseToggle() {
	oaml->PauseToggle();
}

bool oamlApi::IsPaused() {
	return oaml->IsPaused();
}

void oamlApi::MixToBuffer(void *buffer, int size) {
	oaml->MixToBuffer(buffer, size);
}

void oamlApi::SetCondition(int id, int value) {
	oaml->SetCondition(id, value);
}

void oamlApi::SetVolume(float vol) {
	oaml->SetVolume(vol);
}

float oamlApi::GetVolume() {
	return oaml->GetVolume();
}

void oamlApi::AddTension(int value) {
	oaml->AddTension(value);
}

void oamlApi::SetMainLoopCondition(int value) {
	oaml->SetMainLoopCondition(value);
}

void oamlApi::Update() {
	oaml->Update();
}

void oamlApi::SetFileCallbacks(oamlFileCallbacks *cbs) {
	oaml->SetFileCallbacks(cbs);
}

void oamlApi::EnableDynamicCompressor(bool enable, double threshold, double ratio) {
	oaml->EnableDynamicCompressor(enable, threshold, ratio);
}

oamlTracksInfo* oamlApi::GetTracksInfo() {
	return oaml->GetTracksInfo();
}

const char* oamlApi::GetDefsFile() {
	return oaml->GetDefsFile();
}

const char* oamlApi::GetPlayingInfo() {
	return oaml->GetPlayingInfo();
}

void oamlApi::Shutdown() {
	oaml->Shutdown();
}
