#pragma once

#include <stdbool.h>
#include "utils.h"

#include "disjoint_set.h"

#define BOARD_SIZE 11

typedef enum {
  Empty = 0,
  TakenRed,
  TakenBlue
} CellState;

typedef struct {
  CellState state[BOARD_SIZE][BOARD_SIZE];
  DisjointSet* sets[2];
  int current_player;
  Point cursor;
  int winner;
  WINDOW* wnd;
} Board;

typedef enum {
  Red = 1,
  Blue
} Player;

Board* Board_create(WINDOW* wnd);
void Board_free(Board* board);

void Board_reset(Board* board);
void Board_show(Board* board);

// Returns the player who has to make a move
int Board_get_current_player(Board* board);

// Checks if the cell at (y, x) is free
bool Board_can_move(Board* board, int y, int x);
bool Board_make_move(Board* board, int y, int x);

bool Board_is_game_over(Board* board);