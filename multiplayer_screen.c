#include "multiplayer_screen.h"

#include <ncurses.h>
#include <form.h>
#include <assert.h>

#include <stdbool.h>

#include "utils.h"
#include "client.h"
#include "board.h"
#include "scoreboard.h"

static bool local;
static Player player_color;
static Client* client;
static Board* board;
static WINDOW* scoreboard_wnd;

void multiplayer_screen_show_scoreboard() {
  const char* players[2] = {
    client->player_name, client->opponent_name
  };

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
//
// TOOD: Dialog class?
void server_dialog_loop(WINDOW* dialog_wnd, FORM* form, FIELD** fields,
                      char** name, char** host) {
  int ch;
  bool active = true;

  while(active && (ch = wgetch(dialog_wnd)) != EOF) {
    switch (ch) {
      case KEY_DOWN:
        form_driver(form, REQ_NEXT_FIELD);
        form_driver(form, REQ_END_LINE);
        
        break;

      case KEY_UP:
        form_driver(form, REQ_PREV_FIELD);
        form_driver(form, REQ_END_LINE);

        break;
      case KEY_LEFT:
        form_driver(form, REQ_PREV_CHAR);
        break;

      case KEY_RIGHT:
        form_driver(form, REQ_NEXT_CHAR);
        
        break;

      case KEY_BACKSPACE:
        form_driver(form, REQ_DEL_PREV);
        
        break;

      case 10:
      	form_driver(form, REQ_NEXT_FIELD);
  			form_driver(form, REQ_PREV_FIELD);

        *name = field_buffer(fields[0], 0);
        *host = field_buffer(fields[1], 0);

        active = false;
        
        break;

      default:
        form_driver(form, ch);
        break;        
    }
  }
}

void server_dialog(char** name, char** host) {
  WINDOW* dialog_wnd = newwin_cx(16, 51, 4);

  keypad(dialog_wnd, 1);
  box(dialog_wnd, 0, 0);

  FIELD* fields[3];
  FORM* form;

  fields[0] = new_field(1, 15, 1, 7, 0, 0);
  fields[1] = new_field(1, 18, 3, 7, 0, 0);
  fields[2] = NULL;

  set_field_back(fields[0], A_UNDERLINE);
  field_opts_off(fields[0], O_AUTOSKIP);

  set_field_back(fields[1], A_UNDERLINE);
  field_opts_off(fields[1], O_AUTOSKIP);

  if (local) {
    field_opts_off(fields[1], O_ACTIVE);
  }

  set_field_buffer(fields[0], 0, "");
  set_field_buffer(fields[1], 0, "");

  form = new_form(fields);

  set_form_win(form, dialog_wnd);
  set_form_sub(form, dialog_wnd);

  post_form(form);

  wmove(dialog_wnd, 1, 1);
  wprintw(dialog_wnd, "Name:");
  wmove(dialog_wnd, 3, 1);
  wprintw(dialog_wnd, "Host:");
  move(LINES - 1, 0);
  printw("Press Enter to start the server.");

  refresh();
  wrefresh(dialog_wnd);

  server_dialog_loop(dialog_wnd, form, fields, name, host);
  delwin(dialog_wnd);
}

static char* name;
static char* host;

// TODO: common init
void multiplayer_screen_init_local() {
  curs_set(1);
  local = true;
  player_color = Red;
  scoreboard_wnd = newwin(4, 25, 0, 70 - 25);

  server_dialog(&name, &host);
}

void multiplayer_screen_init_net() {
  curs_set(1);
  local = false;
  player_color = Blue;
  scoreboard_wnd = newwin(4, 25, 0, 70 - 25);

  server_dialog(&name, &host);
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

  Client_connect(client, "localhost");

  multiplayer_screen_show_scoreboard();
  Board_show(board);

  Client_loop(client, multiplayer_screen_make_move,
              multiplayer_screen_handle_move);

  Board_free(board);
}

void multiplayer_screen_close() {
  Client_free(client);
}
