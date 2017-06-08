#ifndef BYTES_H
#define BYTES_H

#include <stdio.h>

struct bytes_t {
  unsigned char *head;
  size_t len;
};

struct bytes_t bytes_from_char(unsigned char from);
struct bytes_t bytes_from_short(unsigned short from);
struct bytes_t bytes_from_int(unsigned int from);
struct bytes_t bytes_from_long(unsigned long from);
struct bytes_t bytes_from_string(const char *str);

#endif /* end of include guard: BYTES_H */
