#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "blist.h"
#include "hexdump.h"
#include "parser.h"
#include "net.h"
#include "bytes.h"


int net_connect() {
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  const int sd = socket(AF_INET, SOCK_STREAM, 0);
  connect(sd, (struct sockaddr*)&addr, sizeof(addr));
  return sd;
}

struct inst net_fetch(const char *service_name) {
  size_t command_len = 7 + strlen(service_name);
  char *command = malloc(sizeof(char) * command_len);
  memset(command, '\0', command_len);
  strcat(command, "fetch ");
  strcat(command, service_name);

  int sd = net_connect();
  send(sd, command, command_len, 0);
  free(command);

  struct blist list = new_blist();
  size_t received_len = blist_recv(sd, &list);
  close(sd);

  if (received_len == 0) {
    blist_free(list);
    exit(1);
  }

  struct bytes_t bytes = blist_concat(list);
  blist_free(list);

  hexdump("received", bytes.head, bytes.len);

  return parse(bytes.head, bytes.len);
}

void net_call(const char *service_name, const char *method_name, struct bytes_t arg_bytes) {
  size_t command_len = 7 + strlen(service_name) + strlen(method_name);
  char *command = malloc(sizeof(char) * command_len);
  memset(command, '\0', command_len);
  strcat(command, "call ");
  strcat(command, service_name);
  strcat(command, " ");
  strcat(command, method_name);
  struct bytes_t arg_bytes_len = bytes_from_int(arg_bytes.len);
  printf("arg_bytes.len: %ld, %lx\n", arg_bytes.len, arg_bytes.len);

  int sd = net_connect();
  send(sd, command, command_len, 0);
  send(sd, arg_bytes_len.head, arg_bytes_len.len, 0);
  send(sd, arg_bytes.head, arg_bytes.len, 0);
  free(command);
  close(sd);
}
