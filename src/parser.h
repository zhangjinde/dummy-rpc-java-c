#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include "descriptor.h"

struct inst parse(const unsigned char *bytes, const size_t len);
size_t parse_object(struct inst **instance, const unsigned char *bytes, const size_t len);
size_t parse_classDesc(struct class_t *clazz, const unsigned char *bytes, const size_t len);

#endif /* end of include guard: PARSER_H */
