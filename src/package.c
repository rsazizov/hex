#include "package.h"

#include <unistd.h>

#ifndef __USE_XOPEN2K
#define __USE_XOPEN2K
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <string.h>
#include <stdio.h>
#include <ncurses.h>

#define OP_MOVE_SIZE 8
#define OP_MOVE_BODY_SIZE 6

void send_package(int socket, Package package) {
  // printf("Sending pkg: %s\n", package.op);

  if (!strcmp(package.op, OP_MOVE)) {    
    char content[OP_MOVE_SIZE + 1];

    sprintf(content, "%s %.2d %.2d", OP_MOVE, package.y, package.x);

    send(socket, content, OP_MOVE_SIZE, 0);
  } else if (!strcmp(package.op, OP_NAME)) {
    send(socket, package.op, HEADER_SIZE, 0);

    uint32_t len = htonl(strlen(package.name));

    send(socket, &len, sizeof(len), 0);
    send(socket, package.name, strlen(package.name), 0);
  } else {
    send(socket, package.op, HEADER_SIZE, 0);
  }
}

Package recv_package(int socket) {
  // TODO: op memory leak.
  char op[HEADER_SIZE + 1];
  recv(socket, op, HEADER_SIZE, 0);
  op[HEADER_SIZE] = '\0';

  // printf("recv_package(): %s\n", op);

  Package pkg = {
    NULL,
    -1,
    -1
  };

  pkg.op = strdup(op);
  
  op[HEADER_SIZE] = '\0';

  if (!strcmp(op, OP_MOVE)) {
    char buff[OP_MOVE_BODY_SIZE];
    recv(socket, buff, OP_MOVE_BODY_SIZE, 0);

    buff[OP_MOVE_BODY_SIZE] = '\0';

    int x, y;
    // printf("recv_package(): %s\n", buff);
    sscanf(buff, "%d %d", &y, &x);
    
    pkg.y = y;
    pkg.x = x;
  } else if (!strcmp(op, OP_NAME)) {
    uint32_t len;
    recv(socket, &len, sizeof(len), 0);

    len = ntohl(len);

    char buff[len + 1];
    recv(socket, buff, len, 0);
    buff[len] = '\0';

    pkg.name = strdup(buff);
  }

  return pkg;
}
