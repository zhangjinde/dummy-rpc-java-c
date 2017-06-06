#pragma once

#include <stdio.h>
#include "descriptor.h"

struct desc_class *parse(const unsigned char *bytes, const size_t len);
