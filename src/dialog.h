#pragma once

#include <stddef.h>

#define MAX_NAME_LEN 16

void dialog_show(const char* txt, size_t max_len, char** out);