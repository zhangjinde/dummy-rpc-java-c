#include <string.h>
#include "serializer.h"
#include "blist.h"
#include "descriptor.h"
#include "hexdump.h"

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

void serialize_utf(struct blist *list, const char *str) {
  blist_push(list, bytes_from_string(str));
}

void serialize_className(struct blist *list, const char* name) {
  serialize_utf(list, name);
}

void serialize_serialVersionUID(struct blist *list, unsigned long uid) {
  blist_push(list, bytes_from_long(uid));
}

void serialize_classDescFlags(struct blist *list, unsigned char flag) {
  blist_push(list, bytes_from_char(flag));
}

void serialize_classDescInfo(struct blist *list, const struct class_t *clazz) {
  serialize_classDescFlags(list, clazz->flag);
  // TODO fields
  // TODO classAnnotation
  // TODO superClassDesc
}

void serialize_newClassDesc(struct blist *list, const struct class_t *clazz) {
  // TODO TC_PROXYCLASSDESCの場合をスキップ
  blist_push(list, bytes_from_char(TC_CLASSDESC));
  serialize_className(list, clazz->name);
  serialize_serialVersionUID(list, clazz->uid);
  // TODO newHandle
  serialize_classDescInfo(list, clazz);
}

void serialize_nullReference(struct blist *list) {
  blist_push(list, bytes_from_char(TC_NULL));
}

void serialize_classDesc(struct blist *list, const struct class_t *clazz) {
  if (clazz == NULL) {
    serialize_nullReference(list);
  } else {
    serialize_newClassDesc(list, clazz);
  }
  // TODO ほんとうはprevObjectを考慮する必要あり
}

void serialize_newObject(struct blist *list, const struct inst *instance) {
  blist_push(list, bytes_from_char(TC_OBJECT));
  serialize_classDesc(list, &instance->u.object.clazz);
  // TODO classdata
}

void serialize_object(struct blist *list, const struct inst *instance) {
  switch (instance->type) {
  case TC_OBJECT:
    serialize_newObject(list, instance);
    break;
  // case TC_REFERENCE:
  //   break;
    // TODO ほんとうはhandleを考慮する必要があるが・・・多分なくても動くっちゃ動く．
  case TC_STRING:
    // TODO newString
    break;
  default:
    printf("not implemented (object: 0x%02x)\n", instance->type);
  }
}

struct bytes_t serialize(struct inst instance) {
  struct blist list = new_blist();

  blist_push(&list, bytes_from_short(STREAM_MAGIC));
  blist_push(&list, bytes_from_short(STREAM_VERSION));

  // ほんとうなら contents だが今回はスキップ
  serialize_object(&list, &instance);

  struct bytes_t ret = blist_concat(list);
  blist_free(list);

  hexdump("serialize_result", ret.head, ret.len);
  return ret;
}
