#ifndef DESCRIPTOR_H
#define DESCRIPTOR_H

#include "bytes.h"

#define BYTE 8

#define HANDLE_CLASS 0x00
#define HANDLE_INST 0x01

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

struct handle_t {
  unsigned char type;
  union {
    struct class_t *clazz;
    struct inst *instance;
  } u;
};

struct handle_t *get_handle(unsigned int handle);
unsigned int newHandle_class(struct class_t *clazz);
unsigned int newHandle_inst(struct inst *instance);
void append_class_field(struct class_t *, struct field_t *);
void class_preview(struct class_t *);


#endif /* end of include guard: DESCRIPTOR_H */
