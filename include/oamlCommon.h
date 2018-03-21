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

#ifndef __OAMLCOMMON_H__
#define __OAMLCOMMON_H__

#include <assert.h>

//
// Definitions
//

#ifdef _WIN32
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif


// Visual Studio specific stuff
#ifdef _MSC_VER

#define snprintf	sprintf_s

#endif

#ifdef _MSC_VER
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif


#ifdef DEBUG

#ifdef _MSC_VER
#define ASSERT(e)
#else
#define ASSERT(e)  \
    ((void) ((e) ? ((void)0) : __assert (#e, __FILE__, __LINE__)))
#endif

#else

#define ASSERT(e)

#endif

#include "oaml.h"
#include "gettime.h"
#include "ByteBuffer.h"
#include "audioFile.h"
#include "aif.h"
#ifdef __HAVE_OGG
#include "ogg.h"
#endif
#include "tinyxml2.h"
#include "wav.h"
#include "oamlLayer.h"
#include "oamlAudioFile.h"
#include "oamlAudio.h"
#include "oamlTrack.h"
#include "oamlMusicTrack.h"
#include "oamlSfxTrack.h"
#include "oamlCompressor.h"
#include "oamlBase.h"
#include "oamlUtil.h"


#endif /* __OAMLCOMMON_H__ */
