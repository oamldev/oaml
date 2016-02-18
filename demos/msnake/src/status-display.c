#include <oaml.h>

#include "status-display.h"
#include "highscore.h"

// creates a little display on the top right of the screen informing you
// how long your snake is, how much points you have, how long you played
// and what your current score is
//
// the whole thing should look like this:
//
// #####################015L-00123P-0012s-1206SCR
//                                              #
//                                              #
//                                              #
//                                              #
//                                              #
void status_display(GAME *game) {
  const int width = 25; // the width in chars of the display

  // create a new window in the top right corner with a height of 1
  // and a width of 'width'
  WINDOW *win = newwin(1, width, 0, game->columns - width);

  // print out the current length, the points, the time and the current highscore
  wprintw(win,"%03iL-%05iP-%04is-%04iSCR",
      game->snake.length,
      game->highscore,
      time(NULL) - game->started - game->paused,
      calculate_score(game->highscore, time(NULL) - game->started - game->paused));

  // display it
  wrefresh(win);

  // free the memory needed for the window
  delwin(win);

  win = newwin(1, 40, 0, 0);
  wprintw(win,"%s", oamlGetPlayingInfo());
  wrefresh(win);
  delwin(win);
}
