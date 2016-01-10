#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "tinyxml2.h"
#include "oamlCommon.h"


oamlBase::oamlBase() {
	debugClipping = false;
	writeAudioAtShutdown = false;
	measureDecibels = false;
	avgDecibels = 0;
	tracksN = 0;

	curTrack = NULL;

	fullBuffer = NULL;

	freq = 0;
	channels = 0;
	bytesPerSample = 0;

	volume = OAML_VOLUME_DEFAULT;

	timeMs = 0;
	tension = 0;
	tensionMs = 0;
}

oamlBase::~oamlBase() {
	if (fullBuffer) {
		delete fullBuffer;
		fullBuffer = NULL;
	}
}

int oamlBase::ReadDefs(const char *filename, const char *path) {
	tinyxml2::XMLDocument doc;

	if (doc.LoadFile(filename) != tinyxml2::XML_NO_ERROR)
		return -1;

	tinyxml2::XMLElement *el = doc.FirstChildElement("track");
	while (el != NULL) {
		oamlTrack *track = new oamlTrack();

		tinyxml2::XMLElement *trackEl = el->FirstChildElement();
		while (trackEl != NULL) {
			if (strcmp(trackEl->Name(), "name") == 0) track->SetName(trackEl->GetText());
			else if (strcmp(trackEl->Name(), "fadeIn") == 0) track->SetFadeIn(strtol(trackEl->GetText(), NULL, 0));
			else if (strcmp(trackEl->Name(), "fadeOut") == 0) track->SetFadeOut(strtol(trackEl->GetText(), NULL, 0));
			else if (strcmp(trackEl->Name(), "xfadeIn") == 0) track->SetXFadeIn(strtol(trackEl->GetText(), NULL, 0));
			else if (strcmp(trackEl->Name(), "xfadeOut") == 0) track->SetXFadeOut(strtol(trackEl->GetText(), NULL, 0));
			else if (strcmp(trackEl->Name(), "audio") == 0) {
				oamlAudio *audio = new oamlAudio();

				tinyxml2::XMLElement *audioEl = trackEl->FirstChildElement();
				while (audioEl != NULL) {
					if (strcmp(audioEl->Name(), "filename") == 0) {
						char fname[1024];
						snprintf(fname, 1024, "%s%s", path, audioEl->GetText());
						audio->SetFilename(fname);
					}
					else if (strcmp(audioEl->Name(), "type") == 0) audio->SetType(strtol(audioEl->GetText(), NULL, 0));
					else if (strcmp(audioEl->Name(), "bars") == 0) audio->SetBars(strtol(audioEl->GetText(), NULL, 0));
					else if (strcmp(audioEl->Name(), "bpm") == 0) audio->SetBPM((float)atof(audioEl->GetText()));
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
			} else {
				printf("%s: Unknown track tag: %s\n", __FUNCTION__, trackEl->Name());
			}

			trackEl = trackEl->NextSiblingElement();
		}

		tracks[tracksN++] = track;
//		track->ShowInfo();

		el = el->NextSiblingElement();
	}

	return 0;
}

void oamlBase::ReadInternalDefs(const char *filename) {
	tinyxml2::XMLDocument doc;

	if (doc.LoadFile(filename) != tinyxml2::XML_NO_ERROR)
		return;

	tinyxml2::XMLElement *el = doc.FirstChildElement("base");
	while (el != NULL) {
		tinyxml2::XMLElement *cel = el->FirstChildElement();
		while (cel != NULL) {
			if (strcmp(cel->Name(), "writeAudioAtShutdown") == 0) SetWriteAudioAtShutdown(strtol(cel->GetText(), NULL, 0));
			else if (strcmp(cel->Name(), "debugClipping") == 0) SetDebugClipping(strtol(cel->GetText(), NULL, 0));
			else if (strcmp(cel->Name(), "measureDecibels") == 0) SetMeasureDecibels(strtol(cel->GetText(), NULL, 0));

			cel = cel->NextSiblingElement();
		}

		el = el->NextSiblingElement();
	}
}

int oamlBase::Init(const char *pathToMusic) {
	char path[1024];
	char filename[1024];

	ASSERT(pathToMusic != NULL);

	int len = strlen(pathToMusic);
	if (len > 0 && (pathToMusic[len-1] == '/')) {
		snprintf(path, 1024, "%s", pathToMusic);
	} else {
		snprintf(path, 1024, "%s/", pathToMusic);
	}

	snprintf(filename, 1024, "%soaml.defs", path);
	if (ReadDefs(filename, path) == -1)
		return -1;

	ReadInternalDefs("oamlInternal.defs");

	return 0;
}

void oamlBase::SetAudioFormat(int audioFreq, int audioChannels, int audioBytesPerSample) {
	freq = audioFreq;
	channels = audioChannels;
	bytesPerSample = audioBytesPerSample;
}

int oamlBase::PlayTrackId(int id) {
	if (id >= tracksN)
		return -1;

	if (curTrack) {
		curTrack->Stop();
	}

	curTrack = tracks[id];
	curTrack->Play();

	return 0;
}

int oamlBase::PlayTrack(const char *name) {
	ASSERT(name != NULL);

//	printf("%s %s\n", __FUNCTION__, name);

	for (int i=0; i<tracksN; i++) {
		if (strcmp(tracks[i]->GetName(), name) == 0) {
			return PlayTrackId(i);
		}
	}

	fprintf(stderr, "liboaml: Unable to find track name '%s'\n", name);

	return -1;
}

bool oamlBase::IsTrackPlaying(const char *name) {
	ASSERT(name != NULL);

	for (int i=0; i<tracksN; i++) {
		if (strcmp(tracks[i]->GetName(), name) == 0) {
			return IsTrackPlayingId(i);
		}
	}

	return false;
}

bool oamlBase::IsTrackPlayingId(int id) {
	if (id >= tracksN)
		return false;

	return tracks[id]->IsPlaying();
}

bool oamlBase::IsPlaying() {
	for (int i=0; i<tracksN; i++) {
		if (tracks[i]->IsPlaying())
			return true;
	}

	return false;
}

void oamlBase::StopPlaying() {
	for (int i=0; i<tracksN; i++) {
		if (tracks[i]->IsPlaying()) {
			tracks[i]->Stop();
			break;
		}
	}
}

void oamlBase::ShowPlayingTracks() {
	for (int i=0; i<tracksN; i++) {
		tracks[i]->ShowPlaying();
	}
}

int oamlBase::SafeAdd(int sample1, int sample2) {
	bool clipping;
	int ret;

	// Detect integer overflow and underflow, both will cause clipping in our audio
	if (sample1 > 0 && sample2 > INT_MAX - sample1) {
		int64_t s64a = sample1;
		int64_t s64b = sample2;
		ret = (int)INT_MAX - ((s64a + s64b) - INT_MAX);
		clipping = true;
	} else if (sample1 < 0 && sample2 < INT_MIN - sample1) {
		int64_t s64a = sample1;
		int64_t s64b = sample2;
		ret = (int)INT_MIN - ((s64a + s64b) - INT_MIN);
		clipping = true;
	} else {
		ret = sample1 + sample2;
		clipping = false;
	}

	if (clipping && debugClipping) {
		fprintf(stderr, "oaml: Detected clipping!\n");
	}

	return ret;
}

void oamlBase::MixToBuffer(void *buffer, int size) {
	double sd[2] = { 0, 0 };
	double sum[2] = { 0, 0 };
	int16_t *sbuf = (int16_t *)buffer;
	uint8_t *cbuf = (uint8_t *)buffer;
	int chcount = 0;

	ASSERT(buffer != NULL);
	ASSERT(size != 0);

	for (int i=0; i<size; i++) {
		int sample = 0;

		// Mix all the tracks into a 32bit temp value
		for (int j=0; j<tracksN; j++) {
			sample = SafeAdd(sample, tracks[j]->Read32());
		}

		// Apply the volume
		sample = (((sample>>8) * volume) / OAML_VOLUME_MAX) << 8;

		// Mix our sample into the buffer
		int tmp;
		switch (bytesPerSample) {
			case 1: // 8bit (unsigned)
				// TODO: Test me!
				tmp = SafeAdd(sample, (int)cbuf[i]<<23);
				cbuf[i] = (uint8_t)(tmp>>23);
				break;

			case 2: // 16bit (signed)
				tmp = SafeAdd(sample, (int)sbuf[i]<<16);
				sbuf[i] = (int16_t)(tmp>>16);
				break;

			case 3: // 24bit
				// TODO: Test me!
				tmp = (int)cbuf[i*3+0]<<8;
				tmp|= (int)cbuf[i*3+1]<<16;
				tmp|= (int)cbuf[i*3+2]<<24;

				tmp = SafeAdd(sample, tmp);
				cbuf[i*3+0] = (uint8_t)(tmp>>8);
				cbuf[i*3+1] = (uint8_t)(tmp>>16);
				cbuf[i*3+2] = (uint8_t)(tmp>>24);
				break;
		}
		sample = tmp;

		if (measureDecibels) {
			sd[chcount] = (sample >> 16) / 32768.0;
			sum[chcount]+= fabs(sd[chcount]);

			if (++chcount >= channels) {
				chcount = 0;
			}
		}

		if (writeAudioAtShutdown) {
			if (fullBuffer == NULL) {
				fullBuffer = new ByteBuffer();
			}

			fullBuffer->putShort(sample >> 16);
		}
	}

	if (measureDecibels) {
		for (int i=0; i<channels; i++) {
			double rms = sqrt(sum[i] / (size / channels));
			double decibels = 20 * log10(rms);

			avgDecibels = (avgDecibels + decibels) / 2;
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

void oamlBase::SetVolume(int vol) {
	volume = vol;

	if (volume < OAML_VOLUME_MIN) volume = OAML_VOLUME_MIN;
	if (volume > OAML_VOLUME_MAX) volume = OAML_VOLUME_MAX;
}

void oamlBase::AddTension(int value) {
	tension+= value;
	if (tension >= 100) {
		tension = 100;
	}
}

void oamlBase::Update() {
	uint64_t ms = GetTimeMs64();

	// Update each second
	if (ms >= (timeMs + 1000)) {
		ShowPlayingTracks();

//		printf("%s %d %lld %d\n", __FUNCTION__, tension, tensionMs - ms, ms >= (tensionMs + 5000));
		// Don't allow sudden changes of tension after it changed back to 0
		if (tension > 0) {
			SetCondition(1, tension);
			tensionMs = ms;
		} else {
			if (ms >= (tensionMs + 5000)) {
				SetCondition(1, tension);
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

		if (measureDecibels) {
			printf("Measured decibels: %.2g db\n", avgDecibels);
			avgDecibels = 0;
		}

		timeMs = ms;
	}

/*	if (buffer <= 4096) {
		track->Read(buffer);
	}*/
}

void oamlBase::Shutdown() {
//	printf("%s\n", __FUNCTION__);
	if (writeAudioAtShutdown && fullBuffer) {
		char filename[1024];
		snprintf(filename, 1024, "oaml-%d.wav", (int)time(NULL));
		wavFile *wav = new wavFile();
		wav->WriteToFile(filename, fullBuffer, channels, freq, 2);
		delete wav;
	}
}
