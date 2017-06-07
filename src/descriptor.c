#include <stdio.h>
#include <stdlib.h>
#include "descriptor.h"

#define HANDLE_PREFIX (0x7e << (2 * BYTE))

struct handle_t *handle_list[128];
unsigned char handle_stack = 0;

struct handle_t *get_handle(unsigned int handle) {
  handle = handle - 0x7e0000;
  return handle_list[handle];
}

unsigned int newHandle_class(struct class_t *clazz) {
  struct handle_t *handle = malloc(sizeof(struct handle_t));
  handle->type = HANDLE_CLASS;
  handle->u.clazz = clazz;
  handle_list[handle_stack] = handle;
  return HANDLE_PREFIX + handle_stack++;
}

unsigned int newHandle_inst(struct inst *instance) {
  struct handle_t *handle = malloc(sizeof(struct handle_t));
  handle->type = HANDLE_INST;
  handle->u.instance = instance;
  handle_list[handle_stack] = handle;
  return HANDLE_PREFIX + handle_stack++;
}

void append_class_field(struct class_t *clazz, struct field_t *field) {
  field->next = NULL;
  if (clazz->field == NULL) {
    clazz->field = field;
  } else {
    struct field_t *f = clazz->field;
    while (f->next != NULL) {
      f = f->next;
    }
    f->next = field;
  }
}

void class_preview(struct class_t *clazz) {
  puts("dclass_preview");
  printf("name: %s\n", clazz->name);
  printf("uid: %lx\n", clazz->uid);
  printf("flag: %02x\n", clazz->flag);
  if (clazz->field != NULL) {
    struct field_t *f = clazz->field;
    while (f != NULL) {
      puts("field:");
      printf("  name: %s\n", f->name);
      printf("  type: %c\n", f->type);
      printf("  type_name: %s\n", f->class_name == NULL ? "null" : f->class_name->u.str);
      f = f->next;
    }
  }
}
