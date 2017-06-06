#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "blist.h"

struct blist *blist_append(struct blist *tail, const unsigned char *bytes, const size_t len) {
  struct blist *new_one = malloc(sizeof(struct blist));
  new_one->bytes = malloc(sizeof(unsigned char) * len);
  memcpy(new_one->bytes, bytes, len);
  new_one->len = len;
  new_one->next = NULL;
  if (tail != NULL) { // concat list if provided previous one
    tail->next = new_one;
  }
  return new_one;
}

void blist_concat(unsigned char *dest, struct blist *list) {
  struct blist *listp = list;
  size_t lenp = 0;
  while (listp != NULL) {
    memcpy(&dest[lenp], listp->bytes, listp->len);
    lenp += listp->len;
    listp = listp->next;
  }
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
