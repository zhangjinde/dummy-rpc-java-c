#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <math.h>

#include "src/hexdump.h"
#include "src/blist.h"

#define PORT 8100


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

  // dump(bytes, byteslen);

  close(sd);

  return 0;
}
