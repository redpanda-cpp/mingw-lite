// use C++ to force prototype check

#define DEFINE_SYNC_FETCH_AND_OP(op, size, type)                               \
  extern "C" type __sync_fetch_and_##op##_##size(volatile void *ptr,           \
                                                 type value) {                 \
    return __atomic_fetch_##op(static_cast<volatile type *>(ptr), value,       \
                               __ATOMIC_SEQ_CST);                              \
  }

DEFINE_SYNC_FETCH_AND_OP(add, 1, unsigned char)
DEFINE_SYNC_FETCH_AND_OP(add, 2, unsigned short)
DEFINE_SYNC_FETCH_AND_OP(add, 4, unsigned int)

DEFINE_SYNC_FETCH_AND_OP(sub, 1, unsigned char)
DEFINE_SYNC_FETCH_AND_OP(sub, 2, unsigned short)
DEFINE_SYNC_FETCH_AND_OP(sub, 4, unsigned int)

DEFINE_SYNC_FETCH_AND_OP(or, 1, unsigned char)
DEFINE_SYNC_FETCH_AND_OP(or, 2, unsigned short)
DEFINE_SYNC_FETCH_AND_OP(or, 4, unsigned int)

DEFINE_SYNC_FETCH_AND_OP(and, 1, unsigned char)
DEFINE_SYNC_FETCH_AND_OP(and, 2, unsigned short)
DEFINE_SYNC_FETCH_AND_OP(and, 4, unsigned int)

DEFINE_SYNC_FETCH_AND_OP(xor, 1, unsigned char)
DEFINE_SYNC_FETCH_AND_OP(xor, 2, unsigned short)
DEFINE_SYNC_FETCH_AND_OP(xor, 4, unsigned int)

DEFINE_SYNC_FETCH_AND_OP(nand, 1, unsigned char)
DEFINE_SYNC_FETCH_AND_OP(nand, 2, unsigned short)
DEFINE_SYNC_FETCH_AND_OP(nand, 4, unsigned int)
