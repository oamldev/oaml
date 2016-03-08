#include <SDL/SDL.h>
#include <oaml.h>

#include "effects.h"
#include "fruits.h"

/* Here you can find some diffrent effect diffrent fruits may have if you eat them */

// ---- normal ----
// char: 'x'
// grow by 1
// increase highscore by 1
// grow 1 new fruit
void normal_effect(GAME *game) {
  grow_fruit(game);
  game->snake.grow++;
  game->highscore++;

  int x, y;
  getbegyx(game->snake.parts[0], y, x);
  SDL_LockAudio();
  oamlPlaySfx2d("eat", x, y, game->columns, game->rows);
  SDL_UnlockAudio();
}

// ---- double grow ----
// char: '@'
// grow by 1
// increase highscore by 1
// grow 2 new fruit
void double_grow(GAME *game) {
  normal_effect(game);
  grow_fruit(game);
}

// ---- mega food ----
// char: '%'
// grow by 5
// increase highscore by 10
// grow 1 new fruit
void mega_food(GAME *game) {
  normal_effect(game);
  game->snake.grow += 4;
  game->highscore += 9;
}

// ---- eat boost ----
// char '&'
// grow by 1
// increase highscore by 1
// grow 1 new fruit
// SPECIAL: the range the snake eats food increases by 1 temporarily
void eat_boost(GAME *game) {
  normal_effect(game);
  game->snake.eat_range++;
}
