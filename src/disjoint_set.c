#include "disjoint_set.h"

#include <assert.h>

DisjointSet* DisjointSet_create(size_t n) {
  DisjointSet* set = malloc(sizeof(DisjointSet));
  
  set->n = n;
  set->arr = calloc(n, sizeof(*set->arr));
  
  DisjointSet_reset(set);

  return set;
}

void DisjointSet_free(DisjointSet* set) {
  free(set->arr);
  free(set);
}

void DisjointSet_reset(DisjointSet* set) {
  for (size_t i = 0; i < set->n; ++i) {
    set->arr[i] = i;
  }
}

size_t DisjointSet_size(DisjointSet* set, size_t x) {
  assert(set);

  size_t size = 0;

  for (size_t i = 0; i < set->n; ++i) {
    if (set->arr[i] == x) size++;
  }

  return size;
}

size_t DisjointSet_find(DisjointSet* set, size_t x) {
  assert(set);

  size_t prev;

  while (set->arr[x] != x) {
    prev = x;
    x = set->arr[x];
    set->arr[prev] = set->arr[x];
  }

  return x;
}

void DisjointSet_union(DisjointSet* set, size_t x, size_t y) {
  assert(set);

  size_t x_root = DisjointSet_find(set, x);
  size_t y_root = DisjointSet_find(set, y);

  if (x_root == y_root) return;

  size_t x_root_size = DisjointSet_size(set, x);
  size_t y_root_size = DisjointSet_size(set, y);

  if (x_root_size < y_root_size) {
    int tmp = x_root;
    x_root = y_root;
    y_root = tmp;
  }

  set->arr[y_root] = x_root;
}

bool DisjointSet_query(DisjointSet* set, size_t x, size_t y) {
  assert(set);

  return DisjointSet_find(set, x) == DisjointSet_find(set, y); 
}