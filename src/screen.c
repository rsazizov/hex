#include "screen.h"

#include <stdio.h>
#include <ncurses.h>
#include <assert.h>

#include "menu_screen.h"
#include "singleplayer_screen.h"
#include "multiplayer_screen.h"

void dummy() {}

static int current_screen_id = -1;

struct {
  void(*init)();
  void(*show)();
  void(*close)();
} screens[N_SCREENS] = {
  [SCREEN_MENU] = { 
    menu_screen_init,
    menu_screen_show,
    menu_screen_close
  },
  
  [SCREEN_SP] = {
    singleplayer_screen_init,
    singleplayer_screen_show, 
    singleplayer_screen_close 
  },

  [SCREEN_MP_LOCAL] = {
    multiplayer_screen_init_local,
    multiplayer_screen_show,
    multiplayer_screen_close
  },
  
  [SCREEN_MP_NET] = {
    multiplayer_screen_init_net,
    multiplayer_screen_show,
    multiplayer_screen_close
  }
};

void set_current_screen(int screen) {
  assert(screen >= 0 && screen < N_SCREENS);

  if (current_screen_id > 0) {
    screens[current_screen_id].close();
  }

  current_screen_id = screen;
  
  screens[current_screen_id].init();

  erase();
  refresh();

  screens[current_screen_id].show();
}

int get_current_screen(void) {
  return current_screen_id;
}

