
#ifndef _SNAKE_H

#include "types.h"

void redraw_snake(SNAKE *snake);
void kill_snake(SNAKE *snake);
void grow_snake(SNAKE *snake, int posy, int posx);
int move_snake(GAME *game);
WINDOW *snake_part_is_on(SNAKE *snake, int posy, int posx);

#define _SNAKE_H
#endif /* _SNAKE_H */
