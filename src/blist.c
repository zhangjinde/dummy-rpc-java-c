#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "blist.h"

struct blist *blist_append(struct blist *tail, const unsigned char *bytes, const size_t len) {
  struct blist *new_one = malloc(sizeof(struct blist));
  new_one->bytes.head = malloc(sizeof(char) * len);
  memcpy(new_one->bytes.head, bytes, len);
  new_one->bytes.len = len;
  new_one->next = NULL;
  if (tail != NULL) { // concat list if provided previous one
    tail->next = new_one;
  }
  return new_one;
}

struct bytes_t blist_concat(struct blist *list, size_t total_len) {
  if (total_len == 0) {
    // TODO calc total_len
  }

  struct bytes_t bytes;
  bytes.len = total_len;
  bytes.head = malloc(sizeof(char) * bytes.len);

  size_t read = 0;

  while (list != NULL) {
    memcpy(&bytes.head[read], list->bytes.head, list->bytes.len);
    read += list->bytes.len;
    list = list->next;
  }

  return bytes;
}

size_t blist_recv(int sd, struct blist **list) {
  *list = NULL;
  struct blist *listp = NULL;
  size_t len_sum = 0;
  int len;
  unsigned char *buf = malloc(sizeof(unsigned char) * 64);

  while ((len = recv(sd, buf, sizeof(buf), 0)) != 0) {
    if (len < 0) {
      free(buf);
      printf("!!! recv error\n");
      return 0;
    }

    listp = blist_append(listp, buf, len);
    if (*list == NULL) {
      *list = listp;
    }

    len_sum += len;
  }
  free(buf);

  return len_sum;
}

void blist_free(struct blist *list) {
  while (list != NULL) {
    free(list->bytes.head);
    list = list->next;
  }
}
