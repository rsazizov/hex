#pragma once

#include <stdbool.h>

// Headers

#define HEADER_SIZE 2

#define OP_MOVE_REQUEST "MR"
#define OP_MOVE "MV"
#define OP_WIN "WN"
#define OP_LOSE "LS"

typedef struct {
  char* op;

  int y;
  int x;

} Package;

void send_package(int socket, Package package);
Package recv_package(int socket);
