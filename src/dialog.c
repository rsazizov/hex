#include "dialog.h"
#include <stdlib.h>
#include <ncurses.h>
#include <form.h>

#include "utils.h"

void dialog_loop(WINDOW* dialog_wnd, FORM* form, FIELD** fields,
                      char** buff) {
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

        *buff = trim(field_buffer(fields[0], 0));

        active = false;
        
        break;

      default:
        form_driver(form, ch);
        break;        
    }
  }
}

void dialog_show(int height, int width, int y, const char* param, int max_len, char** out) {
  wclear(stdscr);

  WINDOW* dialog_wnd = newwin_cx(height, width, y);

  keypad(dialog_wnd, 1);
  box(dialog_wnd, 0, 0);

  FIELD* fields[2];
  FORM* form;

  fields[0] = new_field(1, max_len, 1, strlen(param) + 1, 0, 0);
  fields[1] = NULL;

  set_field_back(fields[0], A_UNDERLINE);
  field_opts_off(fields[0], O_AUTOSKIP);

  set_field_buffer(fields[0], 0, "");

  form = new_form(fields);

  set_form_win(form, dialog_wnd);
  set_form_sub(form, dialog_wnd);

  post_form(form);

  wmove(dialog_wnd, 1, 1);
  wprintw(dialog_wnd, param);

  refresh();
  wrefresh(dialog_wnd);
  
  dialog_loop(dialog_wnd, form, fields, out);

  free_field(fields[0]);
  free_form(form);

  delwin(dialog_wnd);
}

void dialog_name_show(const char* txt, char** name) {
  dialog_show(5, 32, 10, txt, 16, name);
}

void dialog_port_show(char** port) {
  dialog_show(5, 16, 10, "Port: ", 4, port);
}

void dialog_host_show(char** host) {
  dialog_show(5, 32, 10, "Host: ", 16, host);
}

