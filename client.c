#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <math.h>

#define PORT 8100

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

size_t dump_newString(const unsigned char *bytes, const size_t len);
size_t dump_object(const unsigned char *bytes, const size_t len);
size_t dump_contents(const unsigned char* bytes, const size_t len);
size_t dump_newClassDesc(const unsigned char *bytes, const size_t len);
size_t dump_prevObject(const unsigned char *bytes, const size_t len);

struct blist {
  unsigned char *bytes;
  size_t len;
  struct blist *next;
};

struct class_field {
  char *name;
  unsigned char type;
  char *type_name;
  struct class_field *next;
};

struct class_desc {
  char *name;
  long uid;
  unsigned char desc_flag;
  struct class_field *field;
};

char *new_str(const char *str) {
  const size_t len = strlen(str) + 1;
  char *ret = malloc(sizeof(char) * len);
  strcpy(ret, str);
  return ret;
}

size_t hexdump(const char *desc, const unsigned char *pc, const size_t len) {
  int i;
  unsigned char buff[17];
  if (desc != NULL)
    printf ("%s:\n", desc);
  if (len == 0) {
    printf("  ZERO LENGTH\n");
    return 0;
  }
  // if (len < 0) {
  //   printf("  NEGATIVE LENGTH: %i\n",len);
  //   return;
  // }
  for (i = 0; i < len; i++) {
    if ((i % 16) == 0) {
      if (i != 0)
        printf ("  %s\n", buff);
      printf ("  %04x ", i);
    }
    printf (" %02x", pc[i]);
    if ((pc[i] < 0x20) || (pc[i] > 0x7e))
      buff[i % 16] = '.';
    else
      buff[i % 16] = pc[i];
    buff[(i % 16) + 1] = '\0';
  }
  while ((i % 16) != 0) {
    printf ("   ");
    i++;
  }
  printf ("  %s\n", buff);
  return len;
}

struct blist *blist_append(struct blist *tail, const unsigned char *bytes, const size_t len) {
  struct blist *new_one = malloc(sizeof(struct blist));
  new_one->bytes = malloc(sizeof(unsigned char) * len);
  memcpy(new_one->bytes, bytes, len);
  new_one->len = len;
  new_one->next = NULL;
  if (tail != NULL) { // concat list if provided previous one
    tail->next = new_one;
  }
  return new_one;
}

void blist_concat(unsigned char *dest, struct blist *list) {
  struct blist *listp = list;
  size_t lenp = 0;
  while (listp != NULL) {
    memcpy(&dest[lenp], listp->bytes, listp->len);
    lenp += listp->len;
    listp = listp->next;
  }
}

size_t blist_recv(int sd, struct blist **list) {
  *list = NULL;
  struct blist *listp = NULL;
  size_t len_sum = 0;
  int len;
  unsigned char *buf = malloc(sizeof(unsigned char) * 64);

  while ((len = recv(sd, buf, sizeof(buf), 0)) != 0) {
    if (len < 0) {
      free(buf);
      printf("!!! recv error\n");
      return 0;
    }

    listp = blist_append(listp, buf, len);
    if (*list == NULL) {
      *list = listp;
    }

    len_sum += len;
  }
  free(buf);

  return len_sum;
}

size_t dump_utf(const char *desc, const unsigned char *bytes) {
  const unsigned char stringlen = (bytes[0] << 4) + bytes[1];
  return 2 + hexdump(desc, &bytes[2], stringlen);
}

size_t dump_classAnnotation(const unsigned char *bytes, const size_t len) {
  switch (bytes[0]) {
  case TC_ENDBLOCKDATA:
    return hexdump("TC_ENDBLOCKDATA", bytes, 1);
  default:
    printf("not implemented\n");
    return 0;
  }
}

size_t dump_className1(const unsigned char *bytes, const size_t len) {
  size_t read = 0;
  read += dump_object(&bytes[0], len - read);
  return read;
}

size_t dump_fieldName(const unsigned char *bytes, const size_t len) {
  return dump_utf("fieldName", bytes);
}

size_t dump_fieldDesc(const unsigned char *bytes, const size_t len) {
  printf("fieldDesc\n");
  size_t read = 0;
  read += hexdump("typecode", &bytes[read], 1);
  switch (bytes[0]) {
    case 'B':	// byte
    case 'C':	// char
    case 'D':	// double
    case 'F':	// float
    case 'I':	// integer
    case 'J':	// long
    case 'S':	// short
    case 'Z':	// boolean
      read += dump_fieldName(&bytes[read], len - read);
      break;
    case '[':	// array
    case 'L':	// object
      read += dump_fieldName(&bytes[read], len - read);
      read += dump_className1(&bytes[read], len - read);
      break;
  }
  return read;
}

size_t dump_fields(const unsigned char *bytes, const size_t len) {
  size_t read = 0;
  read += hexdump("count", &bytes[0], 2);
  unsigned short count = (bytes[0] << 4) + bytes[1];
  for (size_t i = 0; i < count; i++) {
    read += dump_fieldDesc(&bytes[read], len - read);
  }
  return read;
}

size_t dump_classDescFlags(const unsigned char flag) {
  char *name;
  switch (flag) {
  case SC_SERIALIZABLE: name = "SC_SERIALIZABLE"; break;
  }
  return hexdump(name, &flag, 1);
}

