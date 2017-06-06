#include <stdio.h>
#include "descriptor.h"

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
