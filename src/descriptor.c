#include <stdio.h>
#include "descriptor.h"


void class_desc_preview(struct desc_class *desc) {
  puts("class_desc_preview");
  printf("name: %s\n", desc->name);
  printf("uid: %lx\n", desc->uid);
  printf("flag: %02x\n", desc->flag);
  if (desc->field != NULL) {
    struct desc_field *f = desc->field;
    while (f != NULL) {
      puts("field:");
      printf("  name: %s\n", f->name);
      printf("  type: %c\n", f->type);
      printf("  type_name: %s\n", f->type_name == NULL ? "null" : f->type_name);
      f = f->next;
    }
  }
}
