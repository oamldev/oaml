#ifndef __OAMLCOMMON_H__
#define __OAMLCOMMON_H__

#include <assert.h>

//
// Definitions
//


// Visual Studio specific stuff
#ifdef _MSC_VER

#define snprintf	sprintf_s

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

#include "gettime.h"
#include "ByteBuffer.h"
#include "audioFile.h"
#include "aif.h"
#include "ogg.h"
#include "wav.h"
#include "oaml.h"
#include "oamlAudio.h"
#include "oamlTrack.h"
#include "oamlBase.h"


#endif /* __OAMLCOMMON_H__ */
