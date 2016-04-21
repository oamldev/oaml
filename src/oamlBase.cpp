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

#include "tinyxml2.h"
#include "oamlCommon.h"
#ifdef __HAVE_GITSHA1_H
#include "GitSHA1.h"
#endif

static void* oamlOpen(const char *filename) {
	return fopen(filename, "rb");
}

static size_t oamlRead(void *ptr, size_t size, size_t nitems, void *fd) {
	return fread(ptr, size, nitems, (FILE*)fd);
}

static int oamlSeek(void *fd, long offset, int whence) {
	return fseek((FILE*)fd, offset, whence);
}

static long oamlTell(void *fd) {
	return ftell((FILE*)fd);
}

static int oamlClose(void *fd) {
	return fclose((FILE*)fd);
}


static oamlFileCallbacks defCbs = {
	&oamlOpen,
	&oamlRead,
	&oamlSeek,
	&oamlTell,
	&oamlClose
};

oamlBase::oamlBase() {
	defsFile = "";

	verbose = false;
	debugClipping = false;
	writeAudioAtShutdown = false;
	useCompressor = false;
	updateTension = false;

	curTrack = NULL;

	fullBuffer = NULL;

	sampleRate = 0;
	channels = 0;
	bytesPerSample = 0;

	volume = OAML_VOLUME_DEFAULT;
	pause = false;

	timeMs = 0;
	tension = 0;
	tensionMs = 0;

	fcbs = &defCbs;
}

oamlBase::~oamlBase() {
	if (fullBuffer) {
		delete fullBuffer;
		fullBuffer = NULL;
	}
}

oamlRC oamlBase::ReadAudioDefs(tinyxml2::XMLElement *el, oamlTrack *track) {
	oamlAudio *audio = new oamlAudio(fcbs, verbose);

	tinyxml2::XMLElement *audioEl = el->FirstChildElement();
	while (audioEl != NULL) {
		if (strcmp(audioEl->Name(), "filename") == 0) {
			const char *layer = audioEl->Attribute("layer");
			if (layer) {
				if (GetLayerId(layer) == -1) AddLayer(layer);
				const char *randomChance = audioEl->Attribute("randomChance");
				if (randomChance) {
					SetLayerRandomChance(layer, strtol(randomChance, NULL, 0));
				}
				audio->SetFilename(audioEl->GetText(), layer, GetLayer(layer));
			} else {
				audio->SetFilename(audioEl->GetText(), "", NULL);
			}
		} else if (strcmp(audioEl->Name(), "type") == 0) audio->SetType(strtol(audioEl->GetText(), NULL, 0));
		else if (strcmp(audioEl->Name(), "bars") == 0) audio->SetBars(strtol(audioEl->GetText(), NULL, 0));
		else if (strcmp(audioEl->Name(), "volume") == 0) audio->SetVolume(float(atof(audioEl->GetText())));
		else if (strcmp(audioEl->Name(), "bpm") == 0) audio->SetBPM(float(atof(audioEl->GetText())));
		else if (strcmp(audioEl->Name(), "beatsPerBar") == 0) audio->SetBeatsPerBar(strtol(audioEl->GetText(), NULL, 0));
		else if (strcmp(audioEl->Name(), "minMovementBars") == 0) audio->SetMinMovementBars(strtol(audioEl->GetText(), NULL, 0));
		else if (strcmp(audioEl->Name(), "randomChance") == 0) audio->SetRandomChance(strtol(audioEl->GetText(), NULL, 0));
		else if (strcmp(audioEl->Name(), "fadeIn") == 0) audio->SetFadeIn(strtol(audioEl->GetText(), NULL, 0));
		else if (strcmp(audioEl->Name(), "fadeOut") == 0) audio->SetFadeOut(strtol(audioEl->GetText(), NULL, 0));
		else if (strcmp(audioEl->Name(), "xfadeIn") == 0) audio->SetXFadeIn(strtol(audioEl->GetText(), NULL, 0));
		else if (strcmp(audioEl->Name(), "xfadeOut") == 0) audio->SetXFadeOut(strtol(audioEl->GetText(), NULL, 0));
		else if (strcmp(audioEl->Name(), "condId") == 0) audio->SetCondId(strtol(audioEl->GetText(), NULL, 0));
		else if (strcmp(audioEl->Name(), "condType") == 0) audio->SetCondType(strtol(audioEl->GetText(), NULL, 0));
		else if (strcmp(audioEl->Name(), "condValue") == 0) audio->SetCondValue(strtol(audioEl->GetText(), NULL, 0));
		else if (strcmp(audioEl->Name(), "condValue2") == 0) audio->SetCondValue2(strtol(audioEl->GetText(), NULL, 0));
		else {
			printf("%s: Unknown audio tag: %s\n", __FUNCTION__, audioEl->Name());
		}

		audioEl = audioEl->NextSiblingElement();
	}

	track->AddAudio(audio);
	return OAML_OK;
}

