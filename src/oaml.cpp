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

int oamlApi::Init(const char *pathToMusic) {
	return oaml->Init(pathToMusic);
}

void oamlApi::SetMeasureDecibels(bool option) {
	oaml->SetMeasureDecibels(option);
}

void oamlApi::SetAudioFormat(int freq, int channels, int bytesPerSample) {
	oaml->SetAudioFormat(freq, channels, bytesPerSample);
}

int oamlApi::PlayTrackId(int id) {
	return oaml->PlayTrackId(id);
}

int oamlApi::PlayTrack(const char *name) {
	return oaml->PlayTrack(name);
}

bool oamlApi::IsTrackPlaying(const char *name) {
	return oaml->IsTrackPlaying(name);
}

bool oamlApi::IsTrackPlayingId(int id) {
	return oaml->IsTrackPlayingId(id);
}

bool oamlApi::IsPlaying() {
	return oaml->IsPlaying();
}

void oamlApi::StopPlaying() {
	oaml->StopPlaying();
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

void oamlApi::Update() {
	oaml->Update();
}

void oamlApi::Shutdown() {
	oaml->Shutdown();
}
