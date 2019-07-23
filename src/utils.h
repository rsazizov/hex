#pragma once

#include <ncurses.h>

int get_window_width(void);
int get_window_height(void);
int center(int big, int small);

// Create a window centered on the x-axis.
WINDOW* newwin_cx(int h, int w, int y);

typedef struct {
  int x;
  int y;
} Point;

Point get_dimensions(const char* txt);

char* trim(char* str);