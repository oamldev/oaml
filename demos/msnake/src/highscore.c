#include "highscore.h"

// calculate the highscore out of the points and the time needed
int calculate_score(int points, long time_sec) {
  // 10 to highscore for every point you make -2 for every second you need
  return points * 10 - time_sec * 2;
}

char* user_home;
char* hscore_file_path;
size_t hscore_path_len;
char* hscore_file;

#ifdef _WIN32
#include <windows.h>
int msnake_mkdir(char * dirname) {
    return _mkdir(dirname);
}
#else
#include <sys/types.h>
#include <sys/stat.h>
int msnake_mkdir(char * dirname) {
    return mkdir(dirname, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
}
#endif

static char *highscore_file = NULL;
char *get_highscore_file() {
    if(highscore_file == NULL) {
        // get the highscore file name
        char *user_home = getenv("HOME");
        if(user_home == NULL) {
            // if no $HOME variable is available we probably are on windows
            user_home = getenv("APPDATA");
            if(user_home == NULL) {
                // if that still didn't work, simply use the current directory
                user_home = ".";
            }
        }

        int highscore_directory_path_length = strlen(user_home) + 1 + strlen(HIGHSCORE_DIR);
        int highscore_path_length = highscore_directory_path_length + 1 + strlen(HIGHSCORE_FILE) + 1;

        highscore_file = malloc(highscore_path_length);
        strcpy(highscore_file, user_home);
        strcat(highscore_file, "/");
        strcat(highscore_file, HIGHSCORE_DIR);

        // make sure the directory exists
        msnake_mkdir(highscore_file);
        strcat(highscore_file, "/");
        strcat(highscore_file, HIGHSCORE_FILE);
    }
    return highscore_file;
}


// read the highscore file
HIGHSCORE *read_highscore(int *num) {
  // static pointer to an array of highscore records
  static HIGHSCORE *ptr = NULL;
  // the highscore file handle
  FILE *hs_file;
  int x, swapped;
  // the current record
  HIGHSCORE current;

  // if a NULL pointer is given, free all the allocated memory and exit
  if(num == NULL) {
    free(ptr);
    ptr = NULL;
    free(highscore_file);
    highscore_file = NULL;
    return NULL;
  }

  // set num to 0 in case the highscore file cannot be opened
  *num = 0;

  // open the highscore file
  if((hs_file = fopen(get_highscore_file(), "r")) == NULL) {
    return NULL;
  }

  // free the allocated memory of calls before
  if(ptr != NULL) {
    free(ptr);
  }

  // initialize ptr
  ptr = malloc(*num);

  // read the records into the `ptr`-array
  while(fread(&current, sizeof(HIGHSCORE), 1, hs_file) != 0) {
    // next record
    (*num)++;
    // allocate memory for the new record
    ptr = realloc(ptr, sizeof(HIGHSCORE) * *num);
    // save the new record in the array
    ptr[*num - 1] = current;
  }

  // sort the whole thing using a simple bubble sort algorithm
  do {
    // if this variable contains 0 at the end of the following for loop the
    // array is properly sorted
    swapped = 0;
    // iterate througth each record except the last
    for(x = 0; x < *num - 1; x++) {
      // check if the next record's value is greater than the value of the current one
      if(ptr[x].highscore < ptr[x + 1].highscore) {
        // swap the position of the two elements
        current = ptr[x];
        ptr[x] = ptr[x + 1];
        ptr[x + 1] = current;
        // the array isn't properly sorted 
        swapped = 1;
      }
    }
  } while (swapped);

  // return the array
  return ptr;
}

// add an entry to the highscore file
int add_highscore(char *name, int points, int time_sec) {
  FILE *hs_file = NULL;
  HIGHSCORE highscore;

  // write the values into the struct
  strncpy(highscore.name, name, HIGHSCORE_NAME_LENGTH - 1);
  highscore.name[HIGHSCORE_NAME_LENGTH - 1] = '\0'; // make sure the string is ended properly
  highscore.points = points;
  highscore.time_sec = time_sec;
  highscore.highscore = calculate_score(points, time_sec);

  // open the highscore file in append mode
  if((hs_file = fopen(get_highscore_file(), "a")) == NULL) {
    return 1;
  }
  // append a structure to the file
  fwrite(&highscore, sizeof(HIGHSCORE), 1, hs_file);
  // close the file
  fclose(hs_file);
  return 0;
}

// clear the highscore file
void clear_highscore() {
  FILE *hs_file = NULL;
  // open the file in write mode so it gets truncated
  if((hs_file = fopen(get_highscore_file(), "w")) != NULL) {
    fclose(hs_file);
  }
}
