#ifndef DESCRIPTOR_H
#define DESCRIPTOR_H

#include "bytes.h"

#define STREAM_MAGIC 0xaced
#define STREAM_VERSION 5

#define TC_NULL 0x70
#define TC_REFERENCE 0x71
#define TC_CLASSDESC 0x72
#define TC_OBJECT 0x73
#define TC_STRING 0x74
#define TC_ARRAY 0x75
#define TC_CLASS 0x76
#define TC_BLOCKDATA 0x77
#define TC_ENDBLOCKDATA 0x78
#define TC_RESET 0x79
#define TC_BLOCKDATALONG 0x7A
#define TC_EXCEPTION 0x7B
#define TC_LONGSTRING  0x7C
#define TC_PROXYCLASSDESC 0x7D
#define TC_ENUM 0x7E

#define SC_WRITE_METHOD 0x01 //if SC_SERIALIZABLE
#define SC_BLOCK_DATA 0x08    //if SC_EXTERNALIZABLE
#define SC_SERIALIZABLE 0x02
#define SC_EXTERNALIZABLE 0x04
#define SC_ENUM 0x10

#define UTF_HEADER_SIZE 2

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

struct classdata_t {
  union {
    char b;
    char c;
    double d;
    float f;
    int i;
    long j;
    short s;
    char z;
    struct inst *arr;
    struct inst *obj;
  };
  struct classdata_t *next;
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
void append_object_classdata(struct object_t *, struct classdata_t *);
void class_preview(struct class_t *);


#endif /* end of include guard: DESCRIPTOR_H */
