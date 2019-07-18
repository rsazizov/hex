#pragma once

#include <stdbool.h>
#include "utils.h"

#define BOARD_SIZE 11

typedef struct {
  short state[BOARD_SIZE][BOARD_SIZE];
  int current_player;
  Point cursor;
  WINDOW* wnd;
} Board;

Board* Board_create(WINDOW* wnd);
void Board_free(Board* board);

void Board_reset(Board* board);

void Board_show(Board* board);

// Checks if y and x are in range [0, 10]
bool Board_is_legal_move(Board* board, int y, int x);

// Returns the player who has to make a move
int Board_get_current_player();

// Checks if the cell at (y, x) is free
bool Board_can_move(Board* board, int y, int x);
bool Board_make_move(Board* board, int player, int y, int x);