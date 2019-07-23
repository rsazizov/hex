#pragma once

#include <ncurses.h>

void scoreboard_show(WINDOW* wnd, const char* players[2], int current_player);