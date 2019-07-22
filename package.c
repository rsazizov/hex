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

void send_package(int socket, Package package) {
  // printf("Sending pkg: %s\n", package.op);

  if (!strcmp(package.op, OP_MOVE)) {    
    char content[10] = { " " };

    sprintf(content, "%s %.2d %.2d", OP_MOVE, package.y, package.x);

    content[8] = '\0';
    // printf("Sending: %s\n", content);

    send(socket, content, 8, 0);
  } else {
    send(socket, package.op, HEADER_SIZE, 0);
  }
}

Package recv_package(int socket) {
  char op[HEADER_SIZE + 1];
  recv(socket, op, 2, 0);
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
    // TODO: this code is garbage
    char buff[10];
    recv(socket, buff, 6, 0);
    buff[6] = '\0';

    int x, y;
    // printf("recv_package(): %s\n", buff);
    sscanf(buff, "%d %d", &y, &x);
    pkg.y = y;
    pkg.x = x;
  }

  return pkg;
}
