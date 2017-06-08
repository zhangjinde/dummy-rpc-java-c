#include "serializer.h"
#include "blist.h"

struct bytes_t serialize(struct inst instance) {
  struct blist list = new_blist();
  return blist_concat(list);
}
