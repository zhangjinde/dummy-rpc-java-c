#ifndef TASK_H
#define TASK_H

#include "person.h"
#include "descriptor.h"

struct task_t {
  int v;
  char *str1;
  char *str2;
  char x;
  void (*hello)(const struct task_t *, struct person_t);
};

struct task_t cast_task(struct inst);
struct task_t new_task();
void task_hello(const struct task_t *self, struct person_t);
void preview_task(const struct task_t);

#endif /* end of include guard: TASK_H */
