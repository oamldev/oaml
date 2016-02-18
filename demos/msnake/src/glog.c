#include "glog.h"

// a simple log function, works similar to printf
void glog(const char *format, ... ) {
  va_list args; // argument list
  static FILE *logfile = NULL; // file pointer to the logfile
  char *fformat; // the modified format of the string which will be written to the logfile
  int length; // length of the format

  if(!(format == NULL && logfile == NULL)) {
    // open the logfile if not already opened
    if(logfile == NULL) {
      logfile = fopen(LOG_FILE, "w");
      // if that doesn't work exit with an error message
      if(logfile == NULL) {
        fprintf(stderr, "Cannot open logfile %s\n", LOG_FILE);
        exit(EXIT_FAILURE);
      }
    }

    // if NULL is given as format, close the opened file
    if(format != NULL) {
      // increase length by 2 (for \n\0
      length = strlen(format) + 2;
      // allocate memory
      fformat = malloc(sizeof(char) * length);
      // copy the format over
      strncpy(fformat, format, length - 2);
      // append \n\0
      fformat[length - 2] = '\n';
      fformat[length - 1] = '\0';

      // get the rest of the arguments
      va_start(args, format);

      // use vfprintf() to 
      vfprintf(logfile, fformat, args); 
      // forces the logmessage to be written into the file right now
      fflush(logfile);
     
      va_end(args);

      // free the allocated memory for the format string
      free(fformat);
    } else {
      // close the logfile
      fclose(logfile);
    }
  }
}
