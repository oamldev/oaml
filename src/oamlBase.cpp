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

static void *pbase = NULL;

oamlBase::oamlBase() {
	defsFile = "";

	verbose = false;
	debugClipping = false;
	writeAudioAtShutdown = false;
	useCompressor = false;
	updateTension = false;

	bpm = 0.f;
	beatsPerBar = 0;

#ifdef __HAVE_RTAUDIO
	rtAudio = NULL;
#endif

	curTrack = -1;
	fullBuffer = NULL;

	sampleRate = 0;
	channels = 0;
	bytesPerSample = 0;
	floatBuffer = false;
	dataBuffer = new ByteBuffer();

	volume = OAML_VOLUME_DEFAULT;
	pause = false;

	timeMs = 0;
	tension = 0;
	tensionMs = 0;

	fcbs = &defCbs;

	bufferFrames = 2048;
	stopThread = false;
	pbase = this;
	bufferThread = new std::thread(&oamlBase::BufferThreadFunc);
}

oamlBase::~oamlBase() {
	if (bufferThread) {
		stopThread = true;
		bufferThread->join();
		delete bufferThread;
		bufferThread = NULL;
	}

#ifdef __HAVE_RTAUDIO
	if (rtAudio) {
		if (rtAudio->isStreamRunning()) {
			rtAudio->closeStream();
		}

		delete rtAudio;
		rtAudio = NULL;
	}
#endif

	if (fullBuffer) {
		delete fullBuffer;
		fullBuffer = NULL;
	}

	if (dataBuffer) {
		delete dataBuffer;
		dataBuffer = NULL;
	}
}

const char *oamlBase::GetVersion() {
	return "1.3.4";
}

#ifdef __HAVE_RTAUDIO
int rtCallback(void *outputBuffer, void * /*inputBuffer*/, unsigned int nBufferFrames, double /*streamTime*/, RtAudioStreamStatus /*status*/, void *data) {
	oamlBase *base = (oamlBase*)data;
	memset(outputBuffer, 0, nBufferFrames*2*2);
	base->MixToBuffer(outputBuffer, nBufferFrames*2);
	return 0;
}
#endif

oamlRC oamlBase::InitAudioDevice(int sampleRate, int channels) {
#ifdef __HAVE_RTAUDIO
	RtAudio::StreamParameters params;
	unsigned int bufferSize = 1024;

	if (rtAudio == NULL) {
		rtAudio = new RtAudio();
	} else {
		// Close the stream if it's already open, to allow re-initialization of the device
		if (rtAudio->isStreamRunning()) {
			rtAudio->closeStream();
		}
	}

	params.deviceId = rtAudio->getDefaultOutputDevice();
	params.nChannels = channels;
	params.firstChannel = 0;
	try {
		rtAudio->openStream(&params, NULL, RTAUDIO_SINT16, sampleRate, &bufferSize, &rtCallback, (void*)this);
		rtAudio->startStream();

		SetAudioFormat(sampleRate, channels, 2, false);
	}
	catch (RtAudioError &e) {
		rtAudio->closeStream();
		return OAML_ERROR;
	}

	return OAML_OK;
#else
	return OAML_ERROR;
#endif
}

