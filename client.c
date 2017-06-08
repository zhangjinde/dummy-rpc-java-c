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
#include "src/task.h"
#include "src/person.h"
#include "src/serializer.h"

#define PORT 8100

struct person_t person_from_task(const struct task_t task) {
  struct person_t person = new_person();
  size_t person_name_len = strlen(task.str1) + strlen(task.str2) + 1;
  free(person.name);
  person.name = malloc(sizeof(char) * person_name_len);
  memset(person.name, '\0', person_name_len);
  strcat(person.name, task.str1);
  strcat(person.name, task.str2);
  return person;
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

  struct blist list = new_blist();
  size_t received_len = blist_recv(sd, &list);
  close(sd);
  if (received_len == 0) {
    return 1;
  }

  struct bytes_t received_bytes = blist_concat(list);
  blist_free(list);

  hexdump("received", received_bytes.head, received_bytes.len);


  struct inst instance_task = parse(received_bytes.head, received_bytes.len);
  struct task_t task = cast_task(instance_task);
  preview_task(task);
  struct person_t person = person_from_task(task);
  preview_person(person);

  // TODO taskのhelloを呼ぶようにする
  struct inst instance_person = inter_serialize_person(person);
  struct bytes_t serial = serialize(instance_person);


  return 0;
}
