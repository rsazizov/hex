#include "multiplayer_screen.h"

#include <ncurses.h>
#include <form.h>
#include <assert.h>

#include <stdbool.h>

#include "utils.h"
#include "server.h"

static bool local;
// static Server* server  ;

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

void multiplayer_screen_init_local() {
  local = true;
}

void multiplayer_screen_init_net() {
  local = false;
}

void multiplayer_screen_show() {
  curs_set(1);

  char* name = "";
  char* host = "";

  server_dialog(&name, &host);

  if (name == NULL || *name == '\0') {
    name = "ALA MEOW";
  }

  if (!local) {
    assert(host);
  }

  

}

void multiplayer_screen_close() {
}
