#ifndef _GLOG_H
#define _GLOG_H

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"

#define LOG_FILE "game.log"

void glog(const char *format, ... );

#endif /* _GLOG_H */
