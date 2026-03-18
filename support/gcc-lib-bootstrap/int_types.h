#pragma once

typedef int32_t si_int;
typedef uint32_t su_int;

#define clzsi __builtin_clz
#define ctzsi __builtin_ctz

typedef int64_t di_int;
typedef uint64_t du_int;

typedef union {
  di_int all;
  struct {
    su_int low;
    si_int high;
  } s;
} dwords;

typedef union {
  du_int all;
  struct {
    su_int low;
    su_int high;
  } s;
} udwords;
