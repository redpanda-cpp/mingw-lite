#include "common.h"

#include <conio.h>
#include <processenv.h>
#include <stdio.h>
#include <windows.h>

const int MAX_CMDLINE = 32767;
const int MAX_ENV = 32767;

static void clean_shared_libs_recursive(const char *tar_dir,
                                        const char *ref_dir);
static void install_shared_libs_recursive(const char *tar_dir,
                                          const char *ref_dir);
static void mkdir_exist_ok(const char *dir);
static bool need_quote(const char *arg);

void change_to_self_dir() {
  char dir[MAX_PATH];
  resolve_self_dir(dir);
  if (!SetCurrentDirectoryA(dir))
    error_exit("Failed to change to self directory\n");
}

void clean_shared_libs() {
  char mingw_root_dir[MAX_PATH];
  resolve_mingw_root_dir(mingw_root_dir);

  char shared_dir[MAX_PATH];
  snprintf(shared_dir, sizeof(shared_dir), "%s/lib/shared", mingw_root_dir);

  clean_shared_libs_recursive(mingw_root_dir, shared_dir);
}

static void clean_shared_libs_recursive(const char *tar_dir,
                                        const char *ref_dir) {
  WIN32_FIND_DATAA find_data;
  char search_path[MAX_PATH];
  snprintf(search_path, sizeof(search_path), "%s/*", ref_dir);

  HANDLE hFind = FindFirstFileA(search_path, &find_data);
  if (hFind == INVALID_HANDLE_VALUE)
    error_exit("Failed to walk directory\n");
  do {
    if (strcmp(find_data.cFileName, ".") != 0 &&
        strcmp(find_data.cFileName, "..") != 0) {

      char ref_path[MAX_PATH];
      snprintf(ref_path, sizeof(ref_path), "%s/%s", ref_dir,
               find_data.cFileName);

      char target_path[MAX_PATH];
      snprintf(target_path, sizeof(target_path), "%s/%s", tar_dir,
               find_data.cFileName);

      if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        clean_shared_libs_recursive(target_path, ref_path);
      } else {
        if (GetFileAttributesA(target_path) == INVALID_FILE_ATTRIBUTES)
          continue;
        if (!DeleteFileA(target_path))
          error_exit("Failed to clean shared library\n");
      }
    }
  } while (FindNextFileA(hFind, &find_data));

  if (GetLastError() != ERROR_NO_MORE_FILES)
    error_exit("Failed to walk directory\n");
  FindClose(hFind);
}

void error_exit(const char *msg) {
  fprintf(stderr, "%s\n", msg);
  _getch();
  exit(EXIT_FAILURE);
}

void install_shared_libs() {
  char mingw_root_dir[MAX_PATH];
  resolve_mingw_root_dir(mingw_root_dir);

  char shared_dir[MAX_PATH];
  snprintf(shared_dir, sizeof(shared_dir), "%s/lib/shared", mingw_root_dir);

  install_shared_libs_recursive(mingw_root_dir, shared_dir);
}

static void install_shared_libs_recursive(const char *tar_dir,
                                          const char *ref_dir) {
  WIN32_FIND_DATAA find_data;
  char search_path[MAX_PATH];
  snprintf(search_path, sizeof(search_path), "%s/*", ref_dir);

  HANDLE hFind = FindFirstFileA(search_path, &find_data);
  if (hFind == INVALID_HANDLE_VALUE)
    error_exit("Failed to walk directory\n");
  do {
    if (strcmp(find_data.cFileName, ".") != 0 &&
        strcmp(find_data.cFileName, "..") != 0) {

      char ref_path[MAX_PATH];
      snprintf(ref_path, sizeof(ref_path), "%s/%s", ref_dir,
               find_data.cFileName);

      char tar_path[MAX_PATH];
      snprintf(tar_path, sizeof(tar_path), "%s/%s", tar_dir,
               find_data.cFileName);

      if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        mkdir_exist_ok(tar_path);
        install_shared_libs_recursive(tar_path, ref_path);
      } else {
        if (!CopyFileA(ref_path, tar_path, FALSE))
          error_exit("Failed to copy shared library\n");
      }
    }
  } while (FindNextFileA(hFind, &find_data));

  if (GetLastError() != ERROR_NO_MORE_FILES)
    error_exit("Failed to walk directory\n");
  FindClose(hFind);
}

bool is_nt() {
  static OSVERSIONINFOA osvi = {sizeof(osvi)};
  if (!osvi.dwMajorVersion)
    GetVersionExA(&osvi);
  return osvi.dwPlatformId >= VER_PLATFORM_WIN32_NT;
}

bool lt_win98() {
  static OSVERSIONINFOA osvi = {sizeof(osvi)};
  if (!osvi.dwMajorVersion)
    GetVersionExA(&osvi);
  if (osvi.dwPlatformId < VER_PLATFORM_WIN32_WINDOWS)
    return true;
  if (osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
    if (osvi.dwMajorVersion < 4)
      return true;
    if (osvi.dwMajorVersion == 4)
      return osvi.dwMinorVersion < 10;
  }
  return false;
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

int resolve_mingw_bin_dir(char dir[MAX_PATH]) {
  const char *const rel_bin_dir = "/bin";
  const int rel_len = strlen(rel_bin_dir);

  int dir_len = resolve_mingw_root_dir(dir);
  if (dir_len + rel_len + 1 >= MAX_PATH)
    error_exit("Executable path is too long\n");

  memcpy(dir + dir_len, rel_bin_dir, rel_len);
  dir[dir_len + rel_len] = 0;
  return dir_len + rel_len;
}

int resolve_mingw_lib_shared_dir(char dir[MAX_PATH]) {
  const char *const rel_lib_shared_dir = "/lib/shared";
  const int rel_len = strlen(rel_lib_shared_dir);

  int dir_len = resolve_mingw_root_dir(dir);
  if (dir_len + rel_len + 1 >= MAX_PATH)
    error_exit("Executable path is too long\n");

  memcpy(dir + dir_len, rel_lib_shared_dir, rel_len);
  dir[dir_len + rel_len] = 0;
  return dir_len + rel_len;
}

int resolve_mingw_root_dir(char dir[MAX_PATH]) {
  const char *const rel_mingw_root_dir = "/" MINGW_DIR;
  const int rel_len = strlen(rel_mingw_root_dir);

  int dir_len = resolve_self_dir(dir);
  if (dir_len + rel_len + 1 >= MAX_PATH)
    error_exit("Executable path is too long\n");

  memcpy(dir + dir_len, rel_mingw_root_dir, rel_len);
  dir[dir_len + rel_len] = 0;
  return dir_len + rel_len;
}

int resolve_self_dir(char dir[MAX_PATH]) {
  DWORD dir_len = GetModuleFileNameA(NULL, dir, MAX_PATH);
  if (dir_len == 0)
    error_exit("Failed to get executable path\n");
  if (dir_len >= MAX_PATH)
    error_exit("Executable path is too long\n");

  while (dir[dir_len] != '/' && dir[dir_len] != '\\')
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
