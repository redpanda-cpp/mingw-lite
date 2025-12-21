#include "__p___argv.h"

#include <thunk/_common.h>
#include <thunk/string.h>

#include <stdlib.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(
      api_ms_win_crt_environment_l1_1_0, 0, char ***, __cdecl, __p___argv)
  {
    return &internal::u8_argv;
  }

  namespace internal
  {
    char **u8_argv = nullptr;
  } // namespace internal

  __DECLARE_FORCE_OVERRIDE_MINGW_EMU(__p___argv)
} // namespace mingw_thunk
