#include "singleplayer_screen.h"

#include <ncurses.h>
#include <form.h>
#include <stdbool.h>

#include "utils.h"
#include "board.h"

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

void singleplayer_screen_init() {
  curs_set(1);
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

  erase();
  refresh();

  curs_set(0);
  WINDOW* board_wnd = newwin(15, 57, 5, 7);
  
  board = Board_create(board_wnd);

  Board_show(board);

  wrefresh(board_wnd);
  refresh();
}

void singleplayer_screen_close() {
  Board_free(board);
}
