#pragma once

#include <stdbool.h>

typedef struct {
  // Server socket.
  int sd;

  // Number of connected clients.
  // The game starts when both clients are connected.
  int n_connections;
  int client_sockets[2];

  bool running;

  char* port;

  // Player names.
  char* players[2];

  // Error message (if any).
  char* error;
} Server;

Server* Server_create(const char* port);
void Server_free(Server* server);

bool Server_start(Server* server);
void Server_close(Server* server);

void Server_loop(Server* server);

void Server_wait_for_connections(Server* server);

