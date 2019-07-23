#pragma once

#include <stdbool.h>

// Headers

#define HEADER_SIZE 2

#define OP_MOVE_REQUEST "MR"
#define OP_MOVE "MV"
#define OP_WIN "WN"
#define OP_LOSE "LS"
#define OP_NAME "NM"

typedef struct {
  char* op;

  // For OP_MOVE
  int y;
  int x;

  // For OP_NAME
  char* name;

} Package;

void send_package(int socket, Package package);
Package recv_package(int socket);