oamlRC oamlBase::ReadTrackDefs(tinyxml2::XMLElement *el) {
	oamlTrack *track;

	if (el->Attribute("type", "sfx")) {
		track = new oamlSfxTrack(verbose);
	} else {
		track = new oamlMusicTrack(verbose);
	}
	if (track == NULL) return OAML_ERROR;

	tinyxml2::XMLElement *trackEl = el->FirstChildElement();
	while (trackEl != NULL) {
		if (strcmp(trackEl->Name(), "name") == 0) track->SetName(trackEl->GetText());
		else if (strcmp(trackEl->Name(), "group") == 0) track->AddGroup(trackEl->GetText());
		else if (strcmp(trackEl->Name(), "subgroup") == 0) track->AddSubgroup(trackEl->GetText());
		else if (strcmp(trackEl->Name(), "fadeIn") == 0) track->SetFadeIn(strtol(trackEl->GetText(), NULL, 0));
		else if (strcmp(trackEl->Name(), "fadeOut") == 0) track->SetFadeOut(strtol(trackEl->GetText(), NULL, 0));
		else if (strcmp(trackEl->Name(), "xfadeIn") == 0) track->SetXFadeIn(strtol(trackEl->GetText(), NULL, 0));
		else if (strcmp(trackEl->Name(), "xfadeOut") == 0) track->SetXFadeOut(strtol(trackEl->GetText(), NULL, 0));
		else if (strcmp(trackEl->Name(), "volume") == 0) track->SetVolume(float(atof(trackEl->GetText())));
		else if (strcmp(trackEl->Name(), "audio") == 0) {
			oamlRC ret = ReadAudioDefs(trackEl, track);
			if (ret != OAML_OK) return ret;
		} else {
			printf("%s: Unknown track tag: %s\n", __FUNCTION__, trackEl->Name());
		}

		trackEl = trackEl->NextSiblingElement();
	}

	if (track->IsMusicTrack()) {
		musicTracks.push_back(track);
	} else {
		sfxTracks.push_back(track);
	}

	return OAML_OK;
}

oamlRC oamlBase::ReadDefs(const char *buf, int size) {
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError err = doc.Parse(buf, size);
	if (err != tinyxml2::XML_NO_ERROR) {
		fprintf(stderr, "liboaml: Error parsing xml: %s (err=%d)\n", doc.ErrorName(), err);
		return OAML_ERROR;
	}

	tinyxml2::XMLElement *el = doc.FirstChildElement("track");
	while (el != NULL) {
		oamlRC ret = ReadTrackDefs(el);
		if (ret != OAML_OK) return ret;

		el = el->NextSiblingElement();
	}

	return OAML_OK;
}

