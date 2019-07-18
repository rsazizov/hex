#pragma once

#include <stdlib.h>
#include <stdbool.h>

// Implementation of the Disjoint-Set data structure (Union-Find).
// https://en.wikipedia.org/wiki/Disjoint-set_data_structure

typedef struct DisjointSet {
  size_t n;
  size_t* arr;
} DisjointSet;

DisjointSet* DisjointSet_create(size_t n);
void DisjointSet_free(DisjointSet* set);

size_t DisjointSet_size(DisjointSet* set, size_t x);
size_t DisjointSet_find(DisjointSet* set, size_t x);
void DisjointSet_union(DisjointSet* set, size_t x, size_t y);
bool DisjointSet_query(DisjointSet* set, size_t x, size_t y);
