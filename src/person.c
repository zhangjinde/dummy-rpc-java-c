#include "person.h"
#include "strutil.h"
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

struct inst inter_serialize_person(const struct person_t person) {
  struct inst instance;
  instance.type = TC_OBJECT;
  instance.u.object.clazz.name = newstr("Person");
  instance.u.object.clazz.uid = PERSON_T_SERIAL_VERSION;
  instance.u.object.clazz.flag = SC_SERIALIZABLE;
  instance.u.object.clazz.field = NULL;
  instance.u.object.classdata = NULL;

  struct inst *javastring = malloc(sizeof(struct inst));
  javastring->type = TC_STRING;
  javastring->u.str = newstr("Ljava/lang/String;");
  struct field_t *field = malloc(sizeof(struct field_t));
  field->name = newstr("name");
  field->type = 'L';
  field->class_name = javastring;
  append_class_field(&instance.u.object.clazz, field);

  struct inst *strinst = malloc(sizeof(struct inst));
  strinst->type = TC_STRING;
  strinst->u.str = newstr(person.name);
  struct classdata_t *classdata = malloc(sizeof(struct classdata_t));
  classdata->obj = strinst;
  append_object_classdata(&instance.u.object, classdata);

  return instance;
}