void oamlBase::ReadInternalDefs(const char *filename) {
	tinyxml2::XMLDocument doc;

	if (doc.LoadFile(filename) != tinyxml2::XML_NO_ERROR)
		return;

	tinyxml2::XMLElement *el = doc.FirstChildElement("base");
	while (el != NULL) {
		tinyxml2::XMLElement *cel = el->FirstChildElement();
		while (cel != NULL) {
			if (strcmp(cel->Name(), "writeAudioAtShutdown") == 0) SetWriteAudioAtShutdown(strtol(cel->GetText(), NULL, 0) == 1);
			else if (strcmp(cel->Name(), "debugClipping") == 0) SetDebugClipping(strtol(cel->GetText(), NULL, 0) == 1);
			else if (strcmp(cel->Name(), "verbose") == 0) SetVerbose(strtol(cel->GetText(), NULL, 0) == 1);

			cel = cel->NextSiblingElement();
		}

		el = el->NextSiblingElement();
	}

#ifdef __HAVE_GITSHA1_H
	if (verbose) {
		__oamlLog("OAML git sha1: %s\n", GIT_SHA1);
	}
#endif
}

oamlRC oamlBase::Init(const char *defsFilename) {
	ASSERT(defsFilename != NULL);

	if (verbose) __oamlLog("%s: %s\n", __FUNCTION__, defsFilename);

	// In case we're being re-initialized clear previous tracks
	Clear();

	ReadInternalDefs("oamlInternal.defs");

	return ReadDefsFile(defsFilename);
}

oamlRC oamlBase::ReadDefsFile(const char *defsFilename) {
	ByteBuffer buf;
	void *fd;

	ASSERT(defsFilename != NULL);

	if (verbose) __oamlLog("%s: %s\n", __FUNCTION__, defsFilename);

	defsFile = defsFilename;
	fd = fcbs->open(defsFilename);
	if (fd == NULL) {
		fprintf(stderr, "liboaml: Error loading definitions '%s'\n", defsFilename);
		return OAML_ERROR;
	}

	uint8_t buffer[4096];
	size_t bytes;
	do {
		bytes = fcbs->read(buffer, 1, 4096, fd);
		buf.putBytes(buffer, bytes);
	} while (bytes >= 4096);
	fcbs->close(fd);

	uint8_t *cbuf = new uint8_t[buf.size()];
	buf.getBytes(cbuf, buf.size());

	oamlRC ret = ReadDefs((const char*)cbuf, buf.size());
	delete[] cbuf;
	return ret;
}

oamlRC oamlBase::InitString(const char *defs) {
	ASSERT(defs != NULL);

	if (verbose) __oamlLog("%s\n", __FUNCTION__);

	// In case we're being re-initialized clear previous tracks
	Clear();

	ReadInternalDefs("oamlInternal.defs");

	return ReadDefs(defs, strlen(defs));
}

void oamlBase::SetAudioFormat(int audioSampleRate, int audioChannels, int audioBytesPerSample, bool audioFloatBuffer) {
	sampleRate = audioSampleRate;
	channels = audioChannels;
	bytesPerSample = audioBytesPerSample;
	floatBuffer = audioFloatBuffer;

	if (useCompressor) {
		compressor.SetAudioFormat(channels, sampleRate);
	}
}

oamlRC oamlBase::PlayTrackId(int id) {
	if (id >= (int)musicTracks.size())
		return OAML_ERROR;

	if (curTrack) {
		curTrack->Stop();
	}

	curTrack = musicTracks[id];
	return curTrack->Play();
}

oamlRC oamlBase::PlayTrack(const char *name) {
	ASSERT(name != NULL);

	if (verbose) __oamlLog("%s %s\n", __FUNCTION__, name);

	for (std::vector<oamlTrack*>::iterator it=musicTracks.begin(); it<musicTracks.end(); ++it) {
		oamlTrack *track = *it;
		if (track->GetName().compare(name) == 0) {
			if (curTrack) curTrack->Stop();
			curTrack = track;
			return curTrack->Play();
		}
	}

	return OAML_ERROR;
}

