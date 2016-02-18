#include "events.h"
#include "snake.h"
#include "fruits.h"

// check if the snake collides with a fruit
int check_fruit_collision(GAME* game, int cury, int curx) {
  // calculate the range, limit the range to a maximum of 5
  int range = (game->snake.eat_range >= 5 ? 5 : game->snake.eat_range) - 1;
  // calculate the start position
  int startx = curx - range;
  int starty = cury - range;
  // calculate the end position
  int endx = startx + range * 2 + 1;
  int endy = starty + range * 2 + 1;
  int x,y;

  int on = 0; // check if there is food in the calculated range
  // iterate througth every field inside the range
  for(x = startx; x  < endx; x++) {
    for(y = starty; y  < endy; y++) {
      // is a fruit on the current field?
      if(fruit_is_on(&game->fruits, y, x) != NULL) {
        // exclude the field in the middle
        if(!(curx == x && cury == y)) {
          // execute the handler for this field (to eat the fruit)
          check_fruit_collision_handler(game, y, x);
        }
        on = 1; // found one!
      }
    }
  }
  return on;
}
 
// calls the effect of the fruit
int check_fruit_collision_handler(GAME* game, int cury, int curx) {
  // the the fruit by the given position
  FRUIT *fruit = fruit_is_on(&game->fruits, cury, curx);
  // is one on this position?
  if(fruit != NULL) {
    // execute the effect of the fruit
    fruit->effect(game);

    WINDOW *win = newwin(1, 1, cury, curx);
    wprintw(win, " ");
    wrefresh(win);
    delwin(win);
    // remove the fruit from the game
    kill_fruit_by_ptr(&game->fruits, fruit);
  }
  return 1;
}

// check if the snake collides with a border which looks like this
// #################
// #               #
// #               #
// #               #
// #               #
// #               #
// #################
int check_border_collision(GAME* game, int cury, int curx) {
  // check if the given position has a distance of 1 from the screen border
  return cury <= 0 || curx <= 0 || cury >= game->rows - 1 || curx >= game->columns - 1; 
}

// check if the snake collides with the border which looks like this
// #######   #######
// #               #
// #               #
//                  
// #               #
// #               #
// #######   #######
int check_extended_border_collision(GAME* game, int cury, int curx) {
  int range, rangex1, rangex2, rangey1, rangey2;

  // # calculate the positions where holes in the wall start and end
  //  do that for the top and the bottom (x-axis)
  range = game->columns / 2;
  rangex1 = range * 0.9;
  rangex2 = range * 1.1;
  // do that for the left and the right side (y-axis)
  range = game->rows / 2;
  rangey1 = range * 0.9;
  rangey2 = range * 1.1;
  
  // check if the given position is at the border but not within the holes
  return check_border_collision(game, cury, curx)
    && !((curx >= rangex1 && curx <= rangex2) || (cury >= rangey1 && cury <= rangey2));
}

// ends the game if a collison is present 
int check_border_collision_handler(GAME* game, int cury, int curx) {
  return 0; // code for end of the game
}

// check if the snake is colliding on it's self
// we only need to check if the snake head is colliding
int check_self_collision(GAME* game, int cury, int curx) {
  WINDOW* on;
  // check if the position of the snake head is matching with the position of a snake part
  // exept for the last part (because it will move
  return !((on = snake_part_is_on(&game->snake, cury, curx)) == NULL || on == game->snake.parts[game->snake.length - 1]);
}

// ends the game if a collision is present
int check_self_collision_handler(GAME* game, int cury, int curx) {
  return 0; // code for end of the game
}
