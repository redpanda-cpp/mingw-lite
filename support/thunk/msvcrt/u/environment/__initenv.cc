#include "__initenv.h"

#include "thunk/_common.h"

namespace mingw_thunk
{
  char **__initenv = nullptr;

  char ***dllimport___initenv __asm__(
      __DLLIMPORT_SYMBOL_NAME___cdecl(__initenv, 0)) = &__initenv;

  // upstream added this for msvcrtarm64
  __DECLARE_FORCE_OVERRIDE_MINGW_EMU(__initenv)
} // namespace mingw_thunk
