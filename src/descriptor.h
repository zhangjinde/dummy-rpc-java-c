#ifndef DESCRIPTOR_H
#define DESCRIPTOR_H

#include "bytes.h"

struct field_t {
  char *name;
  unsigned char type;
  struct inst *class_name;
  struct field_t *next;
};

struct class_t {
  char *name;
  unsigned long uid;
  unsigned char flag;
  struct field_t *field;
};

struct classdata_t {
  struct bytes_t bytes;
  struct classdata_t *next;
};

struct object_t {
  struct class_t clazz;
  struct classdata_t *classdata;
};

struct inst {
  unsigned char type;
  union {
    struct object_t object;
    char *str;
  } u;
};

void append_class_field(struct class_t *, struct field_t *);
void class_preview(struct class_t *);


#endif /* end of include guard: DESCRIPTOR_H */
