#ifndef BLIST_H
#define BLIST_H

#include <stdio.h>
#include <stdlib.h>
#include "bytes.h"

struct blist_element_t {
  struct bytes_t bytes;
  struct blist_element_t *next;
};

struct blist {
  struct blist_element_t *head;
  size_t len;
};

struct blist new_blist();
void blist_push(struct blist *list, const unsigned char *bytes, const size_t len);
struct bytes_t blist_concat(struct blist list);
size_t blist_recv(int sd, struct blist *list);
void blist_free(struct blist list);

#endif /* end of include guard: BLIST_H */
