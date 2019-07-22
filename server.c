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

#include "board.h"
#include "package.h"

Server* Server_create(const char* port) {
  Server* server = malloc(sizeof(Server));
  server->running = false;

  assert(strlen(port) == 4);

  server->service = strdup(port);
  server->players[0] = NULL;
  server->players[1] = NULL;

  server->s = -1;
  server->client_sockets[0] = -1;
  server->client_sockets[1] = -1;
  
  return server;
}

void Server_free(Server* server) {
  if (server->running) {
    Server_close(server);
  }

  free(server);
}

bool Server_start(Server* server) {
  // TODO: Proper error handling
  int status;

  struct addrinfo hints;
  struct addrinfo* servinfo;

  memset(&hints, 0, sizeof(hints));

  hints.ai_family = AF_UNSPEC;
  hints.ai_flags = AI_PASSIVE;
  hints.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(NULL, server->service, &hints, &servinfo);

  if (status) {
    printf("Error %d\n", status);
    exit(1);
  }

  server->s = socket(servinfo->ai_family, servinfo->ai_socktype,
                    servinfo->ai_protocol);  

  // TODO: Check s

  int yes = 1;
  if (setsockopt(server->s, SOL_SOCKET,SO_REUSEADDR ,&yes, sizeof yes) == -1) {
      perror("setsockopt");
      exit(1);
  } 

  if (bind(server->s, servinfo->ai_addr, servinfo->ai_addrlen)) {
    printf("bind failed\n");
    exit(1);
  }

  return true;
}

void Server_close(Server* server) {
  close(server->client_sockets[0]);
  close(server->client_sockets[1]);

  // TODO: free players
}

void Server_loop(Server* server) {
  assert(server->n_connections == 2);

  // At this point there has been a name exchange between the
  // clients. The server should send a move request to the first
  // client(?), update the board, notify the other player, etc...

  Board* board = Board_create(NULL);

  while (!Board_is_game_over(board)) {
    // TODO: better switching
    // TODO: fix MVU thing...
    int sid = board->current_player - 1;
    int socket = server->client_sockets[sid];
    int other_socket = server->client_sockets[!sid];
    

    send_package(socket, (Package) {
      OP_MOVE_REQUEST, -1, -1
    });
    
    Package p = recv_package(socket);

    printf("Player %d makes a move(%d, %d)\n", 
            board->current_player, p.y, p.x);

    send_package(other_socket, p);

    assert(Board_make_move(board, p.y, p.x));
  }

  Board_free(board);
}

void Server_wait_for_connection(Server* server) {
  listen(server->s, 2);
  
  assert(server->n_connections <= 2);
  server->n_connections++;

  int* client_socket = 
        &server->client_sockets[server->n_connections - 1];

  struct sockaddr_storage in_addr;

  socklen_t addr_size = sizeof(in_addr);
  
  printf("Waiting for connections on %s\n", server->service);

  *client_socket = accept(server->s, (struct sockaddr*) &in_addr, &addr_size);
  //close(server->s);

  printf("Got connection from idk\n");

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

  if (server->n_connections < 2) {
    Server_wait_for_connection(server);
  }
}

bool Server_make_move(int player, int y, int x) {
  return false;
}

char* Server_get_opponent_name(Server* server) {
  return NULL;
}
