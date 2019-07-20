#include "singleplayer_screen.h"

#include <ncurses.h>
#include <form.h>
#include <stdbool.h>

#include "screen.h"
#include "utils.h"
#include "board.h"
#include "scoreboard.h"

void name_dialog_loop(WINDOW* dialog_wnd, FORM* form, FIELD** fields,
                      char** player1, char** player2) {
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

        *player1 = field_buffer(fields[0], 0);
        *player2 = field_buffer(fields[1], 0);

        active = false;
        
        break;

      default:
        form_driver(form, ch);
        break;        
    }
  }
}

void name_dialog(char** player1, char** player2) {
  WINDOW* dialog_wnd = newwin_cx(16, 51, 4);

  keypad(dialog_wnd, 1);
  box(dialog_wnd, 0, 0);

  FIELD* fields[3];
  FORM* form;

  fields[0] = new_field(1, 15, 1, 11, 0, 0);
  fields[1] = new_field(1, 15, 3, 11, 0, 0);
  fields[2] = NULL;

  set_field_back(fields[0], A_UNDERLINE);
  field_opts_off(fields[0], O_AUTOSKIP);

  set_field_back(fields[1], A_UNDERLINE);
  field_opts_off(fields[1], O_AUTOSKIP);

  set_field_buffer(fields[0], 0, "Player 1");
  set_field_buffer(fields[1], 0, "Player 2");

  form = new_form(fields);

  set_form_win(form, dialog_wnd);
  set_form_sub(form, dialog_wnd);

  post_form(form);

  wmove(dialog_wnd, 1, 1);
  wprintw(dialog_wnd, "Player 1:");
  wmove(dialog_wnd, 3, 1);
  wprintw(dialog_wnd, "Player 2:");
  
  move(LINES - 1, 0);
  printw("Press Enter to start the game.");

  refresh();
  wrefresh(dialog_wnd);

  name_dialog_loop(dialog_wnd, form, fields, player1, player2);
  delwin(dialog_wnd);
}

static Board* board;
static WINDOW* scoreboard_wnd;
static const char* players[2];

void singleplayer_reset() {
  Board_reset(board);
  scoreboard_show(scoreboard_wnd, players, board->current_player - 1);
  
  move(LINES - 1, 0);
  clrtoeol();
  printw("Use arrow keys to move. Press enter to make a move. Press \'r\' to restart.");
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

  name_dialog(&player1, &player2);

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
