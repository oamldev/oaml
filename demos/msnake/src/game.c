#include <SDL/SDL.h>
#include <oaml.h>

#include "game.h"
#include "events.h"
#include "fruits.h"
#include "snake.h"
#include "main.h"
#include "dialog.h"
#include "highscore.h"
#include "status-display.h"
#include "time-helpers.h"

// end the game, free the resources
void kill_game(GAME *game) {
  kill_fruits(&game->fruits);
  kill_snake(&game->snake);
}

void run() {
  int ch = 0, ich, i, current_columns, current_rows, success = 1;

  // some variables for the timer (inclusive the interval)
  struct timespec last_time              = {};
  struct timespec current_time           = {};
  long long default_interval             = 100000000; // 100 ms
  long long min_interval                 = 40000000;  // 40 ms
  long long res;
  char playername[HIGHSCORE_NAME_LENGTH] = {};

  int range_counter = 0;

  // create the game struct
  GAME game = {};

  // initialize interval
  game.interval = default_interval;

  // set the eat range to 1
  game.snake.eat_range = 1;

  // helper variable to keep track of how long we've paused
  time_t pause_start;

  // get the dimensions of the terminal and store them in the GAME struct
  getmaxyx(stdscr, game.rows, game.columns);

  // clear the whole screen
  clear();

  // draw the walls
  draw_border(&game);

  // show the newly created walls
  refresh();

  // place the snake in the middle of the game field
  grow_snake(&game.snake, game.rows / 2, game.columns / 2);
  game.snake.dir = DIR_LEFT;

  // create some fruits on the screen
  // NOM, NOM, NOM
  for(i = 0; i < 50; i++) {
    grow_fruit(&game);
  }
  
  // get the time when the game started
  time(&game.started);
  // get the current time
  current_utc_time(&last_time);

  SDL_LockAudio();
  oamlPlayTrack("Theme");
  oamlSetMainLoopCondition(1);
  SDL_UnlockAudio();

  // start the event loop
  while((ich = getch()) && success) {
    // key typed?
    if(ich == ERR) {
    } else {
      // use this key as a direction
      ch = ich;
    }
    // check if we have an overrun
    current_utc_time(&current_time);

    // calculate the dirrence between the last snake move and the current time
    res = timeval_diff(&last_time, &current_time);

    // is the interval over?
    if(res > game.interval) {
      // has an effect on the eat_range ?
      if(game.snake.eat_range > 1) {
        // every 200th field, decrease the range
        range_counter = (range_counter + 1) % 150;
        // it turns to 0 after the 200th field
        if(range_counter == 0) {
          game.snake.eat_range--; // so, decrease it!
        }
      }
      // new direction? 
      if((ch == KEY_UP || ch == 'w') && game.snake.dir != DIR_DOWN) {
        game.snake.dir = DIR_UP;
      } else if((ch == KEY_LEFT || ch == 'a') && game.snake.dir != DIR_RIGHT) {
        game.snake.dir = DIR_LEFT;
      } else if((ch == KEY_RIGHT || ch == 'd') && game.snake.dir != DIR_LEFT) {
        game.snake.dir = DIR_RIGHT;
      } else if((ch == KEY_DOWN || ch == 's') && game.snake.dir != DIR_UP) {
        game.snake.dir = DIR_DOWN;
      }
      // move the snake
      success = move_snake(&game);

      // refresh the screen
      refresh();

      // display the status bar (top-right)
      status_display(&game);

      SDL_LockAudio();
      oamlSetMainLoopCondition(1 + game.snake.length / 10);
      SDL_UnlockAudio();

      game.interval = default_interval - (default_interval * (game.snake.length / 200.0));
      if (game.interval < min_interval)
        game.interval = min_interval;

      // update the time when we last moved the snake
      last_time = current_time;
    }
    
    getmaxyx(stdscr, current_rows, current_columns);
    // 'p' pressed || size of the terminal changed
    if(ich == 'p' || (current_rows != game.rows || current_columns != game.columns)) {
      // use the terminal new size
      game.rows = current_rows;
      game.columns = current_columns;

      // get the time
      time(&pause_start);

      // show the pause dialog
      switch(pause_dialog()) {
        case 2:
          // leave the game if '2' is pressed
          success = 0;
        default:
          // redraw the screen on resume
          game.paused += time(NULL) - pause_start;
          redraw_game(&game);
          break;
      }
    }
  }

  oamlStopPlaying();

  // get the time when the game has ended
  time(&game.ended);

  // display the highscore dialog & let the player enter his name
  display_highscore(&game, playername, HIGHSCORE_NAME_LENGTH);

  // has a name been entered? if not don't create a highscore entry
  if(playername[0]) {
    add_highscore(playername, game.highscore, game.ended - game.started - game.paused);
  }

  // free all the resources reserved in the game struct
  kill_game(&game);
}

void draw_border(GAME *game) {
  int x, y;
  // create a border
  attron(A_BOLD | COLOR_PAIR(6));
  for(x = 0; x < game->columns; x++) {
    for(y = 0; y < game->rows; y++) {
      if(check_extended_border_collision(game, y, x)) {
        mvprintw(y, x, "#");
      }
    }
  }
}

// redraw the whole screen
void redraw_game(GAME *game) {
  // redraw the main window (containg the border and stuff)
  clear();
  draw_border(game);
  redrawwin(stdscr);
  refresh();
  
  // redraw the fruits
  redraw_fruits(&game->fruits);

  // redraw the snake
  redraw_snake(&game->snake);
}
