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

