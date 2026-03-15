#include "common.h"

#include <windows.h>

static HANDLE g_child_process = NULL;

BOOL WINAPI ignore_ctrl_c_if_child_is_running(DWORD event) {
  if (event == CTRL_C_EVENT && g_child_process != NULL) {
    return TRUE;
  }
  return FALSE;
}

int wmain(int argc, wchar_t *argv[]) {
  if (!SetConsoleCtrlHandler(ignore_ctrl_c_if_child_is_running, TRUE)) {
    error_exit("Failed to set console ctrl handler\n");
  }

  wchar_t mingw_bin_dir[MAX_PATH];
  resolve_mingw_bin_dir(mingw_bin_dir);
  prepend_to_env_path(mingw_bin_dir);

  const wchar_t **payload_argv = (const wchar_t **)argv + 1;
  g_child_process = spawn(payload_argv);
  int exit_code = wait(g_child_process);
  g_child_process = NULL;

  return exit_code;
}
