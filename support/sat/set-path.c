#include "common.h"

int wmain(int argc, wchar_t *argv[]) {
  wchar_t mingw_bin_dir[MAX_PATH];
  resolve_mingw_bin_dir(mingw_bin_dir);
  prepend_to_env_path(mingw_bin_dir);

  const wchar_t **payload_argv = (const wchar_t **)argv + 1;
  HANDLE payload_process = spawn(payload_argv);
  return wait(payload_process);
}
