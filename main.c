#include <stdio.h>
#include <stdbool.h>
#include <ncurses.h>
#include <stdlib.h>
#include <pthread.h>

#include "screen.h"
#include "server.h"
#include "client.h"

void init_curses(void) {
  initscr();
  start_color();
  noecho();
  keypad(stdscr, true);
}

void free_curses(void) {
  getch();
  endwin();
}

int main(int argc, char* argv[]) {
  if (argc == 1) {
    Server* server = Server_create("3445");
    Server_start(server);

    Server_wait_for_connection(server);

    Server_loop(server);

    Server_free(server);
  } else {
    init_curses();

    set_current_screen(SCREEN_MENU);

    free_curses();
  }

  return 0;
}
