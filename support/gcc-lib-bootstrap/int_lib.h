#pragma once

#include <limits.h>
#include <stdint.h>

#include "int_types.h"

extern "C" {
du_int __udivmoddi4(du_int a, du_int b, du_int *rem);
}
