#pragma once

enum {
  SCREEN_MENU = 0,
  SCREEN_SP,
  SCREEN_MP,
  SCREEN_START_SERVER,
  SCREEN_CONNECT,
  
  // Keep it last
  N_SCREENS
};

void set_current_screen(int screen);
int get_current_screen(void);

