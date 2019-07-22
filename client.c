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

bool Client_connect(Client* client, const char* host) {
  assert(!client->connected);

  int status;

  struct addrinfo hints;
  struct addrinfo* res;

  memset(&hints, 0, sizeof(hints));

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  status = getaddrinfo("localhost", "3445", &hints, &res);

  if (status) {
    printf("getaddrinfo failed: %d\n", status);
    exit(1);
  }

  client->sd = socket(res->ai_family, res->ai_socktype,
                      res->ai_protocol);  

  if (client->sd < 0) {
    printf("socket failed: %d\n", errno);
    exit(1);
  }

  if (connect(client->sd, res->ai_addr, res->ai_addrlen) < 0) {
    printf("connect failed: %d\n", errno);
    exit(1);
  }

  freeaddrinfo(res);

  // TODO: on_connected()?
  printf("Connected to the server.\n");
  client->connected = true;

  // send(client->sd, client->player, strlen(client->player), 0);

  // char name[16];
  // recv(client->sd, name, 15, 0);

  // TODO: Fix name exchange
  // client->opponent = strdup(name);

  return true;
}

void Client_close(Client* client) {
  assert(client->connected);

  close(client->sd);
}

void Client_loop(Client* client, Point(*make_move)(void), void(handle_move)(Point)) {

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
