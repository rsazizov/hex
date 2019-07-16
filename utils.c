#include "utils.h"

#include <ncurses.h>

int get_window_width(void) {
  return getmaxx(stdscr);
}

int get_window_height(void) {
  return getmaxy(stdscr);
}
