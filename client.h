#pragma once

#include "utils.h"

typedef struct {
  int s;
  char* player;
  char* opponent;
  bool connected;
} Client;

Client* Client_create(const char* name);
void Client_free(Client* client);

// TODO: Port
bool Client_connect(Client* client, const char* host);
void Client_close(Client* client);

bool Client_make_move(Client* client, int y, int x);
Point Client_wait_for_move(Client* client);

void Client_loop(Client* client, Point(*make_move)(void), void(handle_move)(Point));

void Client_sync(Client* client);

bool Client_is_game_over(Client* client, int* winner);