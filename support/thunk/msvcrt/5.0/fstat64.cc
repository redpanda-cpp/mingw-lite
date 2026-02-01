#include <thunk/_common.h>

#include <sys/stat.h>

namespace mingw_thunk
{
  __DECLARE_CRT_IMP_ALIAS(fstat64, _fstat64)
}
