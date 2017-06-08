#include "strutil.h"
#include <stdlib.h>
#include <string.h>

char *newnstr(const unsigned char *bytes, const size_t len) {
  char *ret = malloc(sizeof(unsigned char) * (len + 1));
  memcpy(ret, bytes, len);
  memset(&ret[len], '\0', 1);
  return ret;
}

char *newstr(const char *str) {
  size_t len = strlen(str) + 1;
  char *ret = malloc(sizeof(char) * len);
  strcpy(ret, str);
  return ret;
}
