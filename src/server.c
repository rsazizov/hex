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
  
  if (strlen(server->port) != 4) {
    return false;
  }

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
    printw("getaddrinfo failed: %d\n", status);
    return false;
  }

  server->sd = socket(servinfo->ai_family, servinfo->ai_socktype,
                    servinfo->ai_protocol);  

  int yes = 1;
  if (setsockopt(server->sd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
      printw("setsockopt failed: %d\n", errno);
      return false;
  } 

  if (bind(server->sd, servinfo->ai_addr, servinfo->ai_addrlen)) {
    printw("bind failed: %d\n", errno);
    return false;
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
    int sid = board->current_player - 1;
    int socket = server->client_sockets[sid];
    int other_socket = server->client_sockets[!sid];
    
    send_package(socket, (Package) {
      OP_MOVE_REQUEST, -1, -1
    });
    
    Package p = recv_package(socket);
    assert(!strcmp(p.op, OP_MOVE));

    send_package(other_socket, p);

    // This should never fail
    assert(Board_make_move(board, p.y, p.x));
  }

  int winner_socket = -1;
  int loser_socket = -1;

  if (board->winner == 1) {
    winner_socket = 0;
    loser_socket = 1;
  } else {
    winner_socket = 1;
    loser_socket = 0;
  }

  // Package win_pkg;
  // win_pkg.op = OP_WIN;
  // send_package(winner_socket, win_pkg);

  // Package lose_pkg;
  // lose_pkg.op = OP_LOSE;

  // send_package(lost)

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

  *client_socket = accept(server->sd, (struct sockaddr*) &in_addr, &addr_size);

  char hbuf[32], sbuf[32];

  getnameinfo((struct sockaddr*) &in_addr, addr_size, hbuf, 
      sizeof(hbuf), sbuf, sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV);

  Package pkg;

  pkg = recv_package(*client_socket);
  assert(!strcmp(pkg.op, OP_NAME));

  server->players[server->n_connections - 1] = pkg.name;

  if (server->n_connections > 1 ) {
    Package op_name_pkg;
    op_name_pkg.op = OP_NAME;
    op_name_pkg.name = server->players[0];

    send_package(*client_socket, op_name_pkg);
  }

  if (server->n_connections == 2) {
    close(server->sd);
    Package name_pkg;
    name_pkg.op = OP_NAME;
    name_pkg.name = server->players[1];
    
    send_package(server->client_sockets[0], name_pkg);
  } else {
    Server_wait_for_connections(server);
  }
}

