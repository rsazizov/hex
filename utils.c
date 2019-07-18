#include "utils.h"
#include <string.h>

int get_window_width(void) {
  return getmaxx(stdscr);
}

int get_window_height(void) {
  return getmaxy(stdscr);
}

int center(int big, int small) {
  return (big - small) / 2;
}

WINDOW* newwin_cx(int h, int w, int y) {
  return newwin(h, w, y, center(get_window_width(), w));
}

Point get_dimensions(const char* txt) {
  Point xy;
  xy.x = xy.y = 0;
  
  size_t len = strlen(txt);

  int max_x = 0;
  
  for (size_t i = 0; i < len; ++i) {
    if (txt[i] == '\n') {
      
      xy.y += 1;

      if (xy.x > max_x) {
        max_x = xy.x;
      }

      xy.x = 0;
    } else {
      xy.x += 1;
    }
  }

  xy.x = max_x + 1;
  xy.y += 1;

  return xy;
}