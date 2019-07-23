#include "scoreboard.h"

void scoreboard_show(WINDOW* wnd, const char* players[2], int current_player) {
  box(wnd, 0, 0);
  wmove(wnd, 1, 1);

  wattron(wnd, COLOR_PAIR(2));
  
  if (current_player == 0) wprintw(wnd, "* %s", players[0]);
  else wprintw(wnd, "  %s", players[0]);
  
  wattroff(wnd, COLOR_PAIR(2));
  wattron(wnd, COLOR_PAIR(3));

  wmove(wnd, 2, 1);
  
  if (current_player == 1) wprintw(wnd, "* %s", players[1]);
  else wprintw(wnd, "  %s", players[1]);
  
  wattroff(wnd, COLOR_PAIR(3));

  wrefresh(wnd);

}