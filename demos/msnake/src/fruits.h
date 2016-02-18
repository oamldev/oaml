#ifndef _FRUITS_H
#define _FRUITS_H

#include "types.h"
#include <stdlib.h>

void redraw_fruits(FRUITS *fruits);
void kill_fruits(FRUITS* fruits);
void kill_fruit(FRUITS *fruits, int posy, int posx);
void kill_fruit_by_ptr(FRUITS *fruits, FRUIT *fruit);
void kill_fruit_at_pos(FRUITS *fruits, int index);
FRUIT *fruit_is_on(FRUITS *fruits, int posy, int posx);
void grow_fruit(GAME* game);
void get_fruit(FRUIT *fruit, int posy, int posx);

#endif /* _FRUITS_H */