oamlRC oamlBase::PlaySfx(const char *name) {
	return PlaySfxEx(name, 1.f, 0.f);
}

oamlRC oamlBase::PlaySfxEx(const char *name, float vol, float pan) {
	ASSERT(name != NULL);

	if (verbose) __oamlLog("%s %s\n", __FUNCTION__, name);

	for (std::vector<oamlTrack*>::iterator it=sfxTracks.begin(); it<sfxTracks.end(); ++it) {
		oamlTrack *track = *it;
		if (track->Play(name, vol, pan) == 0) {
			return OAML_OK;
		}
	}

	return OAML_ERROR;
}

static double getDistance2d(int x1, int y1, int x2, int y2) {
	int dx = (x2 - x1) * (x2 - x1);
	int dy = (y2 - y1) * (y2 - y1);

	return sqrt((double)dx + dy);
}

oamlRC oamlBase::PlaySfx2d(const char *name, int x, int y, int width, int height) {
	double posx = double(x) / double(width);
	if (posx > 1.0) posx = 1.0;
	if (posx < 0.0) posx = 0.0;

	double d1 = getDistance2d(0, 0, width + width / 2, height + height / 2);
	double d2 = getDistance2d(x, y, width / 2, height / 2);
	float vol = 1.f - float(d2 / d1);
	if (vol < 0.f) vol = 0.f;
	if (vol > 1.f) vol = 1.f;

	float pan = float((posx * 2.0) - 1.0);
	return PlaySfxEx(name, vol, pan);
}

oamlRC oamlBase::PlayTrackWithStringRandom(const char *str) {
	std::vector<int> list;

	ASSERT(str != NULL);

	if (verbose) __oamlLog("%s %s\n", __FUNCTION__, str);

	for (size_t i=0; i<musicTracks.size(); i++) {
		if (musicTracks[i]->GetName().find(str) == std::string::npos) {
			list.push_back(i);
		}
	}

	if (list.empty() == false) {
		int i = rand() % list.size();
		return PlayTrackId(list[i]);
	}

	return OAML_ERROR;
}

oamlRC oamlBase::PlayTrackByGroupRandom(const char *group) {
	std::vector<int> list;

	ASSERT(group != NULL);

	if (verbose) __oamlLog("%s %s\n", __FUNCTION__, group);

	for (size_t i=0; i<musicTracks.size(); i++) {
		if (musicTracks[i]->HasGroup(std::string(group))) {
			list.push_back(i);
		}
	}

	if (list.empty() == false) {
		int i = rand() % list.size();
		return PlayTrackId(list[i]);
	}

	return OAML_ERROR;
}

oamlRC oamlBase::PlayTrackByGroupAndSubgroupRandom(const char *group, const char *subgroup) {
	std::vector<int> list;

	ASSERT(group != NULL);
	ASSERT(subgroup != NULL);

	if (verbose) __oamlLog("%s %s %s\n", __FUNCTION__, group, subgroup);

	for (size_t i=0; i<musicTracks.size(); i++) {
		if (musicTracks[i]->HasGroup(std::string(group)) && musicTracks[i]->HasSubgroup(std::string(subgroup))) {
			list.push_back(i);
		}
	}

	if (list.empty() == false) {
		int i = rand() % list.size();
		return PlayTrackId(list[i]);
	}

	return OAML_ERROR;
}

oamlRC oamlBase::LoadTrack(const char *name) {
	ASSERT(name != NULL);

	if (verbose) __oamlLog("%s %s\n", __FUNCTION__, name);

	for (std::vector<oamlTrack*>::iterator it=musicTracks.begin(); it<musicTracks.end(); ++it) {
		oamlTrack *track = *it;
		if (track->GetName().compare(name) == 0) {
			return track->Load();
		}
	}

	return OAML_ERROR;
}

bool oamlBase::IsTrackPlaying(const char *name) {
	ASSERT(name != NULL);

	for (size_t i=0; i<musicTracks.size(); i++) {
		if (musicTracks[i]->GetName().compare(name) == 0) {
			return IsTrackPlayingId(i);
		}
	}

	return false;
}

