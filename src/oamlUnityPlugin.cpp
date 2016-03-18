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
#include "oamlUnityPlugin.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include "AudioPluginInterface.h"
#pragma GCC diagnostic pop

oamlApi oaml;

int oamlInit(const char *defsFilename) {
	return oaml.Init(defsFilename);
}

int oamlReadDefsFile(const char *defsFilename) {
	return oaml.ReadDefsFile(defsFilename);
}

int oamlInitString(const char *defs) {
	return oaml.InitString(defs);
}

void oamlSetAudioFormat(int freq, int channels, int bytesPerSample) {
	oaml.SetAudioFormat(freq, channels, bytesPerSample);
}

int oamlPlayTrack(const char *name) {
	return oaml.PlayTrack(name);
}

int oamlPlayTrackWithStringRandom(const char *str) {
	return oaml.PlayTrackWithStringRandom(str);
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

void oamlSetFileCallbacks(oamlFileCallbacks *cbs) {
	oaml.SetFileCallbacks(cbs);
}

void oamlEnableDynamicCompressor(bool enable, double threshold, double ratio) {
	oaml.EnableDynamicCompressor(enable, threshold, ratio);
}

oamlTracksInfo* oamlGetTracksInfo() {
	return oaml.GetTracksInfo();
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

UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK CreateCallback(UnityAudioEffectState*) {
	return UNITY_AUDIODSP_OK;
}

UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK ReleaseCallback(UnityAudioEffectState*) {
	return UNITY_AUDIODSP_OK;
}

UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK ProcessCallback(UnityAudioEffectState* state, float* inbuffer, float* outbuffer, unsigned int length, int inchannels, int outchannels) {
	if (inchannels != outchannels)
		return UNITY_AUDIODSP_ERR_UNSUPPORTED;

	memcpy(outbuffer, inbuffer, sizeof(float) * length * inchannels);

	oaml.SetAudioFormat(state->samplerate, inchannels, 4, true);
	oaml.MixToBuffer(outbuffer, length * inchannels);
	oaml.Update();

	return UNITY_AUDIODSP_OK;
}

UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK SetFloatParameterCallback(UnityAudioEffectState*, int, float) {
	return UNITY_AUDIODSP_OK;
}

UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK GetFloatParameterCallback(UnityAudioEffectState*, int, float*, char *) {
	return UNITY_AUDIODSP_OK;
}

int UNITY_AUDIODSP_CALLBACK GetFloatBufferCallback(UnityAudioEffectState*, const char*, float*, int) {
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
