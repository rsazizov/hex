#pragma once

enum {
  SCREEN_MENU = 0,
  SCREEN_SP,

  // Multiplayer connected to the localhost
  SCREEN_MP_LOCAL,

  // Multiplayer connected to a remote host
  SCREEN_MP_NET,

  // Keep it last
  N_SCREENS
};

void set_current_screen(int screen);
int get_current_screen(void);

