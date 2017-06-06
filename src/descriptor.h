#pragma once

struct desc_field {
  char *name;
  unsigned char type;
  char *type_name;
  // TODO value (union)
  struct desc_field *next;
};

struct desc_class {
  char *name;
  unsigned long uid;
  unsigned char flag;
  struct desc_field *field;
};

void class_desc_preview(struct desc_class *desc);
