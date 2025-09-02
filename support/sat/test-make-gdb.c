#include <conio.h>
#include <stdio.h>
#include <windows.h>

#include "common.h"

int main(int argc, char *argv[]) {
  char mingw_bin_dir[MAX_PATH];
  resolve_mingw_bin_dir(mingw_bin_dir);
  prepend_to_env_path(mingw_bin_dir);

  change_to_self_dir();
  mkdir_p(DEBUG_BUILD_DIR);

  const char *make_argv[] = {
      "mingw32-make",
      "DIR=" DEBUG_BUILD_DIR,
      "SUFFIX=.exe",
      NULL,
  };
  HANDLE make_process = spawn(make_argv);
  if (wait(make_process) != 0)
    error_exit("make failed");

  const char *gdbserver_argv[] = {
      "gdbserver",
      "localhost:1234",
      DEBUG_BUILD_DIR "/breakpoint.exe",
      NULL,
  };
  HANDLE gdbserver_process = spawn(gdbserver_argv);

  const char *gdb_argv[] = {
      "gdb",
      "--batch",
      "--command=gdb-command.txt",
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
