#include <SDL/SDL.h>
#include <oaml.h>

#include "main.h"
#include "dialog.h"
#include "highscore.h"
#include "game.h"

void audioCallback(void* userdata, Uint8* stream, int len) {
  oamlMixToBuffer(stream, len/2);
}

int open_sdl() {
  SDL_AudioSpec spec;

  if (SDL_Init(SDL_INIT_AUDIO) < 0) {
    fprintf(stderr, "Could not initialize SDL.\n");
    return -1;
  }

  SDL_memset(&spec, 0, sizeof(spec));
  spec.freq = 44100;
  spec.format = AUDIO_S16;
  spec.channels = 2;
  spec.samples = 4096;
  spec.callback = audioCallback;

  if (SDL_OpenAudio(&spec, NULL) < 0) {
    fprintf(stderr, "Failed to open audio: %s\n", SDL_GetError());
    return -1;
  }

  oamlSetAudioFormat(44100, 2, 2, false);

  SDL_PauseAudio(0);

  return 0;
}

int main() {

  open_sdl();
  oamlInit("oaml.defs");

  // for some better random numbers (and not always the same)
  srand(time(NULL));

  init_curses();

  main_menu();

  oamlShutdown();
  end_curses();

  // free the allocated memory for the highscore
  read_highscore(NULL);
  // close the logfile
  glog(NULL);
  return EXIT_SUCCESS;
}

void main_menu() {
  int selected_menu_entry;
  do {
    selected_menu_entry = display_menu();
    if(selected_menu_entry == 1) {
      // run the game
      run();
    } else if(selected_menu_entry == 2) {
      // display the highscores
      show_highscores();
    } else if(selected_menu_entry == 3) {
      // display a dialog which explains the controls of the game
      display_controls();
    } else if(selected_menu_entry == 4) {
      // display a dialog which explains the elements of the game
      display_help();
    } else if(selected_menu_entry == 5) {
      // clear highscores
      if(clear_score_dialog() == 1) {
          clear_highscore();
      }
    }
    // leave if the menu entry "exit" is chosen
  } while(selected_menu_entry != 6);
}

void init_curses() {

#if _BSD_SOURCE || _POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600
  // by default, ncurses stops the execution of the program for
  // 1 second if escape is pressed. Disable this behaviour.
  setenv("ESCDELAY", "0", 1);
#endif
  
  initscr();
  // get more control over the input
  cbreak();
  // getch() returns ERR if no input is present and doesn't wait
  nodelay(stdscr, TRUE);
  // don't echo the inserted keys to the screen
  noecho();
  // colors!
  start_color();
  set_colors();
  // also grab keys like F1 etc.
  keypad(stdscr, TRUE);
}

void set_colors() {
  // these don't actually have anything to do with binary, so we are free to use "normal" numbers
  init_pair(1, COLOR_RED, COLOR_BLACK); // red on black
  init_pair(2, COLOR_GREEN, COLOR_BLACK); // green on black
  init_pair(3, COLOR_YELLOW, COLOR_BLACK); // yellow on black
  init_pair(4, COLOR_BLUE, COLOR_BLACK); // blue on black
  init_pair(5, COLOR_MAGENTA, COLOR_BLACK); // magenta on black
  init_pair(6, COLOR_CYAN, COLOR_BLACK); // cyan on black
  init_pair(7, COLOR_WHITE, COLOR_BLACK); // white on black
}

void end_curses() {
  endwin();
}

