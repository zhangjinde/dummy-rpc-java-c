#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "blist.h"
#include "hexdump.h"

struct blist new_blist() {
  struct blist list;
  list.len = 0;
  list.head = NULL;
  return list;
}

void blist_push(struct blist *list, const unsigned char *bytes, const size_t len) {
  struct blist_element_t *new_elem = malloc(sizeof(struct blist_element_t));
  new_elem->bytes.head = malloc(sizeof(char) * len);
  memcpy(new_elem->bytes.head, bytes, len);
  new_elem->bytes.len = len;
  new_elem->next = list->head;
  list->head = new_elem;
  list->len += len;
}

struct bytes_t blist_concat(struct blist list) {
  struct bytes_t bytes;
  bytes.len = list.len;
  bytes.head = malloc(sizeof(char) * bytes.len);

  size_t read = 0;

  struct blist_element_t *e = list.head;
  while (e != NULL) {
    memcpy(&bytes.head[list.len - read - e->bytes.len], e->bytes.head, e->bytes.len);
    read += e->bytes.len;
    e = e->next;
  }

  return bytes;
}

size_t blist_recv(int sd, struct blist *list) {
  list->head = NULL;
  size_t len_sum = 0;
  int len;
  unsigned char *buf = malloc(sizeof(unsigned char) * 64);

  while ((len = recv(sd, buf, sizeof(buf), 0)) != 0) {
    if (len < 0) {
      free(buf);
      blist_free(*list);
      printf("!!! recv error\n");
      return 0;
    }

    blist_push(list, buf, len);

    len_sum += len;
  }
  free(buf);

  return len_sum;
}

void blist_free(struct blist list) {
  struct blist_element_t *e = list.head;
  struct blist_element_t *t = NULL;
  while (e != NULL) {
    free(e->bytes.head);
    t = e->next;
    free(e);
    e = t;
  }
  list.head = NULL;
}
