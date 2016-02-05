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
	if (oaml == NULL)
		return;
	oaml->SetMeasureDecibels(option);
}

void oamlSetAudioFormat(int freq, int channels, int bytesPerSample) {
	if (oaml == NULL)
		return;
	oaml->SetAudioFormat(freq, channels, bytesPerSample);
}

int oamlPlayTrack(const char *name) {
	if (oaml == NULL)
		return -1;
	return oaml->PlayTrack(name);
}

int oamlPlayTrackWithStringRandom(const char *str) {
	if (oaml == NULL)
		return -1;
	return oaml->PlayTrackWithStringRandom(str);
}

bool oamlIsTrackPlaying(const char *name) {
	if (oaml == NULL)
		return false;
	return oaml->IsTrackPlaying(name);
}

bool oamlIsPlaying() {
	if (oaml == NULL)
		return false;
	return oaml->IsPlaying();
}

void oamlStopPlaying() {
	if (oaml == NULL)
		return;
	oaml->StopPlaying();
}

void oamlPause() {
	if (oaml == NULL)
		return;
	oaml->Pause();
}

void oamlResume() {
	if (oaml == NULL)
		return;
	oaml->Resume();
}

void oamlPauseToggle() {
	if (oaml == NULL)
		return;
	oaml->PauseToggle();
}

bool oamlIsPaused() {
	if (oaml == NULL)
		return false;
	return oaml->IsPaused();
}

void oamlMixToBuffer(void *buffer, int size) {
	if (oaml == NULL)
		return;
	oaml->MixToBuffer(buffer, size);
}

void oamlSetCondition(int id, int value) {
	if (oaml == NULL)
		return;
	oaml->SetCondition(id, value);
}

void oamlSetVolume(int vol) {
	if (oaml == NULL)
		return;
	oaml->SetVolume(vol);
}

int oamlGetVolume() {
	if (oaml == NULL)
		return 0;
	return oaml->GetVolume();
}

void oamlAddTension(int value) {
	if (oaml == NULL)
		return;
	oaml->AddTension(value);
}

void oamlSetMainLoopCondition(int value) {
	if (oaml == NULL)
		return;
	oaml->SetMainLoopCondition(value);
}

void oamlUpdate() {
	if (oaml == NULL)
		return;
	oaml->Update();
}

void oamlSetFileCallbacks(oamlFileCallbacks *cbs) {
	if (oaml == NULL)
		return;
	oaml->SetFileCallbacks(cbs);
}

void oamlEnableDynamicCompressor(bool enable, double threshold, double ratio) {
	if (oaml == NULL)
		return;
	oaml->EnableDynamicCompressor(enable, threshold, ratio);
}

oamlTracksInfo* oamlGetTracksInfo() {
	if (oaml == NULL)
		return NULL;
	return oaml->GetTracksInfo();
}

const char* oamlGetDefsFile() {
	if (oaml == NULL)
		return NULL;
	return oaml->GetDefsFile();
}

const char* oamlGetPlayingInfo() {
	if (oaml == NULL)
		return NULL;
	return oaml->GetPlayingInfo();
}

void oamlShutdown() {
	if (oaml == NULL)
		return;
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
		oaml->Update();
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
