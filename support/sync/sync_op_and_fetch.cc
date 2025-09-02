// use C++ to force prototype check

#define DEFINE_SYNC_OP_AND_FETCH(op, size, type)                               \
  extern "C" type __sync_##op##_and_fetch_##size(volatile void *ptr,           \
                                                 type value) {                 \
    return __atomic_##op##_fetch(static_cast<volatile type *>(ptr), value,     \
                                 __ATOMIC_SEQ_CST);                            \
  }

DEFINE_SYNC_OP_AND_FETCH(add, 1, unsigned char)
DEFINE_SYNC_OP_AND_FETCH(add, 2, unsigned short)
DEFINE_SYNC_OP_AND_FETCH(add, 4, unsigned int)

DEFINE_SYNC_OP_AND_FETCH(sub, 1, unsigned char)
DEFINE_SYNC_OP_AND_FETCH(sub, 2, unsigned short)
DEFINE_SYNC_OP_AND_FETCH(sub, 4, unsigned int)

DEFINE_SYNC_OP_AND_FETCH(or, 1, unsigned char)
DEFINE_SYNC_OP_AND_FETCH(or, 2, unsigned short)
DEFINE_SYNC_OP_AND_FETCH(or, 4, unsigned int)

DEFINE_SYNC_OP_AND_FETCH(and, 1, unsigned char)
DEFINE_SYNC_OP_AND_FETCH(and, 2, unsigned short)
DEFINE_SYNC_OP_AND_FETCH(and, 4, unsigned int)

DEFINE_SYNC_OP_AND_FETCH(xor, 1, unsigned char)
DEFINE_SYNC_OP_AND_FETCH(xor, 2, unsigned short)
DEFINE_SYNC_OP_AND_FETCH(xor, 4, unsigned int)

DEFINE_SYNC_OP_AND_FETCH(nand, 1, unsigned char)
DEFINE_SYNC_OP_AND_FETCH(nand, 2, unsigned short)
DEFINE_SYNC_OP_AND_FETCH(nand, 4, unsigned int)
