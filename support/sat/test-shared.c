#include <conio.h>
#include <stdio.h>
#include <windows.h>

#include "common.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
  char mingw_bin_dir[MAX_PATH];
  resolve_mingw_bin_dir(mingw_bin_dir);
  prepend_to_env_path(mingw_bin_dir);

  install_shared_libs();

  change_to_self_dir();

  const char *xmake_config_argv[] = {
      "xmake",
      "config",
      "--verbose",
      "--plat=mingw",
      ("--arch=" XMAKE_ARCH),
      "--builddir=build-shared",
      lt_win98() ? "--ldflags=-fno-lto" : NULL,
      NULL,
  };
  HANDLE xmake_config_process = spawn(xmake_config_argv);
  if (wait(xmake_config_process) != 0)
    error_exit("xmake config failed");

  const char *xmake_build_argv[] = {
      "xmake", "build", "--verbose", is_nt() ? NULL : "--jobs=1", NULL,
  };
  HANDLE xmake_build_process = spawn(xmake_build_argv);
  if (wait(xmake_build_process) != 0)
    error_exit("xmake build failed");

  const char *xmake_test_argv[] = {
      "xmake",
      "test",
      "--verbose",
      NULL,
  };
  HANDLE xmake_test_process = spawn(xmake_test_argv);
  if (wait(xmake_test_process) != 0)
    error_exit("xmake test failed");

  printf("[test-shared] done\n");

  _getch();

  return 0;
}
