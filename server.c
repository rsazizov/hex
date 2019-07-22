#include "server.h"

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

#include "board.h"
#include "package.h"

Server* Server_create(const char* port) {
  Server* server = malloc(sizeof(Server));

  server->running = false;

  // TODO: meh
  assert(strlen(port) == 4);

  server->port = strdup(port);
  server->error = NULL;
  
  server->players[0] = NULL;
  server->players[1] = NULL;

  server->sd = -1;
  server->client_sockets[0] = -1;
  server->client_sockets[1] = -1;
  
  return server;
}

void Server_free(Server* server) {
  if (server->running) {
    Server_close(server);
  }

  free(server->players[0]);
  free(server->players[1]);
  free(server->error);
  
  free(server->port);
  free(server);
}

bool Server_start(Server* server) {
  assert(!server->running);

  // TODO: Proper error handling
  int status;

  struct addrinfo hints;
  struct addrinfo* servinfo;

  memset(&hints, 0, sizeof(hints));

  hints.ai_family = AF_UNSPEC;
  hints.ai_flags = AI_PASSIVE;
  hints.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(NULL, server->port, &hints, &servinfo);

  if (status) {
    printf("getaddrinfo failed: %d\n", status);
    exit(1);
  }

  server->sd = socket(servinfo->ai_family, servinfo->ai_socktype,
                    servinfo->ai_protocol);  

  int yes = 1;
  if (setsockopt(server->sd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
      printf("setsockopt failed: %d\n", errno);
      exit(1);
  } 

  if (bind(server->sd, servinfo->ai_addr, servinfo->ai_addrlen)) {
    printf("bind failed: %d\n", errno);
    exit(1);
  }

  freeaddrinfo(servinfo);

  server->running = true;

  return true;
}

void Server_close(Server* server) {
  assert(server->running);

  close(server->client_sockets[0]);
  close(server->client_sockets[1]);

  server->running = false;
}

void Server_loop(Server* server) {
  assert(server->running);
  assert(server->n_connections == 2);

  // At this point there has been a name exchange between the
  // clients. The server should send a move request to the first
  // client(?), update the board, notify the other player, etc...

  Board* board = Board_create(NULL);

  while (!Board_is_game_over(board)) {
    // TODO: better switching
    int sid = board->current_player - 1;
    int socket = server->client_sockets[sid];
    int other_socket = server->client_sockets[!sid];
    
    send_package(socket, (Package) {
      OP_MOVE_REQUEST, -1, -1
    });
    
    Package p = recv_package(socket);

    printf("Player %d makes a move (%d, %d)\n", 
            board->current_player, p.y, p.x);

    send_package(other_socket, p);

    // This should never fail
    assert(Board_make_move(board, p.y, p.x));
  }

  // At this point the game is over. However, we want ask the
  // users if they want to play again.

  Board_free(board);
}

void Server_wait_for_connections(Server* server) {
  assert(server->running);

  listen(server->sd, 2);
  
  assert(server->n_connections <= 2);
  server->n_connections++;

  int* client_socket = 
        &server->client_sockets[server->n_connections - 1];

  struct sockaddr_storage in_addr;

  socklen_t addr_size = sizeof(in_addr);
  
  printf("Waiting for connections on %s\n", server->port);

  *client_socket = accept(server->sd, (struct sockaddr*) &in_addr, &addr_size);

  char hbuf[32], sbuf[32];

  getnameinfo((struct sockaddr*) &in_addr, addr_size, hbuf, 
      sizeof(hbuf), sbuf, sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV);

  printf("Got connection from %s, %s\n", hbuf, sbuf);

  // if (server->n_connections > 1 ) {
  //   size_t player_len = strlen(server->players[0]);
  //   send(*client_socket, server->players[0], strlen(server->players[0]), 0);
  // }

  // char op[16];
  // ssize_t len = recv(*client_socket, op, 16, 0);
  
  // op[len] = '\0';

  // server->players[server->n_connections - 1] = strdup(op);

  // if (server->n_connections > 1) {
  //   send(server->client_sockets[0], server->players[1], strlen(server->players[1]), 0);    
  // }

  if (server->n_connections == 2) {
    close(server->sd);
  } else {
    Server_wait_for_connections(server);
  }
}

