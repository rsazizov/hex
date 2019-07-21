#pragma once

#include <stdbool.h>

typedef struct {
  // Server socket
  int s;

  int n_connections;
  int client_sockets[2];

  bool running;
  char* service;

  char* players[2];
} Server;

Server* Server_create(const char* port);
void Server_free(Server* server);

bool Server_start(Server* server);
void Server_close(Server* server);

void Server_wait_for_connection(Server* server);
bool Server_make_move(int player, int y, int x);

char* Server_get_opponent_name(Server* server);
