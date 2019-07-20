#include "board.h"

#include <ncurses.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

static const int BOTTOM_NODE_ID = BOARD_SIZE * BOARD_SIZE;
static const int TOP_NODE_ID = BOARD_SIZE * BOARD_SIZE + 1;

Board* Board_create(WINDOW* wnd) {
  Board* board = malloc(sizeof(Board));

  assert(wnd != NULL);
  board->wnd = wnd;

  board->sets[0] = DisjointSet_create(BOARD_SIZE * BOARD_SIZE + 2);
  board->sets[1] = DisjointSet_create(BOARD_SIZE * BOARD_SIZE + 2);

  Board_reset(board);

  return board;
}

void Board_free(Board* board) {
  free(board);

  DisjointSet_free(board->sets[0]);
  DisjointSet_free(board->sets[1]);
}

void Board_reset(Board* board) {
  memset(board->state, 0, sizeof(board->state));

  board->winner = 0;
  board->current_player = Red;

  board->cursor.x = 0;
  board->cursor.y = 0;

  for (int i = 0; i < 2; ++i) {
    DisjointSet_reset(board->sets[i]);
  }
  
  for (int i = 0; i < BOARD_SIZE; ++i) {
    DisjointSet_union(board->sets[0], Board_yx_to_node_id(i, 0), TOP_NODE_ID);
    DisjointSet_union(board->sets[0], Board_yx_to_node_id(i, BOARD_SIZE - 1), BOTTOM_NODE_ID);
  }

  for (int i = 0; i < BOARD_SIZE; ++i) {
    DisjointSet_union(board->sets[1], Board_yx_to_node_id(0, i), TOP_NODE_ID);
    DisjointSet_union(board->sets[1], Board_yx_to_node_id(BOARD_SIZE - 1, i), BOTTOM_NODE_ID);
  }
}

void Board_show_cell(Board* board, int j, int i) {
  const char* hex = "⬡";
  const char* hex_full = "⬢";

  int hex_state = board->state[j][i];

  int color_pair = hex_state + 1;

  bool current = (board->cursor.y == j) && (board->cursor.x == i);
  
  if (current) {
    color_pair += 3;
  } else {
    // Color cells that are on the edges of the board
    if ((hex_state == 0 && i == 0 && j > 0 && j < BOARD_SIZE - 1)
      || (hex_state == 0 && i == BOARD_SIZE - 1 && j > 0 && j < BOARD_SIZE - 1)) {
      color_pair = 7;
    }

    if ((hex_state == 0 && j == 0 && i > 0 && i < BOARD_SIZE - 1)
      || (hex_state == 0 && j == BOARD_SIZE - 1 && i > 0 && i < BOARD_SIZE - 1)) {
      color_pair = 8;
    }
  }

  wattron(board->wnd, COLOR_PAIR(color_pair));
  
  if (hex_state) wprintw(board->wnd, hex_full);
  else wprintw(board->wnd, hex);

  wattroff(board->wnd, COLOR_PAIR(color_pair));

  waddch(board->wnd, ' ');
}

void init_color_pairs() { 
  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  init_pair(2, COLOR_RED, COLOR_BLACK);
  init_pair(3, COLOR_BLUE, COLOR_BLACK);

  init_pair(4, COLOR_BLACK, COLOR_WHITE);
  init_pair(5, COLOR_RED, COLOR_WHITE);
  init_pair(6, COLOR_BLUE, COLOR_WHITE);

  init_pair(7, COLOR_RED, COLOR_BLACK);
  init_pair(8, COLOR_BLUE, COLOR_BLACK);
}

void Board_show(Board* board) {
  init_color_pairs();

  // Print column names

  waddch(board->wnd, ' ');
  for (int i = 0; i < BOARD_SIZE; ++i) {
    char col_id = 'A' + i;
    wprintw(board->wnd, " %c", col_id);
  }

  waddch(board->wnd, '\n');

  for (int i = 0; i < BOARD_SIZE; ++i) {

    for (int k = 0; k < i; ++k) {
      waddch(board->wnd, ' ');
    }

    if (i < 9) wprintw(board->wnd, "%d  ", i + 1);
    else wprintw(board->wnd, "%d ", i + 1);

    for (int j = 0; j < BOARD_SIZE; ++j) {
      Board_show_cell(board, i, j);
    }

    waddch(board->wnd, '\n');
  }

  wrefresh(board->wnd);
}

bool Board_is_legal_move(int y, int x) {
  return y >= 0 && y < BOARD_SIZE && x >= 0 && x < BOARD_SIZE;
}

int Board_get_current_player(Board* board) {
  return board->current_player;
}

bool Board_is_game_over(Board* board) {
  return board->winner > 0;
}

bool Board_can_move(Board* board, int y, int x) {
  return Board_is_legal_move(y, x)
    && board->state[y][x] == 0;
}

bool Board_legal_and_taken(Board* board, Player player, int y, int x) {
  return Board_is_legal_move(y, x) && board->state[y][x] == player;
}

int Board_yx_to_node_id(int y, int x) {
  return BOARD_SIZE * x + y;
}

void Board_maybe_connect(Board* board, int by, int bx, int y, int x) {
  if (Board_legal_and_taken(board, board->current_player, y, x)) {
    DisjointSet* set = board->sets[board->current_player - 1];
    DisjointSet_union(set, Board_yx_to_node_id(by, bx), Board_yx_to_node_id(y, x));
  }
}

bool Board_make_move(Board* board, int y, int x) {
  if (!Board_is_legal_move(y, x) || !Board_can_move(board, y, x)) {
    return false;
  }

  board->state[y][x] = board->current_player;

  int set_id = board->current_player - 1;
  DisjointSet* set = board->sets[set_id];

  Board_maybe_connect(board, y, x, y - 1, x);
  Board_maybe_connect(board, y, x, y - 1, x + 1);
  
  Board_maybe_connect(board, y, x, y, x - 1);
  Board_maybe_connect(board, y, x, y, x + 1);

  Board_maybe_connect(board, y, x, y + 1, x - 1);
  Board_maybe_connect(board, y, x, y + 1, x);

  if (DisjointSet_query(set, TOP_NODE_ID, BOTTOM_NODE_ID)) {
    board->winner = board->current_player;
  }
    
  if (board->current_player == Red) {
   board->current_player = Blue;
  } else {
   board->current_player = Red;
  }

  return true;
}
