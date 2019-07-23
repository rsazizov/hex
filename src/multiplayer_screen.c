#include "multiplayer_screen.h"

#include <ncurses.h>
#include <form.h>
#include <assert.h>

#include <stdbool.h>

#include "utils.h"
#include "client.h"
#include "board.h"
#include "scoreboard.h"
#include "dialog.h"
#include "screen.h"
#include "server.h"

#include <unistd.h>
#include <pthread.h>

static bool local;
static Player player_color;
static Client* client;
static Board* board;
static WINDOW* scoreboard_wnd;

void multiplayer_screen_show_scoreboard() {
  const char* players[2];

  if (player_color == Red) {
    players[0] = client->player_name;
    players[1] = client->opponent_name;
  } else {
    players[1] = client->player_name;
    players[0] = client->opponent_name;
  }

  scoreboard_show(scoreboard_wnd, players, board->current_player - 1);
}

void multiplayer_screen_handle_move(Point move) {
  Board_make_move(board, move.y, move.x);
  werase(board->wnd);
  wrefresh(board->wnd);
  multiplayer_screen_show_scoreboard();
  
  Board_show(board);

  if (Board_is_game_over(board)) {
      int winner = board->winner;
      
      move(LINES - 1, 0);
      clrtoeol();
      printw("Player %d wins! Press enter to start again. Ctrl-C to exit.", winner);
  }
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
          multiplayer_screen_show_scoreboard();
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
    
    multiplayer_screen_show_scoreboard();
    Board_show(board);
  }

}

static char* name;
static char* port;

// For "connect" only.
static char* host;

static Server* server;

void* server_thread(void* arg) {
  // TODO: Put initialization here?
  beep();

  Server_start(server);
  Server_wait_for_connections(server);

  Server_loop(server);

  return NULL;
}

void multiplayer_screen_init_local() {
  curs_set(1);
  local = true;
  player_color = Red;
  scoreboard_wnd = newwin(4, 25, 0, 70 - 25);

  dialog_show("Name: ", MAX_NAME_LEN, &name);
  dialog_show("Port: ", 4, &port);

  server = Server_create(port);
  
  pthread_t tid;
  pthread_create(&tid, NULL, server_thread, "Server thread");
}

void multiplayer_screen_init_net() {
  curs_set(1);
  local = false;
  player_color = Blue;
  scoreboard_wnd = newwin(4, 25, 0, 70 - 25);

  dialog_show("Name: ", MAX_NAME_LEN, &name);
  dialog_show("Host: ", MAX_NAME_LEN, &host);
  dialog_show("Port: ", 4, &port);
}

void multiplayer_screen_show() {

  if (name == NULL || *name == '\0') {
    name = "<noname>";
  }

  if (!local) {
    assert(host);
  }
  
  WINDOW* board_wnd = newwin(15, 57, 5, 7);
  board = Board_create(board_wnd);

  client = Client_create(name);

  if (local) {
    host = "localhost";
  }

  sleep(1);

  if (!Client_connect(client, host, port)) {
    getch();
    set_current_screen(SCREEN_MENU);
  }

  multiplayer_screen_show_scoreboard();
  Board_show(board);


  Client_loop(client, multiplayer_screen_make_move,
              multiplayer_screen_handle_move);

  delwin(board_wnd);
}

void multiplayer_screen_close() {
  Board_free(board);
  Client_free(client);
  Server_free(server);
}
