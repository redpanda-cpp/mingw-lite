#include <stdio.h>

thread_local int value = 42;
thread_local int values[1024];

extern "C" void check_tls(void) {
  printf("read tls] %d %d\n", value, values[1023]);
}
