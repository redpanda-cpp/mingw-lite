#include <thunk/_common.h>

#include <corecrt_startup.h>
#include <stdlib.h>

#include <windows.h>

#include "__p___argv.h"

namespace mingw_thunk
{
  __DEFINE_THUNK(api_ms_win_crt_runtime_l1_1_0,
                 0,
                 int,
                 __cdecl,
                 _configure_narrow_argv,
                 _crt_argv_mode mode)
  {
    // BEWARE: runtime is not fully initialized yet!
    _configure_wide_argv(mode);

    int argc = *__p___argc();
    wchar_t **wargv = *__p___wargv();

    char **argv = (char **)malloc(sizeof(char *) * (argc + 1));
    for (int i = 0; i < argc; i++) {
      int len = WideCharToMultiByte(
          CP_UTF8, 0, wargv[i], -1, nullptr, 0, nullptr, nullptr);
      argv[i] = (char *)malloc(len);
      WideCharToMultiByte(
          CP_UTF8, 0, wargv[i], -1, argv[i], len, nullptr, nullptr);
    }
    argv[argc] = nullptr;

    internal::u8_argv = argv;

    return 0;
  }
} // namespace mingw_thunk
