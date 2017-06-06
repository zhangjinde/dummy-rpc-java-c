#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8100

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
  char len = strlen(requireServiceName);

  send(sd, &len, sizeof(len), 0);
  send(sd, requireServiceName, strlen(requireServiceName), 0);

  // for (size_t i = 0; i < len; i++) {
  //   printf("%d\n", requireServiceName[i]);
  // }


  char *buf = malloc(sizeof(char) * 2048);
  while ((len = recv(sd, buf, sizeof(buf), 0)) != 0) {
    if (len < 0) {
      free(buf);
      close(sd);
      return 1;
    }

    printf("received len : %d\n", len);
    for (size_t i = 0; i < len; i++) {
      printf("%4d", buf[i]);
    }
    printf("\n");
  }


  free(buf);
  close(sd);

  return 0;
}