bool oamlBase::IsTrackPlayingId(int id) {
	if (id >= (int)musicTracks.size())
		return false;

	return musicTracks[id]->IsPlaying();
}

bool oamlBase::IsPlaying() {
	for (size_t i=0; i<musicTracks.size(); i++) {
		if (musicTracks[i]->IsPlaying())
			return true;
	}

	return false;
}

void oamlBase::StopPlaying() {
	if (verbose) __oamlLog("%s\n", __FUNCTION__);
	for (size_t i=0; i<musicTracks.size(); i++) {
		musicTracks[i]->Stop();
	}
}

void oamlBase::Pause() {
	pause = true;
}

void oamlBase::Resume() {
	pause = false;
}

void oamlBase::PauseToggle() {
	pause = !pause;
}

void oamlBase::ShowPlayingTracks() {
	for (size_t i=0; i<musicTracks.size(); i++) {
		musicTracks[i]->ShowPlaying();
	}
}

int oamlBase::SafeAdd(int sample1, int sample2) {
	bool clipping;
	int ret;

	// Detect integer overflow and underflow, both will cause clipping in our audio
	if (sample1 > 0 && sample2 > INT_MAX - sample1) {
		int64_t s64a = sample1;
		int64_t s64b = sample2;
		ret = int(INT_MAX - ((s64a + s64b) - INT_MAX));
		clipping = true;
	} else if (sample1 < 0 && sample2 < INT_MIN - sample1) {
		int64_t s64a = sample1;
		int64_t s64b = sample2;
		ret = int(INT_MIN - ((s64a + s64b) - INT_MIN));
		clipping = true;
	} else {
		ret = sample1 + sample2;
		clipping = false;
	}

	if (clipping && debugClipping) {
		fprintf(stderr, "oaml: Detected clipping!\n");
		ShowPlayingTracks();
	}

	return ret;
}

int oamlBase::ReadSample(void *buffer, int index) {
	switch (bytesPerSample) {
		case 1: { // 8bit (unsigned)
			uint8_t *cbuf = (uint8_t *)buffer;
			return (int)cbuf[index]<<23; }
			break;

		case 2: { // 16bit (signed)
			int16_t *sbuf = (int16_t *)buffer;
			return (int)sbuf[index]<<16; }
			break;

		case 3: { // 24bit
			// TODO: Test me!
			uint8_t *cbuf = (uint8_t *)buffer;
			int tmp = (int)cbuf[index*3+0]<<8;
			tmp|= (int)cbuf[index*3+1]<<16;
			tmp|= (int)cbuf[index*3+2]<<24;
			return tmp; }
			break;

		case 4: { // 32bit (signed)
			int32_t *ibuf = (int32_t *)buffer;
			return (int)ibuf[index]; }
			break;

	}

	return 0;
}

void oamlBase::WriteSample(void *buffer, int index, int sample) {
	switch (bytesPerSample) {
		case 1: { // 8bit (unsigned)
			uint8_t *cbuf = (uint8_t *)buffer;
			cbuf[index] = (uint8_t)(sample>>23); }
			break;

		case 2: { // 16bit (signed)
			int16_t *sbuf = (int16_t *)buffer;
			sbuf[index] = (int16_t)(sample>>16); }
			break;

		case 3: { // 24bit
			// TODO: Test me!
			uint8_t *cbuf = (uint8_t *)buffer;
			cbuf[index*3+0] = (uint8_t)(sample>>8);
			cbuf[index*3+1] = (uint8_t)(sample>>16);
			cbuf[index*3+2] = (uint8_t)(sample>>24); }
			break;

		case 4: { // 32bit (signed)
			int32_t *ibuf = (int32_t *)buffer;
			ibuf[index] = (int32_t)sample; }
			break;

	}
}

