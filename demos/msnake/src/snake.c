#include <SDL/SDL.h>
#include <oaml.h>

#include "snake.h"
#include "events.h"

// check if a part of the snake is on the given spot
// returns a WINDOW * if one is found, NULL if not
WINDOW *snake_part_is_on(SNAKE *snake, int posy, int posx) {
  int i;
  int cury, curx; // position of the current snake part we are iterating

  // iterate each part
  for(i = 0; i < snake->length; i++) {
    // get the position of the current part
    getbegyx(snake->parts[i], cury, curx);

    // compare the position
    if(cury == posy && curx == posx) {
      // return the current part
      return snake->parts[i];
    }
  }

  return NULL;
}

// increase the length of the name by one
void grow_snake(SNAKE *snake, int posy, int posx) {
  // a window containing a single char of a snake
  WINDOW *win;

  // increase the length of the snake
  snake->length++;
  // allocate memory for the new part
  if(snake->length == 0) {
    // initialize the array with malloc if it is the first part
    snake->parts = malloc(sizeof(WINDOW*) * snake->length);
  } else {
    // increase the amount of allocated memory
    snake->parts = realloc(snake->parts, sizeof(WINDOW*) * snake->length);
  }
  
  // create a new window
  snake->parts[snake->length - 1] = win = newwin(1, 1, posy, posx);
  // print the character on the window
  wattron(win, COLOR_PAIR(2));
  wprintw(win, "%c", 'O');
  // display the part
  wrefresh(win);

  return;
}

// free the allocated memory for the snake
void kill_snake(SNAKE *snake) {
  int i;
  // delete each created window of the snake
  for(i = 0; i < snake->length; i++) {
    delwin(snake->parts[i]);
  }
  // free the resources allocated for the window pointers
  free(snake->parts);

  return;
}

// move the snake
int move_snake(GAME *game) {
  int success = 1, i;

  // some variables containing positions of certain things
  int curx, cury, tmpy, tmpx;

  // callbacks to check collisions
  int (*collision_checks[EVENTS])(GAME*, int, int) = {
    check_self_collision,
    check_extended_border_collision,
    check_fruit_collision
  };

  // callbacks which will be called automaticly if a collision is detected
  // the callback at position 1 in the array belongs to the check-function
  // at position 1 from the array above
  int (*collision_handlers[EVENTS])(GAME*, int, int) = {
    check_self_collision_handler,
    check_border_collision_handler,
    check_fruit_collision_handler
  };


  // difference on x-axis according to the direction
  int xdiff = game->snake.dir == DIR_LEFT ? -1 : (game->snake.dir == DIR_RIGHT ? 1 : 0);
  // difference on y-axis according to the direction
  int ydiff = game->snake.dir == DIR_UP ? -1 : (game->snake.dir == DIR_DOWN ? 1 : 0);

  // the position of the snake head
  getbegyx(game->snake.parts[0], cury, curx);

  // make a copy
  tmpy = cury;
  tmpx = curx;

  // calculate the new position and prevent from exceeding the size of the screen
  cury = (cury + ydiff) % game->rows;
  curx = (curx + xdiff) % game->columns;
  // the values have to be positive
  cury = cury < 0 ? game->rows + cury : cury;
  curx = curx < 0 ? game->columns + curx : curx;

  // check for collisons and execute the handlers if a collision occured
  for(i = 0; i < EVENTS && success; i++) {
    // collision?
    if((collision_checks[i](game, cury, curx))) {
      // should we end the game because of the collision?
      if(!collision_handlers[i](game, cury, curx)) {
        success = 0;
      }
    }
  }

  // no collisions ?
  if(success) {
    // set the direction of the head
    mvwprintw(game->snake.parts[0], 0, 0, "%c", game->snake.dir);
    // move the window
    mvwin(game->snake.parts[0], cury, curx);

    // copy values back
    cury = tmpy;
    curx = tmpx;

    // iterate through each part of the snake
    for(i = 1; i < game->snake.length; i++) {
      // get the position of the current part
      getbegyx(game->snake.parts[i], tmpy, tmpx);

      // move the part to the position of the previous one
      mvwin(game->snake.parts[i], cury, curx);

      // make a copy
      cury = tmpy;
      curx = tmpx;
    }

    // grow?
    if(game->snake.grow > 0) {
      // grow the snake
      grow_snake(&game->snake, cury, curx);

      // decrease the grow counter (number of times the snake will grow in the future)
      game->snake.grow--;
    } else {
      // is the snake head on the same position as the last part of the snake was before?
      getbegyx(game->snake.parts[0], tmpy, tmpx);
      if(!(tmpy == cury && tmpx == curx)) {
        // if no print a space at this position
        WINDOW *cur = newwin(1, 1, cury, curx);
        wprintw(cur, "%c", ' ');
        wrefresh(cur);
        delwin(cur);
      }
    }

    // redraw the snake on the screen
    redraw_snake(&game->snake);
  } else {
    SDL_LockAudio();
    oamlPlaySfx("lose");
    SDL_UnlockAudio();
  }
  return success;
}

// redraw the whole snake
void redraw_snake(SNAKE *snake) {
  int i;
  // iterate through each part of the snake
  for(i = 0; i < snake->length; i++) {
    // redraw the current part
    redrawwin(snake->parts[i]);
    wrefresh(snake->parts[i]);
  }
}
