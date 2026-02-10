#include <conio.h>
#include <stdio.h>
#include <windows.h>

#include "common.h"

int wmain(int argc, wchar_t *argv[]) {
  wchar_t mingw_bin_dir[MAX_PATH];
  resolve_mingw_bin_dir(mingw_bin_dir);
  prepend_to_env_path(mingw_bin_dir);

  install_shared_libs();

  change_to_self_dir();

  const wchar_t *xmake_config_argv[] = {
      L"xmake",
      L"config",
      L"--verbose",
      L"--plat=mingw",
      (L"--arch=" XMAKE_ARCH),
      L"--builddir=build-shared",
#ifdef ENABLE_UTF8
      is_nt() ? L"--utf8=y" : L"--utf8=n",
#else
      L"--utf8=n",
#endif
      lt_win98() ? L"--ldflags=-fno-lto" : NULL,
      NULL,
  };
  HANDLE xmake_config_process = spawn(xmake_config_argv);
  if (wait(xmake_config_process) != 0)
    error_exit("xmake config failed");

  const wchar_t *xmake_build_argv[] = {
      L"xmake", L"build", L"--verbose", is_nt() ? NULL : L"--jobs=1", NULL,
  };
  HANDLE xmake_build_process = spawn(xmake_build_argv);
  if (wait(xmake_build_process) != 0)
    error_exit("xmake build failed");

  const wchar_t *xmake_test_argv[] = {
      L"xmake",
      L"test",
      L"--verbose",
      NULL,
  };
  HANDLE xmake_test_process = spawn(xmake_test_argv);
  if (wait(xmake_test_process) != 0)
    error_exit("xmake test failed");

  printf("[test-shared] done\n");

  _getch();

  return 0;
}
