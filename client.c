#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/task.h"
#include "src/person.h"
#include "src/net.h"

struct person_t create_person_from_task(const struct task_t task) {
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
  struct inst abstract = net_fetch("Task");
  struct task_t task = cast_task(abstract);
  preview_task(task);
  struct person_t person = create_person_from_task(task);
  preview_person(person);
  task.hello(&task, person);
  return 0;
}
