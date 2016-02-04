#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "oamlCommon.h"
#include "oamlUnityPlugin.h"
#include "AudioPluginInterface.h"

oamlApi *oaml = NULL;
FILE *log = NULL;

int oamlInit(const char *defsFilename) {
	if (oaml == NULL) {
		oaml = new oamlApi;
	}

	return oaml->Init(defsFilename);
}

int oamlInitString(const char *defs) {
	if (oaml == NULL) {
		oaml = new oamlApi;
	}

	return oaml->InitString(defs);
}

void oamlSetMeasureDecibels(bool option) {
	oaml->SetMeasureDecibels(option);
}

void oamlSetAudioFormat(int freq, int channels, int bytesPerSample) {
	oaml->SetAudioFormat(freq, channels, bytesPerSample);
}

int oamlPlayTrack(const char *name) {
	return oaml->PlayTrack(name);
}

int oamlPlayTrackWithStringRandom(const char *str) {
	return oaml->PlayTrackWithStringRandom(str);
}

bool oamlIsTrackPlaying(const char *name) {
	return oaml->IsTrackPlaying(name);
}

bool oamlIsTrackPlayingId(int id) {
	return oaml->IsTrackPlayingId(id);
}

bool oamlIsPlaying() {
	return oaml->IsPlaying();
}

void oamlStopPlaying() {
	oaml->StopPlaying();
}

void oamlPause() {
	oaml->Pause();
}

void oamlResume() {
	oaml->Resume();
}

void oamlPauseToggle() {
	oaml->PauseToggle();
}

bool oamlIsPaused() {
	return oaml->IsPaused();
}

void oamlMixToBuffer(void *buffer, int size) {
	oaml->MixToBuffer(buffer, size);
}

void oamlSetCondition(int id, int value) {
	oaml->SetCondition(id, value);
}

void oamlSetVolume(int vol) {
	oaml->SetVolume(vol);
}

int oamlGetVolume() {
	return oaml->GetVolume();
}

void oamlAddTension(int value) {
	oaml->AddTension(value);
}

void oamlSetMainLoopCondition(int value) {
	oaml->SetMainLoopCondition(value);
}

void oamlUpdate() {
	oaml->Update();
}

void oamlSetFileCallbacks(oamlFileCallbacks *cbs) {
	oaml->SetFileCallbacks(cbs);
}

void oamlEnableDynamicCompressor(bool enable, double threshold, double ratio) {
	oaml->EnableDynamicCompressor(enable, threshold, ratio);
}

oamlTracksInfo* oamlGetTracksInfo() {
	return oaml->GetTracksInfo();
}

const char* oamlGetDefsFile() {
	return oaml->GetDefsFile();
}

void oamlShutdown() {
	oaml->Shutdown();

	delete oaml;
	oaml = NULL;
}

UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK CreateCallback(UnityAudioEffectState* state) {
	return UNITY_AUDIODSP_OK;
}

UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK ReleaseCallback(UnityAudioEffectState* state) {
	return UNITY_AUDIODSP_OK;
}

UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK ProcessCallback(UnityAudioEffectState* state, float* inbuffer, float* outbuffer, unsigned int length, int inchannels, int outchannels) {
	memcpy(outbuffer, inbuffer, sizeof(float) * length * inchannels);

	if (oaml) {
		oaml->SetAudioFormat(state->samplerate, inchannels, 4, true);
		oaml->MixToBuffer(outbuffer, length * inchannels);
	}

	return UNITY_AUDIODSP_OK;
}

UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK SetFloatParameterCallback(UnityAudioEffectState* state, int index, float value) {
	return UNITY_AUDIODSP_OK;
}

UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK GetFloatParameterCallback(UnityAudioEffectState* state, int index, float* value, char *valuestr) {
	return UNITY_AUDIODSP_OK;
}

int UNITY_AUDIODSP_CALLBACK GetFloatBufferCallback(UnityAudioEffectState* state, const char* name, float* buffer, int numsamples) {
	return UNITY_AUDIODSP_OK;
}

extern "C" UNITY_AUDIODSP_EXPORT_API int UnityGetAudioEffectDefinitions (UnityAudioEffectDefinition*** definitionptr) {
	static UnityAudioEffectDefinition definition;
	static UnityAudioEffectDefinition* definitionp[1];

	memset(&definition, 0, sizeof(definition));
	strcpy(definition.name, "AudioPluginOAML");
	definition.structsize = sizeof(UnityAudioEffectDefinition);
	definition.paramstructsize = sizeof(UnityAudioParameterDefinition);
	definition.apiversion = UNITY_AUDIO_PLUGIN_API_VERSION;
	definition.pluginversion = 0x010000;
	definition.create = CreateCallback;
	definition.release = ReleaseCallback;
	definition.process = ProcessCallback;
	definition.setfloatparameter = SetFloatParameterCallback;
	definition.getfloatparameter = GetFloatParameterCallback;
	definition.getfloatbuffer = GetFloatBufferCallback;

	definitionp[0] = &definition;
	*definitionptr = definitionp;
	return 1;
}
