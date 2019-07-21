#include "menu_screen.h"

#include <ncurses.h>
#include <menu.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "screen.h"
#include "utils.h"

#include "text_res.h"

#define N_ITEMS 3

static const char* item_names[N_ITEMS] = {
  "SINGLEPLAYER",
  "MULTIPLAYER (CONNECT)",
  "MULTIPLAYER (START SERVER)"
};

int get_menu_width() {
  int max_width = 0;
  
  for (int i = 0; i < N_ITEMS; ++i) {
    int width = strlen(item_names[i]) - 1;
    
    if (width > max_width) {
      max_width = width;
    }
  }

  return max_width;
}

void handle_selection(const char* item) {
  if (!strcmp(item, item_names[0])) {
    set_current_screen(SCREEN_SP);
  } else if (!strcmp(item, item_names[1])) {
    set_current_screen(SCREEN_MP_LOCAL);
  } else if (!strcmp(item, item_names[2])) {
    set_current_screen(SCREEN_MP_NET);
  }
}

static ITEM** items;
static MENU* menu;

void show_logo() {
  Point xy = get_dimensions(TEXT_LOGO);

  WINDOW* logoWnd = newwin_cx(xy.y, xy.x, 1);

  wprintw(logoWnd, TEXT_LOGO);  
  wrefresh(logoWnd);
}

void menu_loop(WINDOW* menu_wnd) {
  int ch;
  bool active = true;

  while (active) {
    ch = getch();
  
    switch (ch) {
      case KEY_DOWN:
        menu_driver(menu, REQ_DOWN_ITEM);
        break;
      case KEY_UP:
        menu_driver(menu, REQ_UP_ITEM);
        break;

      // Enter
      case 10:
        active = false;
        handle_selection(item_name(current_item(menu)));
        break;
    }

    wrefresh(menu_wnd);
  }
}

void show_menu() {
  curs_set(0);

  items = (ITEM**) calloc(N_ITEMS + 1, sizeof(ITEM*));

  for (int i = 0; i < N_ITEMS; ++i) {
    items[i] = new_item(item_names[i], '\0');
  }

  items[N_ITEMS] = NULL;

  menu = new_menu(items);
  mvprintw(LINES - 2,  0, "Hex.\nPress Ctrl-C to exit.");

  // Menu title + empty line
  int menu_height = N_ITEMS + 2;
  int menu_width = get_menu_width() + 1;
  
  WINDOW* menu_wnd = newwin_cx(menu_height, menu_width, 10);

  set_menu_win(menu, menu_wnd);
  set_menu_sub(menu, derwin(menu_wnd, 0, 0, 2, 0));

  set_menu_mark(menu, "\0");

  init_pair(1, COLOR_CYAN, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_MAGENTA, COLOR_BLACK);

  set_menu_fore(menu, COLOR_PAIR(1) | A_REVERSE);
	set_menu_grey(menu, COLOR_PAIR(3));

  wmove(menu_wnd, 0, 0);
  wprintw(menu_wnd, "MAIN MENU");
    
  post_menu(menu);
  wrefresh(menu_wnd); 

  menu_loop(menu_wnd);
}

void menu_screen_init(void) {
}

void menu_screen_show(void) {
  show_logo();
  show_menu();
}

void menu_screen_close(void) {
  unpost_menu(menu);
  
  for (int i = 0; i < N_ITEMS; ++i) {
    free_item(items[i]);
  }
  
  free_menu(menu);
}
