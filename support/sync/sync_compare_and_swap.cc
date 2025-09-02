// use C++ to force prototype check

extern "C" bool __sync_bool_compare_and_swap_1(volatile void *ptr,
                                               unsigned char expected,
                                               unsigned char desired) {
  return __atomic_compare_exchange_n(static_cast<volatile unsigned char *>(ptr),
                                     &expected, desired, false,
                                     __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}

extern "C" bool __sync_bool_compare_and_swap_2(volatile void *ptr,
                                               unsigned short expected,
                                               unsigned short desired) {
  return __atomic_compare_exchange_n(
      static_cast<volatile unsigned short *>(ptr), &expected, desired, false,
      __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}

extern "C" bool __sync_bool_compare_and_swap_4(volatile void *ptr,
                                               unsigned int expected,
                                               unsigned int desired) {
  return __atomic_compare_exchange_n(static_cast<volatile unsigned int *>(ptr),
                                     &expected, desired, false,
                                     __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}

extern "C" unsigned char __sync_val_compare_and_swap_1(volatile void *ptr,
                                                       unsigned char oldval,
                                                       unsigned char newval) {
  unsigned char expected = oldval;
  __atomic_compare_exchange_n(static_cast<volatile unsigned char *>(ptr),
                              &expected, newval, false, __ATOMIC_SEQ_CST,
                              __ATOMIC_SEQ_CST);
  return expected;
}

extern "C" unsigned short __sync_val_compare_and_swap_2(volatile void *ptr,
                                                        unsigned short oldval,
                                                        unsigned short newval) {
  unsigned short expected = oldval;
  __atomic_compare_exchange_n(static_cast<volatile unsigned short *>(ptr),
                              &expected, newval, false, __ATOMIC_SEQ_CST,
                              __ATOMIC_SEQ_CST);
  return expected;
}

extern "C" unsigned int __sync_val_compare_and_swap_4(volatile void *ptr,
                                                      unsigned int oldval,
                                                      unsigned int newval) {
  unsigned int expected = oldval;
  __atomic_compare_exchange_n(static_cast<volatile unsigned int *>(ptr),
                              &expected, newval, false, __ATOMIC_SEQ_CST,
                              __ATOMIC_SEQ_CST);
  return expected;
}
