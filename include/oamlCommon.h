#ifndef __OAMLCOMMON_H__
#define __OAMLCOMMON_H__

#include <assert.h>

//
// Definitions
//

#ifdef DEBUG

#define ASSERT(e)  \
    ((void) ((e) ? ((void)0) : __assert (#e, __FILE__, __LINE__)))

#else

#define ASSERT(e)

#endif

#include "gettime.h"
#include "ByteBuffer.h"
#include "wav.h"
#include "oaml.h"
#include "oamlAudio.h"
#include "oamlTrack.h"


#endif /* __OAMLCOMMON_H__ */
