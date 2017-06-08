#include "person.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct person_t new_person() {
  struct person_t person;
  person.name = malloc(sizeof(char) * 5);
  memcpy(person.name, "Jobs", 5);
  return person;
}

void preview_person(const struct person_t person) {
  printf("person.name: %s\n", person.name);
}
