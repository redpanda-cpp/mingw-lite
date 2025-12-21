#include "__getmainargs.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>

#include <stdlib.h>

#include <windows.h>

#include "../environment/__p__environ.h"
#include "__p___argv.h"

extern "C"
{
  struct _startupinfo;
  _CRTIMP int __cdecl __getmainargs(int *_Argc,
                                    char ***_Argv,
                                    char ***_Env,
                                    int _DoWildCard,
                                    _startupinfo *_StartInfo);
  _CRTIMP int __cdecl __wgetmainargs(int *_Argc,
                                     wchar_t ***_Argv,
                                     wchar_t ***_Env,
                                     int _DoWildCard,
                                     _startupinfo *_StartInfo);
}

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt,
                 0,
                 int,
                 __cdecl,
                 __getmainargs,
                 int *pargc,
                 char ***pargv,
                 char ***penvp,
                 int do_wild_card,
                 _startupinfo *start_info)
  {
    // BEWARE: runtime is not fully initialized yet!
    int argc;
    wchar_t **wargv;
    wchar_t **wenvp;

    __wgetmainargs(&argc, &wargv, &wenvp, do_wild_card, start_info);

    char **argv = (char **)malloc(sizeof(char *) * (argc + 1));
    for (int i = 0; i < argc; i++) {
      int len = WideCharToMultiByte(
          CP_UTF8, 0, wargv[i], -1, nullptr, 0, nullptr, nullptr);
      argv[i] = (char *)malloc(len);
      WideCharToMultiByte(
          CP_UTF8, 0, wargv[i], -1, argv[i], len, nullptr, nullptr);
    }
    argv[argc] = nullptr;

    int envc;
    char **envp;

#if THUNK_LEVEL >= NTDDI_VISTA

    {
      auto ret = internal::u8_envp_from_wenvp(wenvp);
      envc = ret.envc;
      envp = ret.envp;
    }

#elif THUNK_LEVEL >= NTDDI_WIN4

    if (internal::os_geq(6, 0)) {
      auto ret = internal::u8_envp_from_wenvp(wenvp);
      envc = ret.envc;
      envp = ret.envp;
    } else {
      // Fake wenvp (converted from envp)
      auto ret = internal::u8_envp_from_win32_env_strings();
      envc = ret.envc;
      envp = ret.envp;
    }

#else

    if (internal::os_geq(6, 0)) {
      auto ret = internal::u8_envp_from_wenvp(wenvp);
      envc = ret.envc;
      envp = ret.envp;
    } else if (internal::is_nt()) {
      // Fake wenvp (converted from envp)
      auto ret = internal::u8_envp_from_win32_env_strings();
      envc = ret.envc;
      envp = ret.envp;
    } else {
      // Fake wenvp is what we need:
      // 1. `GetEnvironmentStringsW` is a stub;
      // 2. environment variables are limited to the code page.
      auto ret = internal::u8_envp_from_wenvp(wenvp);
      envc = ret.envc;
      envp = ret.envp;
    }

#endif

    internal::u8_argv = argv;
    internal::u8_envp = envp;

    __atomic_test_and_set(&internal::u8_environ_lock, __ATOMIC_ACQUIRE);
    internal::u8_environ = envp;
    internal::u8_environ_size = envc;
    __atomic_clear(&internal::u8_environ_lock, __ATOMIC_RELEASE);

    *pargc = argc;
    *pargv = argv;
    *penvp = envp;

    return 0;
  }

  // upstream added this to fix msvcrt32 ABI
  __DECLARE_FORCE_OVERRIDE_MINGW_EMU(__getmainargs)

  namespace internal
  {
    u8_envp_result u8_envp_from_wenvp(wchar_t **wenvp)
    {
      int envc = 0;
      while (wenvp[envc])
        envc++;

      char **envp =
          (char **)malloc(sizeof(char *) * (envc + 1));
      for (int i = 0; i < envc; i++) {
        int len = WideCharToMultiByte(
            CP_UTF8, 0, wenvp[i], -1, nullptr, 0, nullptr, nullptr);
        envp[i] = (char *)malloc(len);
        WideCharToMultiByte(
            CP_UTF8, 0, wenvp[i], -1, envp[i], len, nullptr, nullptr);
      }
      envp[envc] = nullptr;

      return {envc, envp};
    }

    u8_envp_result u8_envp_from_win32_env_strings()
    {
      wchar_t *env_strings = GetEnvironmentStringsW();
      if (!env_strings)
        return {0, nullptr};

      int envc = 0;
      wchar_t *p = env_strings;
      while (*p) {
        envc++;
        p += wcslen(p) + 1;
      }

      char **envp =
          (char **)malloc(sizeof(char *) * (envc + 1));
      p = env_strings;
      for (int i = 0; i < envc; i++) {
        size_t w_len = wcslen(p);
        int u_len = WideCharToMultiByte(
            CP_UTF8, 0, p, w_len, nullptr, 0, nullptr, nullptr);
        envp[i] = (char *)malloc(u_len + 1);
        WideCharToMultiByte(
            CP_UTF8, 0, p, w_len, envp[i], u_len, nullptr, nullptr);
        envp[i][u_len] = 0;
        p += w_len + 1;
      }
      envp[envc] = nullptr;

      FreeEnvironmentStringsW(env_strings);

      return {envc, envp};
    }
  } // namespace internal
} // namespace mingw_thunk
