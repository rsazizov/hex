#include "singleplayer_screen.h"

#include <ncurses.h>
#include <form.h>
#include <stdbool.h>

#include "screen.h"
#include "utils.h"
#include "board.h"
#include "scoreboard.h"
#include "dialog.h"

static Board* board;
static WINDOW* scoreboard_wnd;
static const char* players[2];

void singleplayer_reset() {
  Board_reset(board);
  scoreboard_show(scoreboard_wnd, players, board->current_player - 1);
  
  move(LINES - 3, 0);
  clrtoeol();
  printw("Use arrow keys to move\nPress enter to place a hex.\nPress \'r\' to restart.");
}

void singleplayer_loop() {
  // HACK: 
  WINDOW* board_wnd = board->wnd;

  Point* cursor = &board->cursor;

  int ch;

  while (1) {
    ch = getch();
     
    switch (ch) {
      case KEY_LEFT:
        cursor->x--;
        break;
      case KEY_RIGHT:
        cursor->x++;
        break;

      case KEY_UP:
        cursor->y--;
        break;

      case KEY_DOWN:
        cursor->y++;
        break;

      case 'r':
        singleplayer_reset();

        break;

      case 10:
        if (Board_is_game_over(board)) {
          singleplayer_reset();
          break;
        }

        Board_make_move(board, cursor->y, cursor->x);
        
        if (Board_is_game_over(board)) {
          int winner = board->winner;
          
          move(LINES - 1, 0);
          clrtoeol();
          printw("%s wins! Press enter to start again. Ctrl-C to exit.", players[winner - 1]);
        }

        scoreboard_show(scoreboard_wnd, players, board->current_player - 1);

        break;
    }

    if (cursor->x >= BOARD_SIZE) cursor->x = 0;
    if (cursor->x < 0) cursor->x = BOARD_SIZE - 1;

    if (cursor->y >= BOARD_SIZE) cursor->y = 0;
    if (cursor->y < 0) cursor->y = BOARD_SIZE - 1;

    werase(board_wnd);
    wrefresh(board_wnd);
    
    Board_show(board);
  }
}

void singleplayer_screen_init() {
  curs_set(1);

  scoreboard_wnd = newwin(4, 25, 0, 70 - 25);
}

void singleplayer_screen_show() {
  char* player1 = NULL;
  char* player2 = NULL;

  dialog_show("Player 1: ", MAX_NAME_LEN, &player1);
  dialog_show("Player 2: ", MAX_NAME_LEN, &player2);

  if (player1 == NULL  || *player1 == '\0') {
    player1 = "Player 1";
  }

  if(player2 == NULL || *player2 == '\0') {
    player2 = "Player 2";
  }

  players[0] = trim(player1);
  players[1] = trim(player2);

  erase();
  refresh();

  curs_set(0);
  WINDOW* board_wnd = newwin(15, 57, 5, 7);
  
  board = Board_create(board_wnd);

  Board_show(board);
  scoreboard_show(scoreboard_wnd, players, board->current_player - 1);

  wrefresh(board_wnd);
  refresh();

  singleplayer_reset();

  singleplayer_loop();
}

void singleplayer_screen_close() {
  Board_free(board);
  
  delwin(scoreboard_wnd);
  delwin(board->wnd);
}
