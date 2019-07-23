#include "client.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifndef __USE_XOPEN2K
#define __USE_XOPEN2K
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <ncurses.h>

#include "package.h"

Client* Client_create(const char* name) {
  Client* client = malloc(sizeof(Client));

  client->connected  = false;
  client->player_name = strdup(name);
  client->opponent_name = NULL;
  client->sd = -1;

  return client;
}

void Client_free(Client* client) {
  if (client->connected) {
    Client_close(client);
  }
  
  free(client->player_name);
  free(client->opponent_name);
  free(client);
}

bool Client_connect(Client* client, const char* host, const char* port) {
  assert(!client->connected);

  int status;

  struct addrinfo hints;
  struct addrinfo* res;

  memset(&hints, 0, sizeof(hints));

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(host, port, &hints, &res);

  if (status) {
    printw("getaddrinfo failed: %d\n", status);
    return false;
  }

  client->sd = socket(res->ai_family, res->ai_socktype,
                      res->ai_protocol);  

  if (client->sd < 0) {
    printw("socket failed: %d\n", errno);
    return false;
  }

  if (connect(client->sd, res->ai_addr, res->ai_addrlen) < 0) {
    printw("connect failed: %d\n", errno);
    return false;
  }

  freeaddrinfo(res);

  // TODO: on_connected()?
  printw("Connected to the server.\n");
  client->connected = true;

  // Je me presente.
  
  Package my_name_pkg;
  my_name_pkg.op = OP_NAME;
  my_name_pkg.name = client->player_name;

  send_package(client->sd, my_name_pkg);

  Package op_name_pkg = recv_package(client->sd);
  assert(!strcmp(op_name_pkg.op, OP_NAME));

  client->opponent_name = op_name_pkg.name;

  return true;
}

void Client_close(Client* client) {
  assert(client->connected);

  close(client->sd);
}

void Client_loop(Client* client, Point(*make_move)(void), void(handle_move)(Point)) {
  printf("Playing with: %s\n", client->opponent_name);

  while (1) {
    Package p = recv_package(client->sd);

    if (!strcmp(p.op, OP_MOVE_REQUEST)) {
      Point move = make_move();
      Package move_pkg = {OP_MOVE, move.y, move.x};
      send_package(client->sd, move_pkg);
    } else if (!strcmp(p.op, OP_MOVE)) {
      handle_move((Point) {p.x, p.y});
    }
  }

}