bool oamlBase::IsAudioFormatSupported() {
	// Basic check, we need a sampleRate
	if (sampleRate == 0)
		return false;

	// Only mono or stereo for now
	if (channels <= 0 || channels > 2)
		return false;

	// Supported 8bit, 16bit, 24bit and 32bit, also float with floatBuffer == true
	if (bytesPerSample <= 0 || bytesPerSample > 4)
		return false;

	return true;
}

void oamlBase::MixToBuffer(void *buffer, int size) {
	ASSERT(buffer != NULL);
	ASSERT(size != 0);

	if (IsAudioFormatSupported() == false || pause)
		return;

	for (int i=0; i<size; i+= channels) {
		float fsample[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

		for (size_t j=0; j<sfxTracks.size(); j++) {
			sfxTracks[j]->Mix(fsample, channels, debugClipping);
		}

		for (size_t j=0; j<musicTracks.size(); j++) {
			musicTracks[j]->Mix(fsample, channels, debugClipping);
		}

		// Apply effects
		if (useCompressor) {
			compressor.ProcessData(fsample);
		}

		for (int c=0; c<channels; c++) {
			// Apply the volume
			fsample[c] = fsample[c] * volume;

			if (floatBuffer) {
				((float*)buffer)[i+c]+= fsample[c];
			} else {
				int sample = __oamlFloatToInteger24(fsample[c]) << 8;

				// Mix our sample into the buffer
				int tmp = ReadSample(buffer, i+c);
				tmp = SafeAdd(sample, tmp);
				WriteSample(buffer, i+c, tmp);
			}
		}
	}

	if (writeAudioAtShutdown) {
		for (int i=0; i<size; i++) {
			int sample = ReadSample(buffer, i);
			if (fullBuffer == NULL) {
				fullBuffer = new ByteBuffer();
			}

			fullBuffer->putShort(sample >> 16);
		}
	}

//	ShowPlayingTracks();
}

void oamlBase::SetCondition(int id, int value) {
//	printf("%s %d %d\n", __FUNCTION__, id, value);
	if (curTrack) {
		curTrack->SetCondition(id, value);
	}
}

void oamlBase::SetVolume(float vol) {
	volume = vol;

	if (volume < OAML_VOLUME_MIN) volume = OAML_VOLUME_MIN;
	if (volume > OAML_VOLUME_MAX) volume = OAML_VOLUME_MAX;
}

void oamlBase::AddTension(int value) {
	tension+= value;
	if (tension >= 100) {
		tension = 100;
	}

	updateTension = true;
}

void oamlBase::SetTension(int value) {
	tension = value;

	updateTension = false;
}

void oamlBase::SetMainLoopCondition(int value) {
	SetCondition(OAML_CONDID_MAIN_LOOP, value);
}

void oamlBase::AddLayer(const char *layer) {
	if (GetLayerId(layer) == -1) {
		oamlLayerData *info = new oamlLayerData();
		info->id = layers.size();
		info->name = layer;
		info->randomChance = 100;
		info->gain = 1.f;
		layers.push_back(info);
	}
}

int oamlBase::GetLayerId(const char *layer) {
	if (layer == NULL)
		return -1;

	for (std::vector<oamlLayerData*>::iterator it=layers.begin(); it<layers.end(); ++it) {
		oamlLayerData *info = *it;
		if (info->name.compare(layer) == 0) {
			return info->id;
		}
	}

	return -1;
}

oamlLayerData* oamlBase::GetLayer(const char *layer) {
	if (layer == NULL)
		return NULL;

	for (std::vector<oamlLayerData*>::iterator it=layers.begin(); it<layers.end(); ++it) {
		oamlLayerData *info = *it;
		if (info->name.compare(layer) == 0) {
			return info;
		}
	}

	return NULL;
}

void oamlBase::SetLayerGain(const char *layer, float gain) {
	oamlLayerData *info = GetLayer(layer);
	if (info == NULL)
		return;

	info->gain = gain;
}

void oamlBase::SetLayerRandomChance(const char *layer, int randomChance) {
	oamlLayerData *info = GetLayer(layer);
	if (info == NULL)
		return;

	info->randomChance = randomChance;
}

void oamlBase::UpdateTension(uint64_t ms) {
//	printf("%s %d %lld %d\n", __FUNCTION__, tension, tensionMs - ms, ms >= (tensionMs + 5000));
	// Don't allow sudden changes of tension after it changed back to 0
	if (tension > 0) {
		SetCondition(OAML_CONDID_TENSION, tension);
		tensionMs = ms;
	} else {
		if (ms >= (tensionMs + 5000)) {
			SetCondition(OAML_CONDID_TENSION, tension);
			tensionMs = ms;
		}
	}

	// Lower tension
	if (tension >= 1) {
		if (tension >= 2) {
			tension-= (tension+20)/10;
			if (tension < 0)
				tension = 0;
		} else {
			tension--;
		}
	}
}

void oamlBase::Update() {
	uint64_t ms = GetTimeMs64();

	// Update each second
	if (ms >= (timeMs + 1000)) {
		if (verbose) ShowPlayingTracks();

		if (updateTension) {
			UpdateTension(ms);
		}

		timeMs = ms;
	}
}

void oamlBase::SetFileCallbacks(oamlFileCallbacks *cbs) {
	fcbs = cbs;
}

void oamlBase::EnableDynamicCompressor(bool enable, double threshold, double ratio) {
	useCompressor = enable;
	if (useCompressor) {
		compressor.SetThreshold(threshold);
		compressor.SetRatio(ratio);
	}
}

oamlTracksInfo* oamlBase::GetTracksInfo() {
	tracksInfo.tracks.clear();

	for (std::vector<oamlTrack*>::iterator it=musicTracks.begin(); it<musicTracks.end(); ++it) {
		oamlTrack *track = *it;

		oamlTrackInfo tinfo;
		track->ReadInfo(&tinfo);
		tracksInfo.tracks.push_back(tinfo);
	}

	for (std::vector<oamlTrack*>::iterator it=sfxTracks.begin(); it<sfxTracks.end(); ++it) {
		oamlTrack *track = *it;

		oamlTrackInfo tinfo;
		track->ReadInfo(&tinfo);
		tracksInfo.tracks.push_back(tinfo);
	}

	return &tracksInfo;
}

const char* oamlBase::GetDefsFile() {
	return defsFile.c_str();
}

const char* oamlBase::GetPlayingInfo() {
	playingInfo = "";
	for (size_t i=0; i<musicTracks.size(); i++) {
		playingInfo+= musicTracks[i]->GetPlayingInfo();
	}

	if (tension > 0) {
		char str[1024];
		snprintf(str, 1024, " tension=%d", tension);
		playingInfo+= str;
	}

	return playingInfo.c_str();
}

void oamlBase::Clear() {
	while (musicTracks.empty() == false) {
		oamlTrack *track = musicTracks.back();
		musicTracks.pop_back();

		delete track;
	}

	while (sfxTracks.empty() == false) {
		oamlTrack *track = sfxTracks.back();
		sfxTracks.pop_back();

		delete track;
	}

	for (size_t i=0; i<tracksInfo.tracks.size(); i++) {
		tracksInfo.tracks[i].audios.clear();
	}
	tracksInfo.tracks.clear();

	curTrack = NULL;
}

void oamlBase::Shutdown() {
	if (verbose) __oamlLog("%s\n", __FUNCTION__);

	Clear();

	if (writeAudioAtShutdown && fullBuffer) {
		char filename[1024];
		snprintf(filename, 1024, "oaml-%d.wav", (int)time(NULL));
		wavFile *wav = new wavFile(fcbs);
		wav->WriteToFile(filename, fullBuffer, channels, sampleRate, 2);
		delete wav;
	}
}
