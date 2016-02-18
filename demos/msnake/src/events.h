
#ifndef _EVENTS_H
#define _EVENTS_H

#define EVENTS 3

#include "types.h"

int check_fruit_collision(GAME* game, int cury, int curx);
int check_fruit_collision_handler(GAME* game, int cury, int curx);

int check_border_collision(GAME* game, int cury, int curx);
int check_border_collision_handler(GAME* game, int cury, int curx);

int check_extended_border_collision(GAME* game, int cury, int curx);

int check_self_collision(GAME* game, int cury, int curx);
int check_self_collision_handler(GAME* game, int cury, int curx);

#endif /* _EVENTS_H */
