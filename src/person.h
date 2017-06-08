#ifndef PERSON_H
#define PERSON_H

#include "descriptor.h"

#define PERSON_T_SERIAL_VERSION 0x1122334455667788

struct person_t {
  char *name;
};

struct person_t new_person();
void preview_person(const struct person_t);
struct inst inter_serialize_person(const struct person_t person);

#endif /* end of include guard: PERSON_H */
