#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "descriptor.h"
#include "bytes.h"
#include "blist.h"

struct bytes_t serialize(struct inst);
void serialize_object(struct blist *list, const struct inst *instance);
void serialize_classDesc(struct blist *list, const struct class_t *clazz);

#endif /* end of include guard: SERIALIZER_H */
