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

void multiplayer_screen_on_end() {
  int winner = board->winner;
  
  const char* winner_name = NULL;

  if (local) {
    if (board->winner == 1) {
      winner_name = client->player_name;
    } else {
      winner_name = client->opponent_name;
    }
  } else {
    if (board->winner == 1) {
      winner_name = client->opponent_name;
    } else {
      winner_name = client->player_name;
    }
  }

  move(LINES - 2, 0);
  clrtoeol();
  printw("%s wins!\nPress enter to start again. Ctrl-C to exit.", winner_name);
  refresh();
}

void multiplayer_screen_handle_move(Point move) {
  Board_make_move(board, move.y, move.x);
  werase(board->wnd);
  wrefresh(board->wnd);
  multiplayer_screen_show_scoreboard();
  
  Board_show(board);

  if (Board_is_game_over(board)) {
    multiplayer_screen_on_end();
  }
}

Point multiplayer_screen_make_move() {
  WINDOW* board_wnd = board->wnd;

  Point* cursor = &board->cursor;

  int ch;

  flushinp();

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
        if (Board_make_move(board, cursor->y, cursor->x)) {
          werase(board_wnd);
          wrefresh(board_wnd);
          multiplayer_screen_show_scoreboard();
          Board_show(board);
          
          if (Board_is_game_over(board)) {
            multiplayer_screen_on_end();
          }

          return (Point) { cursor->x, cursor->y };
        }
        
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
static pthread_t server_thid;

void* server_thread(void* arg) {
  // TODO: Put initialization here?
  beep();

  Server_start(server);
  Server_wait_for_connections(server);

  Server_loop(server);

  return NULL;
}

void start_server_thread() {
  server = Server_create(port);
  
  if (pthread_create(&server_thid, NULL, server_thread, "Server thread") < 0) {
    printw("pthread_create failed.\n");
    getch();
    set_current_screen(SCREEN_MENU);
  }
}

void multiplayer_screen_init_local() {
  curs_set(1);
  local = true;
  player_color = Red;
  scoreboard_wnd = newwin(4, 25, 0, 70 - 25);

  dialog_show("Name: ", MAX_NAME_LEN, &name);
  dialog_show("Port: ", 4, &port);

  start_server_thread();

  refresh();
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
  // Server is running at this point (if local).

  curs_set(0);

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
  if (local) {
    pthread_join(server_thid, NULL);
  }

  Board_free(board);
  Client_free(client);
  Server_free(server);
}
