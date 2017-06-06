#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <math.h>

#define PORT 8100

struct blist {
  unsigned char *bytes;
  size_t len;
  struct blist *next;
};

void hexdump(char *desc, void *addr, int len) {
  int i;
  unsigned char buff[17];
  unsigned char *pc = (unsigned char*)addr;
  if (desc != NULL)
    printf ("%s:\n", desc);
  if (len == 0) {
    printf("  ZERO LENGTH\n");
    return;
  }
  if (len < 0) {
    printf("  NEGATIVE LENGTH: %i\n",len);
    return;
  }
  for (i = 0; i < len; i++) {
    if ((i % 16) == 0) {
      if (i != 0)
        printf ("  %s\n", buff);
      printf ("  %04x ", i);
    }
    printf (" %02x", pc[i]);
    if ((pc[i] < 0x20) || (pc[i] > 0x7e))
      buff[i % 16] = '.';
    else
      buff[i % 16] = pc[i];
    buff[(i % 16) + 1] = '\0';
  }
  while ((i % 16) != 0) {
    printf ("   ");
    i++;
  }
  printf ("  %s\n", buff);
}

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

int main(int argc, char const *argv[]) {
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  const int sd = socket(AF_INET, SOCK_STREAM, 0);
  connect(sd, (struct sockaddr*)&addr, sizeof(addr));

  const char* command = "fetch Task\n";
  send(sd, command, strlen(command), 0);

  struct blist *list;
  size_t byteslen = blist_recv(sd, &list);

  if (byteslen == 0) {
    close(sd);
    return 1;
  }

  unsigned char *bytes = malloc(sizeof(unsigned char) * byteslen);
  blist_concat(bytes, list);
  hexdump("received", bytes, byteslen);

  close(sd);

  return 0;
}
