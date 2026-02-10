#include <conio.h>
#include <stdio.h>
#include <windows.h>

#include "common.h"

int wmain(int argc, wchar_t *argv[]) {
  wchar_t mingw_bin_dir[MAX_PATH];
  resolve_mingw_bin_dir(mingw_bin_dir);
  prepend_to_env_path(mingw_bin_dir);

#ifdef ENABLE_SHARED
  clean_shared_libs();
#endif

  change_to_self_dir();
  mkdir_p(L"" DEBUG_BUILD_DIR);

  const wchar_t *make_argv[] = {
      L"mingw32-make",
      (L"DIR=" DEBUG_BUILD_DIR),
      L"SUFFIX=.exe",
      lt_win98() ? L"LDFLAGS=-fno-lto" : NULL,
      NULL,
  };
  HANDLE make_process = spawn(make_argv);
  if (wait(make_process) != 0)
    error_exit("make failed");

  const wchar_t *gdbserver_argv[] = {
      L"gdbserver",
      L"localhost:1234",
      L"" DEBUG_BUILD_DIR "/breakpoint.exe",
      NULL,
  };
  HANDLE gdbserver_process = spawn(gdbserver_argv);

  const wchar_t *gdb_argv[] = {
      L"gdb",
      L"--batch",
      L"--command=gdb-command.txt",
      NULL,
  };
  HANDLE gdb_process = spawn(gdb_argv);

  if (wait(gdbserver_process) != 0)
    error_exit("gdbserver failed");

  if (wait(gdb_process) != 0)
    error_exit("gdb failed");

  printf("[test-make-gdb] done\n");

  _getch();

  return 0;
}
