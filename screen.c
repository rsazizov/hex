#include "screen.h"

#include <stdio.h>
#include <ncurses.h>
#include <assert.h>

#include "menu_screen.h"

void dummy() {}

static int current_screen_id = -1;

struct {
  void(*init)();
  void(*show)();
  void(*close)();
} screens[N_SCREENS] = {
  [SCREEN_MENU] = { menu_screen_init, menu_screen_show, menu_screen_close},
  [SCREEN_SP] = { dummy, dummy, dummy},
  [SCREEN_MP] = { dummy, dummy, dummy},
  [SCREEN_START_SERVER] = { dummy, dummy, dummy},
  [SCREEN_CONNECT] = { dummy, dummy, dummy}
};

void set_current_screen(int screen) {
  assert(screen >= 0 && screen < N_SCREENS);

  if (current_screen_id > 0) {
    screens[current_screen_id].close();
  }

  erase();
  refresh();

  current_screen_id = screen;
  
  screens[current_screen_id].init();
  screens[current_screen_id].show();
}

int get_current_screen(void) {
  return current_screen_id;
}