size_t dump_nullReference(const unsigned char *bytes, const size_t len) {
  return hexdump("TC_NULL", bytes, 1);
}

size_t dump_classDesc(const unsigned char *bytes, const size_t len) {
  switch(bytes[0]) {
  case TC_CLASSDESC:
  case TC_PROXYCLASSDESC:
    return dump_newClassDesc(bytes, len);
  case TC_NULL:
    return dump_nullReference(bytes, len);
  case TC_REFERENCE:
    return dump_prevObject(bytes, len);
  default:
    printf("!!! error classDesc\n");
    return 0;
  }
}

size_t dump_superClassDesc(const unsigned char *bytes, const size_t len) {
  return dump_classDesc(bytes, len);
}

size_t dump_classDescInfo(const unsigned char *bytes, const size_t len) {
  size_t read = 0;
  read += dump_classDescFlags(bytes[read]);
  read += dump_fields(&bytes[read], len - read);
  read += dump_classAnnotation(&bytes[read], len - read);
  read += dump_superClassDesc(&bytes[read], len - read);
  return read;
}

size_t dump_newHandle(const unsigned char *bytes, const size_t len) {
  // TODO newHandle
  return hexdump("newHandle", bytes, 0);
}

size_t dump_serialVersionUID(const unsigned char *bytes, const size_t len) {
  return hexdump("serialVersionUID", bytes, len);
}

size_t dump_className(const unsigned char *bytes, const size_t len) {
  return dump_utf("className", bytes);
}

size_t dump_newString(const unsigned char *bytes, const size_t len) {
  size_t read = 0;
  switch (bytes[0]) {
    case TC_STRING:
      // TODO newHandle
      read += hexdump("TC_STRING", &bytes[read], 1);
      read += dump_utf("newString", &bytes[read]);
  }
  return read;
}

size_t dump_newClassDesc(const unsigned char *bytes, const size_t len) {
  // TODO クラスオブジェクトを作る
  size_t read = 0;
  switch (bytes[0]) {
  case TC_CLASSDESC: // className serialVersionUID newHandle classDescInfo
    read += hexdump("TC_CLASSDESC", &bytes[read], 1);
    read += dump_className(&bytes[read], len - read);
    read += dump_serialVersionUID(&bytes[read], 8);
    // newHandle
    read += dump_classDescInfo(&bytes[read], len - read);
    break;
  case TC_PROXYCLASSDESC: // newHandle proxyClassDescInfo
    // hexdump("TC_PROXYCLASSDESC", &bytes[0], 1);
    printf("not implemented\n");
    return 0;
  }
  return read;
}

size_t dump_prevObject(const unsigned char *bytes, const size_t len) {
  size_t read = 0;
  read += hexdump("TC_REFERENCE", &bytes[read], 1);
  read += hexdump("handle", &bytes[read], 4);
  return read;
}

size_t dump_classdata(const unsigned char *bytes, const size_t len) {
  size_t read = 0;
  read += hexdump("classdata", &bytes[read], len);
  return read;
}

size_t dump_newObject(const unsigned char* bytes, const size_t len) {
  size_t read = 0;
  read += hexdump("TC_OBJECT", &bytes[read], 1);
  read += dump_classDesc(&bytes[read], len - read);
  // newHandle
  read += dump_classdata(&bytes[read], len - read);
  return read;
}

size_t dump_object(const unsigned char *bytes, const size_t len) {
  size_t read = 0;
  switch (bytes[0]) {
  case TC_OBJECT: read += dump_newObject(bytes, len); break;
  case TC_REFERENCE: read += dump_prevObject(bytes, len); break;
  case TC_STRING: read += dump_newString(bytes, len); break;
  }
  return read;
}

size_t dump_content(const unsigned char* bytes, const size_t len) {
  size_t read = 0;
  switch (bytes[0]) {
  case TC_OBJECT:
  case TC_REFERENCE:
    read += dump_object(bytes, len);
    break;
  default:
    printf("not implemented\n");
    return 0;
  }
  return read;
}

size_t dump_contents(const unsigned char *bytes, const size_t len) {
  size_t read = 0;
  while (read < len) {
    read += dump_content(&bytes[read], len - read);
  }
  return read;
}

void dump(const unsigned char* bytes, const size_t len) {
  size_t read = 0;
  read += hexdump("magic", &bytes[0], 2);
  read += hexdump("version", &bytes[2], 2);
  read += dump_contents(&bytes[4], len - 4);
  printf("len : %ld\n", len);
  printf("read : %ld\n", read);
}

int main(int argc, char const *argv[]) {
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  const int sd = socket(AF_INET, SOCK_STREAM, 0);
  connect(sd, (struct sockaddr*)&addr, sizeof(addr));

  const char* command = "fetch Task\n";
  send(sd, command, strlen(command), 0);

  struct blist *list;
  size_t byteslen = blist_recv(sd, &list);

  if (byteslen == 0) {
    close(sd);
    return 1;
  }

  unsigned char *bytes = malloc(sizeof(unsigned char) * byteslen);
  blist_concat(bytes, list);
  hexdump("received", bytes, byteslen);

  dump(bytes, byteslen);

  close(sd);

  return 0;
}
