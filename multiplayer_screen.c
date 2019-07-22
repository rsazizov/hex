#include "multiplayer_screen.h"

#include <ncurses.h>
#include <form.h>
#include <assert.h>

#include <stdbool.h>

#include "utils.h"
#include "client.h"
#include "board.h"

static bool local;
// static Server* server  ;

void multiplayer_screen_init_local() {
  local = true;
}

void multiplayer_screen_init_net() {
  local = false;
}

static Client* client;
static Board* board;


void multiplayer_screen_handle_move(Point move) {
  Board_make_move(board, move.y, move.x);
  werase(board->wnd);
  wrefresh(board->wnd);
  Board_show(board);
}

Point multiplayer_screen_make_move() {
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
        // TODO: Reset request

        break;

      case 10:
        // if (Board_is_game_over(board)) {
        //   singleplayer_reset();
        //   break;
        // }

        if (Board_make_move(board, cursor->y, cursor->x)) {
          werase(board_wnd);
          wrefresh(board_wnd);
          Board_show(board);

          return (Point) { cursor->x, cursor->y };
        }
        
        // if (Board_is_game_over(board)) {
        //   int winner = board->winner;
          
        //   move(LINES - 1, 0);
        //   clrtoeol();
        //   // printw("%s wins! Press enter to start again. Ctrl-C to exit."]);
        // }

        // scoreboard_show(scoreboard_wnd, players, board->current_player - 1);

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

void multiplayer_screen_show() {
  curs_set(1);

  char* name = "";
  char* host = "localhost";

  //server_dialog(&name, &host);

  if (name == NULL || *name == '\0') {
    name = "<noname>";
  }

  if (!local) {
    assert(host);
  }
  
  WINDOW* board_wnd = newwin(15, 57, 5, 7);
  board = Board_create(board_wnd);

  client = Client_create("myau");

  Client_connect(client, "localhost");

  Board_show(board);

  Client_loop(client, multiplayer_screen_make_move,
              multiplayer_screen_handle_move);

  Board_free(board);
}

void multiplayer_screen_close() {
  Client_free(client);
}
