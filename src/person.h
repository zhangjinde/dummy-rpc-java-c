#ifndef PERSON_H
#define PERSON_H

#define PERSON_T_SERIAL_VERSION 0x1122334455667788

struct person_t {
  char *name;
};

void preview_person(const struct person_t);

#endif /* end of include guard: PERSON_H */
