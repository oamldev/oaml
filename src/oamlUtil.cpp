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


float __oamlInteger24ToFloat(int i) {
	const float Q = 1.0f / (0x7fffff + 0.5f);
	if (i & 0x800000) i |= ~0xffffff;
	return (i + 0.5f) * Q;
}

int __oamlFloatToInteger24(float f) {
	return ((int)(f * 8388608) & 0x00ffffff);
}

int __oamlRandom(int min, int max) {
	int range = max - min + 1;
	return rand() % range + min;
}

void __oamlLog(const char* fmt, ...) {
	va_list args;

	FILE *log = fopen("oaml.log", "a+");
	if (log == NULL)
		return;

	va_start(args, fmt);
	vfprintf(log, fmt, args);
	va_end(args);

	fclose(log);
}

