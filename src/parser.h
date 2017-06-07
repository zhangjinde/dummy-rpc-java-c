#ifndef PARSER_H
#define PARSER_H

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


#include <stdio.h>
#include "descriptor.h"

struct inst parse(const unsigned char *bytes, const size_t len);
size_t parse_object(struct inst **instance, const unsigned char *bytes, const size_t len);
size_t parse_classDesc(struct class_t *clazz, const unsigned char *bytes, const size_t len);

#endif /* end of include guard: PARSER_H */
