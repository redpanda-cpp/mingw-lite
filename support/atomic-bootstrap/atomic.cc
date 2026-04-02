// use C++ to force prototype check

extern "C" __attribute__((__target__("arch=i486"))) bool
__atomic_compare_exchange_4(volatile void *ptr, void *expected,
                            unsigned int desired, bool /*weak*/,
                            int /*success_memorder*/, int /*failure_memorder*/
) {
  return __sync_bool_compare_and_swap(static_cast<volatile unsigned int *>(ptr),
                                      *static_cast<unsigned int *>(expected),
                                      desired);
}
