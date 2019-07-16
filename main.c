#include <stdio.h>
#include <stdbool.h>
#include <ncurses.h>
#include <stdlib.h>

#include "screen.h"

void init_curses(void) {
  initscr();
  start_color();
  noecho();
  keypad(stdscr, true);
}

void free_curses(void) {
  getch();
  endwin();
}

int main() {
  init_curses();

  set_current_screen(SCREEN_MENU);

  free_curses();
  
  return 0;
}
