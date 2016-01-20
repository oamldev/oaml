#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vorbis/codec.h"
#include "vorbis/vorbisfile.h"

#include "oamlCommon.h"


audioFile::audioFile(oamlFileCallbacks *cbs) {
	fcbs = cbs;
}

audioFile::~audioFile() {
}
