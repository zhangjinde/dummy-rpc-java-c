#ifndef NET_H
#define NET_H

#include "descriptor.h"
#include "bytes.h"

#define PORT 8100

int net_connect();
struct inst net_fetch(const char *service_name);
void net_call(const char *service_name, const char *method_name, struct bytes_t arg);

#endif /* end of include guard: NET_H */
