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

void oamlApi::SetMeasureDecibels(bool option) {
	oaml->SetMeasureDecibels(option);
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

void oamlApi::SetVolume(int vol) {
	oaml->SetVolume(vol);
}

int oamlApi::GetVolume() {
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

void oamlApi::Shutdown() {
	oaml->Shutdown();
}
