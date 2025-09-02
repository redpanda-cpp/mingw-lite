#include "common.h"

#include <conio.h>
#include <processenv.h>
#include <stdio.h>
#include <windows.h>

const int MAX_CMDLINE = 32767;
const int MAX_ENV = 32767;

static void mkdir_exist_ok(const char *dir);
static bool need_quote(const char *arg);

void change_to_self_dir() {
  char dir[MAX_PATH];
  resolve_self_dir(dir);
  if (!SetCurrentDirectoryA(dir))
    error_exit("Failed to change to self directory\n");
}

void error_exit(const char *msg) {
  fprintf(stderr, "%s\n", msg);
  _getch();
  exit(EXIT_FAILURE);
}

bool is_nt() {
  static OSVERSIONINFOA osvi = {sizeof(osvi)};
  if (!osvi.dwMajorVersion)
    GetVersionExA(&osvi);
  return osvi.dwPlatformId >= VER_PLATFORM_WIN32_NT;
}

void mkdir_exist_ok(const char *dir) {
  if (CreateDirectoryA(dir, NULL))
    return;
  if (GetLastError() != ERROR_ALREADY_EXISTS) {
    fprintf(stderr, "CreateDirectoryA: %ld\n", GetLastError());
    error_exit("Failed to create directory\n");
  }
  DWORD attr = GetFileAttributesA(dir);
  if (attr == INVALID_FILE_ATTRIBUTES) {
    fprintf(stderr, "GetFileAttributesA: %ld\n", GetLastError());
    error_exit("Failed to get directory attributes\n");
  }
  if (!(attr & FILE_ATTRIBUTE_DIRECTORY))
    error_exit("Directory name occupied by a file\n");
}

void mkdir_p(const char *dir) {
  char parent[MAX_PATH];
  int idx = 0;
  while (dir[idx]) {
    char cur = dir[idx];
    if (cur == '/' || cur == '\\') {
      parent[idx] = 0;
      mkdir_exist_ok(parent);
    }
    parent[idx] = cur;
    idx++;
  }
  parent[idx] = 0;
  mkdir_exist_ok(dir);
}

static bool need_quote(const char *arg) {
  if (*arg == 0)
    // empty string
    return true;

  while (*arg) {
    if (*arg == ' ' || *arg == '\t' || *arg == '\n' || *arg == '\v' ||
        *arg == '\"')
      return true;
    arg++;
  }
  return false;
}

void prepend_to_env_path(const char *path) {
  char env_path[MAX_ENV];
  int path_len = strlen(path);
  memcpy(env_path, path, path_len);
  env_path[path_len] = ';';

  int remaining_len = MAX_ENV - path_len - 1;
  int get_len =
      GetEnvironmentVariableA("PATH", env_path + path_len + 1, remaining_len);
  if (get_len >= remaining_len)
    error_exit("PATH is too long\n");
  if (get_len == 0) {
    if (GetLastError() == ERROR_ENVVAR_NOT_FOUND) {
      env_path[path_len] = 0;
    } else {
      error_exit("Failed to get PATH\n");
    }
  }
  SetEnvironmentVariableA("PATH", env_path);
}

void resolve_mingw_bin_dir(char dir[MAX_PATH]) {
  const char *const rel_mingw_bin_dir = MINGW_DIR "/bin";
  const int rel_len = strlen(rel_mingw_bin_dir);

  int dir_len = resolve_self_dir(dir);
  if (dir_len + rel_len + 1 >= MAX_PATH)
    error_exit("Executable path is too long\n");

  memcpy(dir + dir_len, rel_mingw_bin_dir, rel_len);
  dir[dir_len + rel_len] = 0;
}

int resolve_self_dir(char dir[MAX_PATH]) {
  DWORD dir_len = GetModuleFileNameA(NULL, dir, MAX_PATH);
  if (dir_len == 0)
    error_exit("Failed to get executable path\n");
  if (dir_len >= MAX_PATH)
    error_exit("Executable path is too long\n");

  while (dir[dir_len - 1] != '/' && dir[dir_len - 1] != '\\')
    dir_len--;
  dir[dir_len] = 0;
  return dir_len;
}

HANDLE spawn(const char *argv[]) {
  char cmdline[MAX_CMDLINE];
  int argi = 0;
  int idx = 0;
  // Everyone quotes command line arguments the wrong way
  // https://learn.microsoft.com/en-us/archive/blogs/twistylittlepassagesallalike/everyone-quotes-command-line-arguments-the-wrong-way
  for (; argv[argi]; argi++) {
    const char *arg = argv[argi];
    if (need_quote(arg)) {
      cmdline[idx++] = '\"';
      for (const char *p = arg;; p++) {
        int n_backslash = 0;
        while (*p == '\\') {
          p++;
          n_backslash++;
        }
        if (!*p) {
          // Escape all backslashes, but let the terminating double quotation
          // mark we add below be interpreted as a metacharacter.
          for (int i = 0; i < n_backslash * 2; i++)
            cmdline[idx++] = '\\';
        } else if (*p == '\"') {
          // Escape all backslashes and the following double quotation mark.
          for (int i = 0; i < n_backslash * 2 + 1; i++)
            cmdline[idx++] = '\\';
          cmdline[idx++] = '\"';
        } else {
          // Backslashes aren't special here.
          for (int i = 0; i < n_backslash; i++)
            cmdline[idx++] = '\\';
          cmdline[idx++] = *p;
        }
      }
      cmdline[idx++] = '\"';
    } else {
      int arg_len = strlen(arg);
      memcpy(cmdline + idx, arg, arg_len);
      idx += arg_len;
      cmdline[idx++] = ' ';
    }
  }
  cmdline[idx] = 0;

  STARTUPINFOA si = {sizeof(si)};
  PROCESS_INFORMATION pi = {0};
  DWORD exit_code = 0;
  if (!CreateProcessA(NULL, cmdline, NULL, NULL, TRUE, 0, NULL, NULL, &si,
                      &pi)) {
    fprintf(stderr, "CreateProcessA: %ld\n", GetLastError());
    error_exit("Failed to spawn process\n");
  }
  return pi.hProcess;
}

int wait(HANDLE process) {
  DWORD exit_code;
  if (WaitForSingleObject(process, INFINITE) != WAIT_OBJECT_0) {
    fprintf(stderr, "WaitForSingleObject: %ld\n", GetLastError());
    error_exit("Failed to wait for process\n");
  }
  if (!GetExitCodeProcess(process, &exit_code)) {
    fprintf(stderr, "GetExitCodeProcess: %ld\n", GetLastError());
    error_exit("Failed to get process exit code\n");
  }
  CloseHandle(process);
  return exit_code;
}
