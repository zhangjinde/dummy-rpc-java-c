#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "parser.h"
#include "descriptor.h"
#include "hexdump.h"
#include "strutil.h"

#define SERIAL_VERSION_UID_SIZE 8

size_t read_bytes_1(char *dest, const unsigned char *bytes) {
  *dest = bytes[0];
  return 1;
}

size_t read_bytes_4(int *dest, const unsigned char *bytes) {
  size_t size = 4;
  *dest = 0;
  for (size_t i = 0; i < size; i++) {
    *dest <<= BYTE;
    *dest += bytes[i];
  }
  return size;
}

size_t parse_utf(char **dest, const unsigned char *bytes) {
  // const size_t len = (bytes[0] << BYTE) + bytes[1];
  size_t len = 0;
  for (size_t i = 0; i < UTF_HEADER_SIZE; i++) {
    len <<= BYTE;
    len += bytes[i];
  }
  *dest = newnstr(&bytes[UTF_HEADER_SIZE], len);
  // hexdump("utf", &bytes[UTF_HEADER_SIZE], len);
  return UTF_HEADER_SIZE + len;
}

size_t parse_classdata(struct object_t *object, const unsigned char *bytes, const size_t len) {
  object->classdata = NULL;

  size_t read = 0;
  struct field_t *field = object->clazz.field;
  while (field != NULL) {
    struct classdata_t *classdata = malloc(sizeof(struct classdata_t));

    switch (field->type) {
    case 'B':	// byte
      read += read_bytes_1(&classdata->b, &bytes[read]);
      break;
    // case 'C':	// char
    // case 'D':	// double
    // case 'F':	// float
    case 'I':	// integer
      read += read_bytes_4(&classdata->i, &bytes[read]);
      break;
    // case 'J':	// long
    // case 'S':	// short
    // case 'Z':	// boolean
    // case '[':	// array
    case 'L':	// object
      read += parse_object(&classdata->obj, &bytes[read], len - read);
      break;
    default:
      printf("not implemented (classdata type %c)\n", field->type);
    }

    append_object_classdata(object, classdata);
    field = field->next;
  }

  // hexdump("classdata (read)", bytes, read);
  return read;
}

size_t parse_className1(struct inst **instance, const unsigned char *bytes, const size_t len) {
  return parse_object(instance, bytes, len);
}

size_t parse_fieldName(char **dest, const unsigned char *bytes) {
  return parse_utf(dest, bytes);
}

size_t parse_fieldDesc(struct field_t *field, const unsigned char *bytes, const size_t len) {
  // hexdump("fieldDesc typecode", &bytes[0], 1);
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
  // hexdump("field (read)", bytes, read);
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
  // hexdump("fields (read)", bytes, read);
  return read;
}

size_t parse_classDescFlags(unsigned char *dest, const unsigned char *bytes) {
  *dest = bytes[0];
  return 1;
}

size_t parse_endBlockData(const unsigned char *bytes) {
  // hexdump("classAnnotation TC_ENDBLOCKDATA", bytes, 1);
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
  // printf("classDescInfo %x\n", bytes[0]);
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
  switch (bytes[read++]) {
  case TC_CLASSDESC:
    // hexdump("newClassDesc TC_CLASSDESC", &bytes[read], 1);
    read += parse_className(&clazz->name, &bytes[read]);
    read += parse_serialVersionUID(&clazz->uid, &bytes[read]);
    unsigned short handle = newHandle_class(clazz);
    printf("newHandle %x\n", handle);
    read += parse_classDescInfo(clazz, &bytes[read], len - read);
    break;
  case TC_PROXYCLASSDESC:
    printf("not implemented (newClassDesc: TC_PROXYCLASSDESC)\n");
    break;
  }
  // hexdump("newClassDesc (read)", bytes, read);
  return read;
}

size_t parse_nullRefernce(const unsigned char *bytes) {
  // hexdump("nullReference TC_NULL", bytes, 1);
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
    printf("not implemented (classDesc: TC_REFERENCE)\n");
    break;
  default:
    printf("not implemented (classDesc: %02x)\n", bytes[0]);
  }

  return 0;
}

size_t parse_newObject(struct inst **instance_, const unsigned char *bytes, const size_t len) {
  struct inst *instance = *instance_ = malloc(sizeof(struct inst));

  instance->type = TC_OBJECT;
  // hexdump("newObject TC_OBJECT", &bytes[0], 1);

  size_t read = 1;
  read += parse_classDesc(&instance->u.object.clazz, &bytes[read], len - read);
  unsigned short handle = newHandle_inst(instance);
  printf("newHandle %x\n", handle);
  read += parse_classdata(&instance->u.object, &bytes[read], len - read);
  return read;
}

size_t parse_newString(struct inst **instance_, const unsigned char *bytes, const size_t len) {
  struct inst *instance = *instance_ = malloc(sizeof(struct inst));

  instance->type = TC_STRING;
  // hexdump("newString TC_STRING", &bytes[0], 1);

  unsigned short handle;

  size_t read = 1;
  switch (bytes[0]) {
    case TC_STRING:
      handle = newHandle_inst(instance);
      printf("newHandle %x\n", handle);
      read += parse_utf(&instance->u.str, &bytes[read]);
      break;
    case TC_LONGSTRING:
      // newHandle
      printf("not implemented (newString TC_LONGSTRING)\n");
      break;
  }
  return read;
}

size_t parse_handle(struct handle_t **handle, const unsigned char *bytes) {
  unsigned int index = 0;
  for (size_t i = 0; i < 4; i++) {
    index <<= BYTE;
    index += bytes[i];
  }
  printf("handle %x\n", index);
  *handle = get_handle(index);
  return 4;
}

size_t parse_prevObject(struct inst **instance, const unsigned char *bytes, const size_t len) {
  // hexdump("prevObject TC_REFERENCE", &bytes[0], 1);

  size_t read = 1;
  struct handle_t *handle;
  read += parse_handle(&handle, &bytes[1]);
  *instance = handle->u.instance;
  return read;
}

size_t parse_object(struct inst **instance, const unsigned char *bytes, const size_t len) {
  size_t read = 0;
  switch (bytes[0]) {
  case TC_OBJECT:
    read += parse_newObject(instance, bytes, len);
    break;
  case TC_REFERENCE:
    read += parse_prevObject(instance, bytes, len);
    break;
  case TC_STRING:
    read += parse_newString(instance, bytes, len);
    break;
  default:
    printf("not implemented (object: 0x%02x)\n", bytes[0]);
  }
  return read;
}

struct inst parse(const unsigned char *bytes, const size_t len) {
  size_t read = 0;

  // hexdump("magic", &bytes[read], 2);
  read += 2;
  // hexdump("version", &bytes[read], 2);
  read += 2;

  // ほんとうならここは parse_contents だが今回はショートカット
  struct inst *instance;
  read += parse_object(&instance, &bytes[read], len - read);

  printf("len: %ld, read: %ld\n", len, read);

  return *instance;
}
