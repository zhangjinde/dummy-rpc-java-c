#include "bytes.h"
#include <string.h>
#include <stdlib.h>
#include "descriptor.h"

struct bytes_t bytes_from_char(unsigned char from) {
  struct bytes_t bytes;
  bytes.len = 1;
  bytes.head = malloc(sizeof(char) * bytes.len);
  bytes.head[0] = from;
  return bytes;
}

struct bytes_t bytes_from_short(unsigned short from) {
  struct bytes_t bytes;
  bytes.len = 2;
  bytes.head = malloc(sizeof(char) * bytes.len);
  for (size_t i = 1; i <= bytes.len; i++) {
    bytes.head[bytes.len - i] = (unsigned char)from;
    from >>= BYTE;
  }
  return bytes;
}

struct bytes_t bytes_from_int(unsigned int from) {
  struct bytes_t bytes;
  bytes.len = 4;
  bytes.head = malloc(sizeof(char) * bytes.len);
  for (size_t i = 1; i <= bytes.len; i++) {
    bytes.head[bytes.len - i] = (unsigned char)from;
    from >>= BYTE;
  }
  return bytes;
}

struct bytes_t bytes_from_long(unsigned long from) {
  struct bytes_t bytes;
  bytes.len = 8;
  bytes.head = malloc(sizeof(char) * bytes.len);
  for (size_t i = 1; i <= bytes.len; i++) {
    bytes.head[bytes.len - i] = (unsigned char)from;
    from >>= BYTE;
  }
  return bytes;
}

struct bytes_t bytes_from_string(const char *str) {
  struct bytes_t bytes;
  size_t slen = strlen(str);
  bytes.len = slen + UTF_HEADER_SIZE;
  bytes.head = malloc(sizeof(char) * bytes.len);
  memcpy(&bytes.head[UTF_HEADER_SIZE], str, slen);
  for (size_t i = 1; i <= UTF_HEADER_SIZE; i++) {
    bytes.head[UTF_HEADER_SIZE - i] = (unsigned char)slen;
    slen >>= BYTE;
  }
  return bytes;
}
