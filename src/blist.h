#ifndef BLIST_H
#define BLIST_H

#include <stdio.h>
#include <stdlib.h>
#include "bytes.h"

struct blist {
  struct bytes_t bytes;
  struct blist *next;
};

struct blist *blist_push(struct blist *list, const unsigned char *bytes, const size_t len);
struct bytes_t blist_concat(struct blist *list, size_t total_len);
size_t blist_recv(int sd, struct blist **list);
void blist_free(struct blist *list);

#endif /* end of include guard: BLIST_H */
