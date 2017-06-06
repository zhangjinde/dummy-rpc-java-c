#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "parser.h"
#include "descriptor.h"
#include "hexdump.h"

#define BYTE 8
#define SERIAL_VERSION_UID_SIZE 8

char *newnstr(const unsigned char *bytes, const size_t len) {
  char *ret = malloc(sizeof(unsigned char) * (len + 1));
  memcpy(ret, bytes, len);
  memset(&ret[len], '\0', 1);
  return ret;
}

size_t parse_utf(char **dest, const unsigned char *bytes) {
  const size_t len = (bytes[0] << BYTE) + bytes[1];
  *dest = newnstr(&bytes[2], len);
  return 2 + len;
}

size_t parse_classdata(struct object_t *object, const unsigned char *bytes, const size_t len) {
  struct class_t *clazz = &object->clazz;
  hexdump("classdata", bytes, len);
  return len;
}

size_t parse_className1(struct inst **instance, const unsigned char *bytes, const size_t len) {
  *instance = malloc(sizeof(struct inst));
  return parse_object(*instance, bytes, len);
}

size_t parse_fieldName(char **dest, const unsigned char *bytes) {
  return parse_utf(dest, bytes);
}

size_t parse_fieldDesc(struct field_t *field, const unsigned char *bytes, const size_t len) {
  hexdump("fieldDesc typecode", &bytes[0], 1);
  field->type = bytes[0];
  size_t read = 0;
  switch (bytes[read++]) {
    case 'B':	// byte
    case 'C':	// char
    case 'D':	// double
    case 'F':	// float
    case 'I':	// integer
    case 'J':	// long
    case 'S':	// short
    case 'Z':	// boolean
      read += parse_fieldName(&field->name, &bytes[read]);
      break;
    case '[':	// array
    case 'L':	// object
      read += parse_fieldName(&field->name, &bytes[read]);
      read += parse_className1(&field->class_name, &bytes[read], len - read);
      break;
  }
  return read;
}

size_t parse_fields(struct class_t *clazz, const unsigned char *bytes, const size_t len) {
  size_t read = 0;
  unsigned short count = (bytes[0] << BYTE) + bytes[1];
  read += 2;
  clazz->field = NULL;
  for (size_t i = 0; i < count; i++) {
    struct field_t *field = malloc(sizeof(struct field_t));
    read += parse_fieldDesc(field, &bytes[read], len - read);
    append_class_field(clazz, field);
  }
  return read;
}

size_t parse_classDescFlags(unsigned char *dest, const unsigned char *bytes) {
  *dest = bytes[0];
  return 1;
}

size_t parse_endBlockData(const unsigned char *bytes) {
  hexdump("classAnnocation TC_ENDBLOCKDATA", bytes, 1);
  return 1;
}

size_t parse_classAnnotation(const unsigned char *bytes) {
  // contents endBlockData
  // 今回はcontentsを省略
  return parse_endBlockData(bytes);
}

size_t parse_superClassDesc(const unsigned char *bytes, const size_t len) {
  struct class_t clazz;
  return parse_classDesc(&clazz, bytes, len);
}

size_t parse_classDescInfo(struct class_t *clazz, const unsigned char *bytes, const size_t len) {
  size_t read = 0;
  read += parse_classDescFlags(&clazz->flag, &bytes[read]);
  read += parse_fields(clazz, &bytes[read], len - read);
  read += parse_classAnnotation(&bytes[read]);
  read += parse_superClassDesc(&bytes[read], len - read);
  return read;
}

size_t parse_serialVersionUID(unsigned long *dest, const unsigned char *bytes) {
  *dest = bytes[0];
  for (size_t i = 0; i < SERIAL_VERSION_UID_SIZE; i++) {
    *dest <<= BYTE;
    *dest += bytes[i];
  }
  return SERIAL_VERSION_UID_SIZE;
}

size_t parse_className(char **dest, const unsigned char *bytes) {
  return parse_utf(dest, bytes);
}

size_t parse_newClassDesc(struct class_t *clazz, const unsigned char *bytes, const size_t len) {
  size_t read = 0;
  switch (bytes[0]) {
  case TC_CLASSDESC:
    hexdump("newClassDesc TC_CLASSDESC", &bytes[read++], 1);
    read += parse_className(&clazz->name, &bytes[read]);
    read += parse_serialVersionUID(&clazz->uid, &bytes[read]);
    // TODO newHandle
    read += parse_classDescInfo(clazz, &bytes[read], len - read);
  case TC_PROXYCLASSDESC:
    printf("not implemented (newClassDesc: %02x)\n", bytes[0]);
  }
  return read;
}

size_t parse_nullRefernce(const unsigned char *bytes) {
  hexdump("nullReference TC_NULL", bytes, 1);
  return 1;
}

size_t parse_classDesc(struct class_t *clazz, const unsigned char *bytes, const size_t len) {
  switch (bytes[0]) {
  case TC_CLASSDESC:
  case TC_PROXYCLASSDESC:
    return parse_newClassDesc(clazz, bytes, len);
    break;
  case TC_NULL:
    return parse_nullRefernce(bytes);
    break;
  case TC_REFERENCE:
    break;
  default:
    printf("not implemented (classDesc: %02x)\n", bytes[0]);
  }

  return 0;
}

size_t parse_newObject(struct object_t *object, const unsigned char *bytes, const size_t len) {
  size_t read = 0;
  hexdump("newObject TC_OBJECT", &bytes[read++], 1);
  // obj.clazz = parse_classDesc(&bytes[1], len - 1);
  read += parse_classDesc(&object->clazz, &bytes[read], len - read);
  // newHandle
  read += parse_classdata(object, &bytes[read], len - read);
  return read;
}

size_t parse_newString(char **str, const unsigned char *bytes, const size_t len) {
  size_t read = 0;
  switch (bytes[read++]) {
    case TC_STRING:
      // newHandle
      read += parse_utf(str, &bytes[read]);
      break;
    case TC_LONGSTRING:
      // newHandle
      printf("not implemented (newString TC_LONGSTRING)\n");
      break;
  }
  return read;
}

size_t parse_prevObject(struct inst *instance, const unsigned char *bytes, const size_t len) {
  size_t read = 0;
  hexdump("prevObject TC_REFERENCE", &bytes[read++], 1);
  hexdump("prevObject handle", &bytes[read], 4);
  printf("not implemented (prevObject TC_REFERENCE)\n");
  return read + 4;
}

size_t parse_object(struct inst *instance, const unsigned char *bytes, const size_t len) {
  size_t read = 0;
  switch (bytes[0]) {
  case TC_OBJECT:
    instance->type = TC_OBJECT;
    read += parse_newObject(&instance->u.object, bytes, len);
    break;
  case TC_REFERENCE:
    read += parse_prevObject(instance, bytes, len);
    break;
  case TC_STRING:
    instance->type = TC_STRING;
    read += parse_newString(&instance->u.str, bytes, len);
    break;
  default:
    printf("not implemented (object: 0x%02x)\n", bytes[0]);
  }
  return read;
}

struct inst parse(const unsigned char *bytes, const size_t len) {

  hexdump("magic", &bytes[0], 2);
  hexdump("version", &bytes[2], 2);

  // ほんとうならここは parse_contents だが今回はショートカット
  struct inst instance;
  parse_object(&instance, &bytes[4], len - 4);

  return instance;
}