oamlRC oamlBase::ReadAudioDefs(tinyxml2::XMLElement *el, oamlTrack *track) {
	oamlAudio *audio = new oamlAudio(this, fcbs, verbose);

	tinyxml2::XMLElement *audioEl = el->FirstChildElement();
	while (audioEl != NULL) {
		if (strcmp(audioEl->Name(), "name") == 0) audio->SetName(audioEl->GetText());
		else if (strcmp(audioEl->Name(), "filename") == 0) {
			const char *layer = audioEl->Attribute("layer");
			if (layer) {
				const char *randomChanceAttr = audioEl->Attribute("randomChance");
				int randomChance = randomChanceAttr ? strtol(randomChanceAttr, NULL, 0) : -1;

				if (GetLayerId(layer) == -1) {
					AddLayer(layer);
				}

				audio->AddAudioFile(audioEl->GetText(), layer, randomChance);
			} else {
				audio->AddAudioFile(audioEl->GetText());
			}
		} else if (strcmp(audioEl->Name(), "type") == 0) audio->SetType(strtol(audioEl->GetText(), NULL, 0));
		else if (strcmp(audioEl->Name(), "bars") == 0) audio->SetBars(strtol(audioEl->GetText(), NULL, 0));
		else if (strcmp(audioEl->Name(), "volume") == 0) audio->SetVolume(float(atof(audioEl->GetText())));
		else if (strcmp(audioEl->Name(), "bpm") == 0) audio->SetBPM(float(atof(audioEl->GetText())));
		else if (strcmp(audioEl->Name(), "beatsPerBar") == 0) audio->SetBeatsPerBar(strtol(audioEl->GetText(), NULL, 0));
		else if (strcmp(audioEl->Name(), "minMovementBars") == 0) audio->SetMinMovementBars(strtol(audioEl->GetText(), NULL, 0));
		else if (strcmp(audioEl->Name(), "randomChance") == 0) audio->SetRandomChance(strtol(audioEl->GetText(), NULL, 0));
		else if (strcmp(audioEl->Name(), "playOrder") == 0) audio->SetPlayOrder(strtol(audioEl->GetText(), NULL, 0));
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

	if (audio->GetName() == "" || track->GetAudio(audio->GetName())) {
		std::vector<std::string> list;
		track->GetAudioList(list);
		char name[256];
		snprintf(name, sizeof(name), "audio%d", (int)list.size());
		audio->SetName(name);
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

	mutex.lock();
	if (track->IsMusicTrack()) {
		musicTracks.push_back((oamlMusicTrack*)track);
	} else {
		sfxTracks.push_back((oamlSfxTrack*)track);
	}
	mutex.unlock();

	return OAML_OK;
}

oamlRC oamlBase::ReadDefs(const char *buf, int size) {
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError err = doc.Parse(buf, size);
	if (err != tinyxml2::XML_NO_ERROR) {
		fprintf(stderr, "liboaml: Error parsing xml: %s (err=%d)\n", doc.ErrorName(), err);
		return OAML_ERROR;
	}

	tinyxml2::XMLElement *prjEl = doc.FirstChildElement("project");
	if (prjEl) {
		// New oaml.defs, root node is a <project> tag which contains project settings and a list of tracks
		tinyxml2::XMLElement *el = prjEl->FirstChildElement();
		while (el != NULL) {
			if (strcmp(el->Name(), "track") == 0) {
				oamlRC ret = ReadTrackDefs(el);
				if (ret != OAML_OK) return ret;
			} else if (strcmp(el->Name(), "bpm") == 0) {
				ProjectSetBPM(float(atof(el->GetText())));
			} else if (strcmp(el->Name(), "beatsPerBar") == 0) {
				ProjectSetBeatsPerBar(strtol(el->GetText(), NULL, 0));
			} else {
				printf("%s: Unknown project tag: %s\n", __FUNCTION__, el->Name());
			}

			el = el->NextSiblingElement();
		}
	} else {
		// Old oaml.defs, search for al the <track> tags
		tinyxml2::XMLElement *el = doc.FirstChildElement("track");
		while (el != NULL) {
			oamlRC ret = ReadTrackDefs(el);
			if (ret != OAML_OK) return ret;

			el = el->NextSiblingElement();
		}
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
	oamlRC err = OAML_ERROR;

	if (id < (int)musicTracks.size()) {
		if (curTrack >= 0 && (size_t)curTrack < musicTracks.size()) {
			musicTracks[curTrack]->Stop();
		}

		curTrack = id;
		int mainCondValue = 0;
		for (size_t i=0; i<conditions.size(); i++) {
			if (conditions[i].first == OAML_CONDID_MAIN_LOOP) {
				mainCondValue = conditions[i].second;
				break;
			}
		}

		err = musicTracks[id]->Play(mainCondValue);
		UpdateCondition();
	}

	return err;
}

oamlRC oamlBase::PlayTrack(const char *name) {
	ASSERT(name != NULL);

	if (verbose) __oamlLog("%s %s\n", __FUNCTION__, name);

	oamlRC err = OAML_ERROR;
	int i=0;

	mutex.lock();
	for (std::vector<oamlMusicTrack*>::iterator it=musicTracks.begin(); it<musicTracks.end(); ++it, i++) {
		oamlTrack *track = *it;
		if (track->GetName().compare(name) == 0) {
			err = PlayTrackId(i);
			break;
		}
	}
	mutex.unlock();

	return err;
}

oamlRC oamlBase::PlaySfx(const char *name) {
	return PlaySfxEx(name, 1.f, 0.f);
}

oamlRC oamlBase::PlaySfxEx(const char *name, float vol, float pan) {
	ASSERT(name != NULL);

	if (verbose) __oamlLog("%s %s\n", __FUNCTION__, name);

	oamlRC err = OAML_ERROR;

	mutex.lock();
	for (std::vector<oamlSfxTrack*>::iterator it=sfxTracks.begin(); it<sfxTracks.end(); ++it) {
		oamlSfxTrack *track = *it;
		if (track->Play(name, vol, pan) == 0) {
			err = OAML_OK;
			break;
		}
	}
	mutex.unlock();

	return err;
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
	oamlRC err = OAML_ERROR;

	ASSERT(str != NULL);

	if (verbose) __oamlLog("%s %s\n", __FUNCTION__, str);

	mutex.lock();
	for (size_t i=0; i<musicTracks.size(); i++) {
		if (musicTracks[i]->GetName().find(str) == std::string::npos) {
			list.push_back(i);
		}
	}

	if (list.empty() == false) {
		int i = rand() % list.size();
		err = PlayTrackId(list[i]);
	}
	mutex.unlock();

	return err;
}

oamlRC oamlBase::PlayTrackByGroupRandom(const char *group) {
	std::vector<int> list;
	oamlRC err = OAML_ERROR;

	ASSERT(group != NULL);

	if (verbose) __oamlLog("%s %s\n", __FUNCTION__, group);

	mutex.lock();
	for (size_t i=0; i<musicTracks.size(); i++) {
		if (musicTracks[i]->HasGroup(std::string(group))) {
			list.push_back(i);
		}
	}

	if (list.empty() == false) {
		int i = rand() % list.size();
		err = PlayTrackId(list[i]);
	}
	mutex.unlock();

	return err;
}

oamlRC oamlBase::PlayTrackByGroupAndSubgroupRandom(const char *group, const char *subgroup) {
	std::vector<int> list;
	oamlRC err = OAML_ERROR;

	ASSERT(group != NULL);
	ASSERT(subgroup != NULL);

	if (verbose) __oamlLog("%s %s %s\n", __FUNCTION__, group, subgroup);

	mutex.lock();
	for (size_t i=0; i<musicTracks.size(); i++) {
		if (musicTracks[i]->HasGroup(std::string(group)) && musicTracks[i]->HasSubgroup(std::string(subgroup))) {
			list.push_back(i);
		}
	}

	if (list.empty() == false) {
		int i = rand() % list.size();
		err = PlayTrackId(list[i]);
	}
	mutex.unlock();

	return err;
}

oamlRC oamlBase::LoadTrack(const char *name) {
	oamlRC err = OAML_ERROR;

	ASSERT(name != NULL);

	if (verbose) __oamlLog("%s %s\n", __FUNCTION__, name);

	mutex.lock();
	for (std::vector<oamlMusicTrack*>::iterator it=musicTracks.begin(); it<musicTracks.end(); ++it) {
		oamlTrack *track = *it;
		if (track->GetName().compare(name) == 0) {
			err = track->Load();
			break;
		}
	}
	mutex.unlock();

	return err;
}

float oamlBase::LoadTrackProgress(const char *name) {
	float ret = -1.f;

	ASSERT(name != NULL);

	if (verbose) __oamlLog("%s %s\n", __FUNCTION__, name);

	mutex.lock();
	for (std::vector<oamlMusicTrack*>::iterator it=musicTracks.begin(); it<musicTracks.end(); ++it) {
		oamlTrack *track = *it;
		if (track->GetName().compare(name) == 0) {
			ret = track->LoadProgress();
			break;
		}
	}
	mutex.unlock();

	return ret;
}

bool oamlBase::IsTrackPlaying(const char *name) {
	bool ret = false;

	ASSERT(name != NULL);

	mutex.lock();
	for (size_t i=0; i<musicTracks.size(); i++) {
		if (musicTracks[i]->GetName().compare(name) == 0) {
			ret = IsTrackPlayingId(i);
			break;
		}
	}
	mutex.unlock();

	return ret;
}

bool oamlBase::IsTrackPlayingId(int id) {
	if (id >= (int)musicTracks.size())
		return false;

	return musicTracks[id]->IsPlaying();
}

bool oamlBase::IsPlaying() {
	bool ret = false;

	mutex.lock();
	for (size_t i=0; i<musicTracks.size(); i++) {
		if (musicTracks[i]->IsPlaying()) {
			ret = true;
			break;
		}
	}
	mutex.unlock();

	return ret;
}

void oamlBase::StopPlaying() {
	if (verbose) __oamlLog("%s\n", __FUNCTION__);

	mutex.lock();
	for (size_t i=0; i<musicTracks.size(); i++) {
		musicTracks[i]->Stop();
	}
	mutex.unlock();
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
	mutex.lock();
	for (size_t i=0; i<musicTracks.size(); i++) {
		((oamlTrack*)musicTracks[i])->ShowPlaying();
	}
	mutex.unlock();
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

void oamlBase::BufferThreadFunc() {
	oamlBase *base = (oamlBase*)pbase;
	while (base->stopThread == false) {
		base->mutex.lock();
		if (base->dataBuffer->bytesRemaining() < (uint32_t)base->bufferFrames) {
			base->BufferData();
		}
		base->mutex.unlock();

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void oamlBase::BufferData() {
	if (IsAudioFormatSupported() == false)
		return;

	dataBuffer->setReadPos(0);
	dataBuffer->setWritePos(0);

	for (int i=0; i<bufferFrames; i+= channels) {
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
			dataBuffer->putFloat(fsample[c] * volume);
		}
	}
}

void oamlBase::MixToBuffer(void *buffer, int size) {
	ASSERT(buffer != NULL);
	ASSERT(size != 0);

	if (IsAudioFormatSupported() == false || pause)
		return;

	mutex.lock();
	int i = 0;
	while (size > 0) {
		if (dataBuffer->bytesRemaining() < (uint32_t)channels * 4) {
			BufferData();
		} else {
			for (int c=0; c<channels; c++, i++, size--) {
				float fsample = dataBuffer->getFloat();
				if (floatBuffer) {
					((float*)buffer)[i]+= fsample;
				} else {
					int sample = __oamlFloatToInteger24(fsample) << 8;

					// Mix our sample into the buffer
					int tmp = ReadSample(buffer, i);
					tmp = SafeAdd(sample, tmp);
					WriteSample(buffer, i, tmp);
				}
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
	mutex.unlock();
}

void oamlBase::UpdateCondition() {
	if (curTrack >= 0 && (size_t)curTrack < musicTracks.size()) {
		for (size_t i=0; i<conditions.size(); i++) {
			musicTracks[curTrack]->SetCondition(conditions[i].first, conditions[i].second);
		}
	}
}

void oamlBase::SetCondition(int id, int value) {
//	printf("%s %d %d\n", __FUNCTION__, id, value);
	mutex.lock();
	bool found = false;
	for (size_t i=0; i<conditions.size() && !found; i++) {
		if (conditions[i].first == id) {
			conditions[i].second = value;
			found = true;
		}
	}

	if (!found) {
		conditions.push_back(std::pair<int, int>(id, value));
	}

	UpdateCondition();
	mutex.unlock();
}

void oamlBase::ClearConditions() {
	mutex.lock();
	conditions.clear();
	mutex.unlock();
}

void oamlBase::SetVolume(float vol) {
	mutex.lock();
	volume = vol;

	if (volume < OAML_VOLUME_MIN) volume = OAML_VOLUME_MIN;
	if (volume > OAML_VOLUME_MAX) volume = OAML_VOLUME_MAX;
	mutex.unlock();
}

void oamlBase::AddTension(int value) {
	mutex.lock();
	tension+= value;
	if (tension >= 100) {
		tension = 100;
	}
	mutex.unlock();

	updateTension = true;
}

void oamlBase::SetTension(int value) {
	tension = value;

	updateTension = false;
}

int oamlBase::GetTension() {
	return tension;
}

void oamlBase::SetMainLoopCondition(int value) {
	SetCondition(OAML_CONDID_MAIN_LOOP, value);
}

void oamlBase::AddLayer(std::string layer) {
	if (GetLayerId(layer) == -1) {
		oamlLayer *l = new oamlLayer(layers.size(), layer);
		layers.push_back(l);
	}
}

int oamlBase::GetLayerId(std::string layer) {
	for (std::vector<oamlLayer*>::iterator it=layers.begin(); it<layers.end(); ++it) {
		oamlLayer *info = *it;
		if (info->GetName().compare(layer) == 0) {
			return info->GetId();
		}
	}

	return -1;
}

oamlLayer* oamlBase::GetLayer(std::string layer) {
	for (std::vector<oamlLayer*>::iterator it=layers.begin(); it<layers.end(); ++it) {
		oamlLayer *info = *it;
		if (info->GetName().compare(layer) == 0) {
			return info;
		}
	}

	return NULL;
}

void oamlBase::SetLayerGain(const char *layer, float gain) {
	oamlLayer *info = GetLayer(layer);
	if (info == NULL)
		return;

	info->SetGain(gain);

	mutex.lock();
	for (size_t j=0; j<musicTracks.size(); j++) {
		musicTracks[j]->SetLayerGain(layer, gain);
	}
	mutex.unlock();
}

float oamlBase::GetLayerGain(const char *layer) {
	oamlLayer *info = GetLayer(layer);
	if (info == NULL)
		return 0.f;

	return info->GetGain();
}

void oamlBase::SetLayerRandomChance(const char *layer, int randomChance) {
	oamlLayer *info = GetLayer(layer);
	if (info == NULL)
		return;

	info->SetRandomChance(randomChance);
}

int oamlBase::GetLayerRandomChance(const char *layer) {
	oamlLayer *info = GetLayer(layer);
	if (info == NULL)
		return 0;

	return info->GetRandomChance();
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
	mutex.lock();
	useCompressor = enable;
	if (useCompressor) {
		compressor.SetThreshold(threshold);
		compressor.SetRatio(ratio);
	}
	mutex.unlock();
}

oamlTracksInfo* oamlBase::GetTracksInfo() {
	mutex.lock();
	tracksInfo.tracks.clear();

	for (std::vector<oamlMusicTrack*>::iterator it=musicTracks.begin(); it<musicTracks.end(); ++it) {
		oamlMusicTrack *track = *it;

		oamlTrackInfo tinfo;
		track->ReadInfo(&tinfo);
		tracksInfo.tracks.push_back(tinfo);
	}

	for (std::vector<oamlSfxTrack*>::iterator it=sfxTracks.begin(); it<sfxTracks.end(); ++it) {
		oamlSfxTrack *track = *it;

		oamlTrackInfo tinfo;
		track->ReadInfo(&tinfo);
		tracksInfo.tracks.push_back(tinfo);
	}

	tracksInfo.bpm = bpm;
	tracksInfo.beatsPerBar = beatsPerBar;
	mutex.unlock();

	return &tracksInfo;
}

std::string oamlBase::SaveState() {
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement *rootNode = doc.NewElement("oamlState");
	doc.InsertEndChild(rootNode);

	tinyxml2::XMLElement *versionNode = doc.NewElement("version");
	tinyxml2::XMLText *versionText = doc.NewText("1.0.1");
	versionNode->InsertEndChild(versionText);
	rootNode->InsertEndChild(versionNode);

	tinyxml2::XMLElement *baseNode = doc.NewElement("base");
	baseNode->SetAttribute("curTrack", curTrack);
	baseNode->SetAttribute("tension", tension);
	rootNode->InsertEndChild(baseNode);

	mutex.lock();
	for (size_t i=0; i<conditions.size(); i++) {
		tinyxml2::XMLElement *node = doc.NewElement("condition");
		node->SetAttribute("id", conditions[i].first);
		node->SetAttribute("value", conditions[i].second);
		rootNode->InsertEndChild(node);
	}

	for (size_t i=0; i<musicTracks.size(); i++) {
		tinyxml2::XMLElement *node = doc.NewElement("musicTrack");
		musicTracks[i]->SaveState(doc, node);
		rootNode->InsertEndChild(node);
	}
	mutex.unlock();

	tinyxml2::XMLPrinter printer;
	doc.Print(&printer);
	return printer.CStr();
}

void oamlBase::LoadState(std::string state) {
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError err = doc.Parse(state.c_str(), state.length());
	if (err != tinyxml2::XML_NO_ERROR) {
		fprintf(stderr, "liboaml: Error parsing xml: %s (err=%d)\n", doc.ErrorName(), err);
		return;
	}

	mutex.lock();

	conditions.clear();

	tinyxml2::XMLElement *rootNode = doc.FirstChildElement("oamlState");
	if (rootNode) {
		tinyxml2::XMLElement *el = rootNode->FirstChildElement();
		while (el != NULL) {
			if (strcmp(el->Name(), "version") == 0) {
				int major;
				int minor;
				int patch;

				sscanf(el->GetText(), "%d.%d.%d", &major, &minor, &patch);
				int version = (major << 16) | (minor << 8) | (patch);
				if (version < 0x00010001) {
					fprintf(stderr, "old version! %X\n", version);
					break;
				}
			} else if (strcmp(el->Name(), "base") == 0) {
				curTrack = el->IntAttribute("curTrack");
				tension = el->IntAttribute("tension");
			} else if (strcmp(el->Name(), "condition") == 0) {
				int id = el->IntAttribute("id");
				int value = el->IntAttribute("value");
				conditions.push_back(std::pair<int, int>(id, value));
			} else if (strcmp(el->Name(), "musicTrack") == 0) {
				const char *str = el->Attribute("name");
				if (str) {
					for (size_t i=0; i<musicTracks.size(); i++) {
						if (strcmp(musicTracks[i]->GetNameStr(), str) == 0) {
							musicTracks[i]->LoadState(el);
							break;
						}
					}
				}
			} else {
				fprintf(stderr, "%s: Unknown state tag: %s\n", __FUNCTION__, el->Name());
			}

			el = el->NextSiblingElement();
		}
	}

	UpdateCondition();

	mutex.unlock();
}

const char* oamlBase::GetDefsFile() {
	return defsFile.c_str();
}

const char* oamlBase::GetPlayingInfo() {
	playingInfo = "";

	mutex.lock();
	for (size_t i=0; i<musicTracks.size(); i++) {
		playingInfo+= musicTracks[i]->GetPlayingInfo();
	}

	if (tension > 0) {
		char str[1024];
		snprintf(str, 1024, " tension=%d", tension);
		playingInfo+= str;
	}
	mutex.unlock();

	return playingInfo.c_str();
}

void oamlBase::Clear() {
	mutex.lock();
	while (musicTracks.empty() == false) {
		oamlMusicTrack *track = musicTracks.back();
		musicTracks.pop_back();

		delete track;
	}

	while (sfxTracks.empty() == false) {
		oamlSfxTrack *track = sfxTracks.back();
		sfxTracks.pop_back();

		delete track;
	}

	for (size_t i=0; i<tracksInfo.tracks.size(); i++) {
		tracksInfo.tracks[i].audios.clear();
	}
	tracksInfo.tracks.clear();

	curTrack = -1;
	mutex.unlock();
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

void oamlBase::ProjectNew() {
	StopPlaying();

	Clear();

	bpm = 0.f;
	beatsPerBar = 0;
}

void oamlBase::ProjectSetBPM(float _bpm) {
	bpm = _bpm;
}

void oamlBase::ProjectSetBeatsPerBar(int _beatsPerBar) {
	beatsPerBar = _beatsPerBar;
}

float oamlBase::ProjectGetBPM() {
	return bpm;
}

int oamlBase::ProjectGetBeatsPerBar() {
	return beatsPerBar;
}

oamlRC oamlBase::TrackNew(std::string name, bool sfxTrack) {
	oamlTrack *track;

	if (sfxTrack) {
		track = new oamlSfxTrack(verbose);
	} else {
		track = new oamlMusicTrack(verbose);
	}

	if (track == NULL) return OAML_ERROR;

	track->SetName(name);

	if (track->IsMusicTrack()) {
		musicTracks.push_back((oamlMusicTrack*)track);
	} else {
		sfxTracks.push_back((oamlSfxTrack*)track);
	}

	return OAML_OK;
}

oamlTrack* oamlBase::GetTrack(std::string name) {
	for (std::vector<oamlMusicTrack*>::iterator it=musicTracks.begin(); it<musicTracks.end(); ++it) {
		oamlMusicTrack *track = *it;
		if (track->GetName().compare(name) == 0) {
			return track;
		}
	}

	for (std::vector<oamlSfxTrack*>::iterator it=sfxTracks.begin(); it<sfxTracks.end(); ++it) {
		oamlSfxTrack *track = *it;
		if (track->GetName().compare(name) == 0) {
			return track;
		}
	}

	return NULL;
}

oamlRC oamlBase::TrackRemove(std::string name) {
	for (std::vector<oamlMusicTrack*>::iterator it=musicTracks.begin(); it<musicTracks.end(); ++it) {
		oamlMusicTrack *track = *it;
		if (track->GetName().compare(name) == 0) {
			musicTracks.erase(it);
			delete track;
			return OAML_OK;
		}
	}

	for (std::vector<oamlSfxTrack*>::iterator it=sfxTracks.begin(); it<sfxTracks.end(); ++it) {
		oamlSfxTrack *track = *it;
		if (track->GetName().compare(name) == 0) {
			sfxTracks.erase(it);
			delete track;
			return OAML_OK;
		}
	}

	return OAML_NOT_FOUND;
}

void oamlBase::TrackRename(std::string name, std::string newName) {
	oamlTrack *track = GetTrack(name);
	if (track == NULL)
		return;

	track->SetName(newName);
}

void oamlBase::TrackSetVolume(std::string name, float volume) {
	oamlTrack *track = GetTrack(name);
	if (track == NULL)
		return;

	track->SetVolume(volume);
}

void oamlBase::TrackSetFadeIn(std::string name, int fadeIn) {
	oamlTrack *track = GetTrack(name);
	if (track == NULL)
		return;

	track->SetFadeIn(fadeIn);
}

void oamlBase::TrackSetFadeOut(std::string name, int fadeOut) {
	oamlTrack *track = GetTrack(name);
	if (track == NULL)
		return;

	track->SetFadeOut(fadeOut);
}

void oamlBase::TrackSetXFadeIn(std::string name, int xFadeIn) {
	oamlTrack *track = GetTrack(name);
	if (track == NULL)
		return;

	track->SetXFadeIn(xFadeIn);
}

void oamlBase::TrackSetXFadeOut(std::string name, int xFadeOut) {
	oamlTrack *track = GetTrack(name);
	if (track == NULL)
		return;

	track->SetXFadeOut(xFadeOut);
}

bool oamlBase::TrackExists(std::string name) {
	oamlTrack *track = GetTrack(name);
	return track != NULL;
}

bool oamlBase::TrackIsSfxTrack(std::string name) {
	oamlTrack *track = GetTrack(name);
	if (track == NULL)
		return false;

	return track->IsSfxTrack();
}

bool oamlBase::TrackIsMusicTrack(std::string name) {
	oamlTrack *track = GetTrack(name);
	if (track == NULL)
		return false;

	return track->IsMusicTrack();
}

void oamlBase::TrackGetAudioList(std::string name, std::vector<std::string>& list) {
	oamlTrack *track = GetTrack(name);
	if (track == NULL)
		return;

	return track->GetAudioList(list);
}

float oamlBase::TrackGetVolume(std::string name) {
	oamlTrack *track = GetTrack(name);
	if (track == NULL)
		return 1.0;

	return track->GetVolume();
}

int oamlBase::TrackGetFadeIn(std::string name) {
	oamlTrack *track = GetTrack(name);
	if (track == NULL)
		return 0;

	return track->GetFadeIn();
}

int oamlBase::TrackGetFadeOut(std::string name) {
	oamlTrack *track = GetTrack(name);
	if (track == NULL)
		return 0;

	return track->GetFadeOut();
}

int oamlBase::TrackGetXFadeIn(std::string name) {
	oamlTrack *track = GetTrack(name);
	if (track == NULL)
		return 0;

	return track->GetXFadeIn();
}

int oamlBase::TrackGetXFadeOut(std::string name) {
	oamlTrack *track = GetTrack(name);
	if (track == NULL)
		return 0;

	return track->GetXFadeOut();
}

oamlRC oamlBase::AudioNew(std::string trackName, std::string audioName, int type) {
	oamlTrack *track = GetTrack(trackName);
	if (track == NULL)
		return OAML_NOT_FOUND;

	oamlAudio *audio = new oamlAudio(this, fcbs, verbose);
	if (audio == NULL)
		return OAML_ERROR;

	audio->SetType(type);
	audio->SetName(audioName);
	audio->SetBPM(bpm);
	audio->SetBeatsPerBar(beatsPerBar);
	track->AddAudio(audio);

	return OAML_OK;
}

oamlAudio* oamlBase::GetAudio(std::string trackName, std::string audioName) {
	oamlTrack *track = GetTrack(trackName);
	if (track == NULL)
		return NULL;

	return track->GetAudio(audioName);
}

oamlRC oamlBase::AudioRemove(std::string trackName, std::string audioName) {
	oamlTrack *track = GetTrack(trackName);
	if (track == NULL)
		return OAML_NOT_FOUND;

	return track->RemoveAudio(audioName);
}

void oamlBase::AudioAddAudioFile(std::string trackName, std::string audioName, std::string filename) {
	oamlAudio *audio = GetAudio(trackName, audioName);
	if (audio == NULL)
		return;

	audio->AddAudioFile(filename);
}

void oamlBase::AudioSetName(std::string trackName, std::string audioName, std::string name) {
	oamlAudio *audio = GetAudio(trackName, audioName);
	if (audio == NULL)
		return;

	audio->SetName(name);
}

void oamlBase::AudioSetVolume(std::string trackName, std::string audioName, float volume) {
	oamlAudio *audio = GetAudio(trackName, audioName);
	if (audio == NULL)
		return;

	audio->SetVolume(volume);
}

void oamlBase::AudioSetBPM(std::string trackName, std::string audioName, float bpm) {
	oamlAudio *audio = GetAudio(trackName, audioName);
	if (audio == NULL)
		return;

	audio->SetBPM(bpm);
}

void oamlBase::AudioSetBeatsPerBar(std::string trackName, std::string audioName, int beatsPerBar) {
	oamlAudio *audio = GetAudio(trackName, audioName);
	if (audio == NULL)
		return;

	audio->SetBeatsPerBar(beatsPerBar);
}

void oamlBase::AudioSetBars(std::string trackName, std::string audioName, int bars) {
	oamlAudio *audio = GetAudio(trackName, audioName);
	if (audio == NULL)
		return;

	audio->SetBars(bars);
}

void oamlBase::AudioSetMinMovementBars(std::string trackName, std::string audioName, int minMovementBars) {
	oamlAudio *audio = GetAudio(trackName, audioName);
	if (audio == NULL)
		return;

	audio->SetMinMovementBars(minMovementBars);
}

void oamlBase::AudioSetRandomChance(std::string trackName, std::string audioName, int randomChance) {
	oamlAudio *audio = GetAudio(trackName, audioName);
	if (audio == NULL)
		return;

	audio->SetRandomChance(randomChance);
}

void oamlBase::AudioSetFadeIn(std::string trackName, std::string audioName, int fadeIn) {
	oamlAudio *audio = GetAudio(trackName, audioName);
	if (audio == NULL)
		return;

	audio->SetFadeIn(fadeIn);
}

void oamlBase::AudioSetFadeOut(std::string trackName, std::string audioName, int fadeOut) {
	oamlAudio *audio = GetAudio(trackName, audioName);
	if (audio == NULL)
		return;

	audio->SetFadeOut(fadeOut);
}

void oamlBase::AudioSetXFadeIn(std::string trackName, std::string audioName, int xFadeIn) {
	oamlAudio *audio = GetAudio(trackName, audioName);
	if (audio == NULL)
		return;

	audio->SetXFadeIn(xFadeIn);
}

void oamlBase::AudioSetXFadeOut(std::string trackName, std::string audioName, int xFadeOut) {
	oamlAudio *audio = GetAudio(trackName, audioName);
	if (audio == NULL)
		return;

	audio->SetXFadeOut(xFadeOut);
}

void oamlBase::AudioSetCondId(std::string trackName, std::string audioName, int condId) {
	oamlAudio *audio = GetAudio(trackName, audioName);
	if (audio == NULL)
		return;

	audio->SetCondId(condId);
}

void oamlBase::AudioSetCondType(std::string trackName, std::string audioName, int condType) {
	oamlAudio *audio = GetAudio(trackName, audioName);
	if (audio == NULL)
		return;

	audio->SetCondType(condType);
}

void oamlBase::AudioSetCondValue(std::string trackName, std::string audioName, int condValue) {
	oamlAudio *audio = GetAudio(trackName, audioName);
	if (audio == NULL)
		return;

	audio->SetCondValue(condValue);
}

void oamlBase::AudioSetCondValue2(std::string trackName, std::string audioName, int condValue2) {
	oamlAudio *audio = GetAudio(trackName, audioName);
	if (audio == NULL)
		return;

	audio->SetCondValue2(condValue2);
}

bool oamlBase::AudioExists(std::string trackName, std::string audioName) {
	oamlAudio *audio = GetAudio(trackName, audioName);
	return audio != NULL;
}

void oamlBase::AudioGetAudioFileList(std::string trackName, std::string audioName, std::vector<std::string>& list) {
	oamlAudio *audio = GetAudio(trackName, audioName);
	if (audio == NULL)
		return;

	return audio->GetAudioFileList(list);
}

int oamlBase::AudioGetType(std::string trackName, std::string audioName) {
	oamlAudio *audio = GetAudio(trackName, audioName);
	if (audio == NULL)
		return 0;

	return audio->GetType();
}

float oamlBase::AudioGetVolume(std::string trackName, std::string audioName) {
	oamlAudio *audio = GetAudio(trackName, audioName);
	if (audio == NULL)
		return 1.f;

	return audio->GetVolume();
}

float oamlBase::AudioGetBPM(std::string trackName, std::string audioName) {
	oamlAudio *audio = GetAudio(trackName, audioName);
	if (audio == NULL)
		return 0.f;

	return audio->GetBPM();
}

int oamlBase::AudioGetBeatsPerBar(std::string trackName, std::string audioName) {
	oamlAudio *audio = GetAudio(trackName, audioName);
	if (audio == NULL)
		return 0;

	return audio->GetBeatsPerBar();
}

int oamlBase::AudioGetBars(std::string trackName, std::string audioName) {
	oamlAudio *audio = GetAudio(trackName, audioName);
	if (audio == NULL)
		return 0;

	return audio->GetBars();
}

int oamlBase::AudioGetMinMovementBars(std::string trackName, std::string audioName) {
	oamlAudio *audio = GetAudio(trackName, audioName);
	if (audio == NULL)
		return 0;

	return audio->GetMinMovementBars();
}

int oamlBase::AudioGetRandomChance(std::string trackName, std::string audioName) {
	oamlAudio *audio = GetAudio(trackName, audioName);
	if (audio == NULL)
		return 0;

	return audio->GetRandomChance();
}

int oamlBase::AudioGetFadeIn(std::string trackName, std::string audioName) {
	oamlAudio *audio = GetAudio(trackName, audioName);
	if (audio == NULL)
		return 0;

	return audio->GetFadeIn();
}

int oamlBase::AudioGetFadeOut(std::string trackName, std::string audioName) {
	oamlAudio *audio = GetAudio(trackName, audioName);
	if (audio == NULL)
		return 0;

	return audio->GetFadeOut();
}

int oamlBase::AudioGetXFadeIn(std::string trackName, std::string audioName) {
	oamlAudio *audio = GetAudio(trackName, audioName);
	if (audio == NULL)
		return 0;

	return audio->GetXFadeIn();
}

int oamlBase::AudioGetXFadeOut(std::string trackName, std::string audioName) {
	oamlAudio *audio = GetAudio(trackName, audioName);
	if (audio == NULL)
		return 0;

	return audio->GetXFadeOut();
}

int oamlBase::AudioGetCondId(std::string trackName, std::string audioName) {
	oamlAudio *audio = GetAudio(trackName, audioName);
	if (audio == NULL)
		return 0;

	return audio->GetCondId();
}

int oamlBase::AudioGetCondType(std::string trackName, std::string audioName) {
	oamlAudio *audio = GetAudio(trackName, audioName);
	if (audio == NULL)
		return 0;

	return audio->GetCondType();
}

int oamlBase::AudioGetCondValue(std::string trackName, std::string audioName) {
	oamlAudio *audio = GetAudio(trackName, audioName);
	if (audio == NULL)
		return 0;

	return audio->GetCondValue();
}

int oamlBase::AudioGetCondValue2(std::string trackName, std::string audioName) {
	oamlAudio *audio = GetAudio(trackName, audioName);
	if (audio == NULL)
		return 0;

	return audio->GetCondValue2();
}

oamlAudioFile* oamlBase::GetAudioFile(std::string trackName, std::string audioName, std::string filename) {
	oamlAudio *audio = GetAudio(trackName, audioName);
	if (audio == NULL)
		return NULL;

	return audio->GetAudioFile(filename);
}

void oamlBase::AudioFileRemove(std::string trackName, std::string audioName, std::string filename) {
	oamlAudio *audio = GetAudio(trackName, audioName);
	if (audio == NULL)
		return;

	audio->RemoveAudioFile(filename);
}

void oamlBase::AudioFileSetLayer(std::string trackName, std::string audioName, std::string filename, std::string layer) {
	oamlAudioFile *file = GetAudioFile(trackName, audioName, filename);
	if (file == NULL)
		return;

	file->SetLayer(layer);
}

void oamlBase::AudioFileSetRandomChance(std::string trackName, std::string audioName, std::string filename, int randomChance) {
	oamlAudioFile *file = GetAudioFile(trackName, audioName, filename);
	if (file == NULL)
		return;

	file->SetRandomChance(randomChance);
}

std::string oamlBase::AudioFileGetLayer(std::string trackName, std::string audioName, std::string filename) {
	oamlAudioFile *file = GetAudioFile(trackName, audioName, filename);
	if (file == NULL)
		return "";

	return file->GetLayer();
}

int oamlBase::AudioFileGetRandomChance(std::string trackName, std::string audioName, std::string filename) {
	oamlAudioFile *file = GetAudioFile(trackName, audioName, filename);
	if (file == NULL)
		return 0;

	return file->GetRandomChance();
}

oamlRC oamlBase::LayerNew(std::string name) {
	AddLayer(name);

	return OAML_OK;
}

void oamlBase::LayerList(std::vector<std::string>& list) {
	for (std::vector<oamlLayer*>::iterator it=layers.begin(); it<layers.end(); ++it) {
		oamlLayer *layer = *it;
		list.push_back(layer->GetName());
	}
}

void oamlBase::LayerRename(std::string layerName, std::string name) {
	oamlLayer *layer = GetLayer(layerName);
	if (layer == NULL)
		return;

	layer->SetName(name);
}

int oamlBase::LayerGetId(std::string layerName) {
	oamlLayer *layer = GetLayer(layerName);
	if (layer == NULL)
		return -1;

	return layer->GetId();
}

int oamlBase::LayerGetRandomChance(std::string layerName) {
	oamlLayer *layer = GetLayer(layerName);
	if (layer == NULL)
		return 100;

	return layer->GetRandomChance();
}

float oamlBase::LayerGetGain(std::string layerName) {
	oamlLayer *layer = GetLayer(layerName);
	if (layer == NULL)
		return 1.f;

	return layer->GetGain();
}
