#include <conio.h>
#include <stdio.h>
#include <windows.h>

#include "common.h"

int wmain(int argc, wchar_t *argv[]) {
  wchar_t mingw_bin_dir[MAX_PATH];
  resolve_mingw_bin_dir(mingw_bin_dir);
  prepend_to_env_path(mingw_bin_dir);

  clean_shared_libs();
  change_to_self_dir();
  mkdir_p(L"" RELEASE_BUILD_DIR);

  const wchar_t *make_argv[] = {
      L"mingw32-make", L"-f", L"Makefile.console", (L"DIR=" RELEASE_BUILD_DIR),
      L"-j",           NULL,
  };
  HANDLE make_process = spawn(make_argv);
  if (wait(make_process) != 0)
    error_exit("make failed");

  const wchar_t *make_test_argv[] = {
      L"mingw32-make",
      L"-f",
      L"Makefile.console",
      (L"DIR=" RELEASE_BUILD_DIR),
      L"-j1",
      L"test",
      NULL,
  };
  HANDLE make_test_process = spawn(make_test_argv);
  if (wait(make_test_process) != 0)
    error_exit("console test failed");

  printf("[test-console] done\n");

  flush_console_and_get_char();

  return 0;
}
