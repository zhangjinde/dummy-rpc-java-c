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
#include "src/bytes.h"
#include "src/parser.h"
#include "src/descriptor.h"

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

  struct bytes_t bytes;
  struct blist *list;
  bytes.len = blist_recv(sd, &list);

  if (bytes.len == 0) {
    close(sd);
    return 1;
  }

  bytes.head = malloc(sizeof(unsigned char) * bytes.len);
  blist_concat(bytes.head, list);
  // TODO blist_free

  hexdump("received", bytes.head, bytes.len);

  struct inst obj = parse(bytes.head, bytes.len);

  close(sd);

  return 0;
}
