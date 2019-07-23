#pragma once

#include "utils.h"

typedef struct {
  int sd;
  char* player_name;
  char* opponent_name;
  bool connected;
} Client;

Client* Client_create(const char* name);
void Client_free(Client* client);

bool Client_connect(Client* client, const char* host, const char* port);
void Client_close(Client* client);

void Client_loop(Client* client, Point(*make_move)(void), void(handle_move)(Point));

void Client_sync(Client* client);
