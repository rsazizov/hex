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

Client* Client_create(const char* name) {
  Client* client = malloc(sizeof(Client));

  client->connected  = false;
  client->player = strdup(name);
  client->opponent = NULL;
  client->s = -1;

  return client;
}

void Client_free(Client* client) {
  if (client->connected) {
    Client_close(client);
  }
  
  free(client);
}

bool Client_connect(Client* client, const char* host) {
  int status;

  struct addrinfo hints;
  struct addrinfo* res;

  memset(&hints, 0, sizeof(hints));

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  status = getaddrinfo("localhost", "3445", &hints, &res);

  if (status) {
    printf("Error %d\n", status);
    exit(1);
  }

  client->s = socket(res->ai_family, res->ai_socktype,
                      res->ai_protocol);  

  if (connect(client->s, res->ai_addr, res->ai_addrlen) < 0) {
    printf("fml\n");
    exit(1);
  }

  printf("Connected to the server.\n");

  send(client->s, client->player, strlen(client->player), 0);

  char name[16];
  recv(client->s, name, 15, 0);

  // TODO: free()
  client->opponent = strdup(name);

  return true;
}

void Client_close(Client* client) {
}

bool Client_make_move(Client* client, int y, int x) {
  return false;
}

Point Client_wait_for_move(Client* client) {
  return (Point) {0, 0};
}

bool Client_is_game_over(Client* client, int* winner) {
  return false;
}
