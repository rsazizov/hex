#include "menu_screen.h"

#include <ncurses.h>
#include <assert.h>
#include <menu.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "screen.h"
#include "utils.h"

// TODO: Make this file auto-generated?
#include "text_res.h"

// TODO: Magic numbers

const int n_items = 3;

static const char* item_names[] = {
  "SINGLEPLAYER",
  "MULTIPLAYER (CONNECT)",
  "MULTIPLAYER (START SERVER)"
};

void handle_selection(const char* item) {
  // Singleplayer
  if (!strcmp(item, item_names[0])) {
    set_current_screen(SCREEN_SP);
  } else if (!strcmp(item, item_names[1])) {
    set_current_screen(SCREEN_START_SERVER);
  } else if (!strcmp(item, item_names[2])) {
    set_current_screen(SCREEN_CONNECT);
  }
}

static WINDOW* menuWnd;
static ITEM** items;
static MENU* menu;
static int n_choices;

void show_logo() {
  WINDOW* logoWnd = newwin(10, 50, 1, get_window_width() / 2 - 20);  

  wprintw(logoWnd, TEXT_LOGO);  
  wrefresh(logoWnd);
}

void show_menu() {
  curs_set(0);

  menuWnd = newwin(40, 40, 10, get_window_width() / 2 - 13);
  
  n_choices = sizeof(item_names) / sizeof(*item_names);

  items = (ITEM**) calloc(n_choices + 1, sizeof(ITEM*));

  for (int i = 0; i < n_choices; ++i) {
    items[i] = new_item(item_names[i], '\0');
  }

  items[n_choices] = NULL;

  menu = new_menu(items);
  mvprintw(LINES - 2,  0, "Hex.\nPress Ctrl-C to exit.");

  set_menu_win(menu, menuWnd);
  set_menu_sub(menu, derwin(menuWnd, 38, 40, 2, 0));

  set_menu_mark(menu, "\0");

  init_pair(1, COLOR_CYAN, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_MAGENTA, COLOR_BLACK);

  set_menu_fore(menu, COLOR_PAIR(1) | A_REVERSE);
	set_menu_grey(menu, COLOR_PAIR(3));

  wmove(menuWnd, 0, 0);
  wprintw(menuWnd, "MAIN MENU");
    
  post_menu(menu);
  wrefresh(menuWnd); 

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

    wrefresh(menuWnd);
  }
}

void menu_screen_init(void) {
}

void menu_screen_show(void) {
  show_logo();
  show_menu();
}

void menu_screen_close(void) {
  unpost_menu(menu);
  
  for (int i = 0; i < n_choices; ++i) {
    free_item(items[i]);
  }
  
  free_menu(menu);
}