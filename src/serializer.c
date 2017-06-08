#include "serializer.h"
#include "blist.h"

struct bytes_t serialize(struct inst instance) {
  struct blist list = new_blist();



  struct bytes_t ret = blist_concat(list);
  blist_free(list);
  return ret;
}
