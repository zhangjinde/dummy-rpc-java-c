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

void hexdump(const unsigned char *bytes, const size_t len) {
  for (size_t i = 0; i < len; i += 8) {
    const char rawtail = (char)fmin(i + 8, len);
    for (size_t j = 0; j < 8; j++) {
      if (i + j < len) {
        printf("%02x ", bytes[i + j]);
      } else {
        printf("   ");
      }
    }
    printf(" ");
    for (size_t j = 0; j < rawtail - i; j++) {
      printf("%c", bytes[i + j]);
    }
    printf("\n");
  }

}

int main(int argc, char const *argv[]) {
  // if (argc < 2) {
  //   printf("need 1 arg\n");
  //   return 1;
  // }

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  const int sd = socket(AF_INET, SOCK_STREAM, 0);
  connect(sd, (struct sockaddr*)&addr, sizeof(addr));

  const char* requireServiceName = "Task";
  int len = strlen(requireServiceName);

  send(sd, &len, sizeof(len), 0);
  send(sd, requireServiceName, len, 0);

  unsigned char *buf = malloc(sizeof(unsigned char) * 64);

  struct blist *list = NULL, *listp = NULL;
  size_t byteslen = 0;

  while ((len = recv(sd, buf, sizeof(buf), 0)) != 0) {
    if (len < 0) {
      free(buf);
      close(sd);
      printf("!!! recv error\n");
      return 1;
    }

    if (list == NULL) {
      list = listp = malloc(sizeof(struct blist));
    } else {
      listp->next = malloc(sizeof(struct blist));
      listp = listp->next;
    }
    listp->bytes = malloc(sizeof(unsigned char) * len);
    memcpy(listp->bytes, buf, len);
    listp->len = len;
    listp->next = NULL;
    byteslen += len;

    // printf("received len : %d\n", len);

    // for (size_t i = 0; i < len; i++) {
    //   printf("%c", buf[i]);
    // }
    // printf("\n");
    // for (size_t i = 0; i < len; i++) {
    //   printf("%2x ", buf[i]);
    // }
    // printf("\n");
  }

  listp = list;
  unsigned char *bytes = malloc(sizeof(unsigned char) * byteslen);
  size_t lenp = 0;

  while (listp != NULL) {
    memcpy(&bytes[lenp], listp->bytes, listp->len);
    lenp += listp->len;
    listp = listp->next;
  }

  hexdump(bytes, byteslen);

  free(buf);
  close(sd);

  return 0;
}
