#include "common.h"

#include <conio.h>
#include <fileapi.h>
#include <processenv.h>
#include <stdio.h>
#include <wchar.h>
#include <windows.h>

const int MAX_CMDLINE = 32767;
const int MAX_ENV = 32767;

static void clean_shared_libs_recursive(const wchar_t *tar_dir,
                                        const wchar_t *ref_dir);
static void install_shared_libs_recursive(const wchar_t *tar_dir,
                                          const wchar_t *ref_dir);
static void mkdir_exist_ok(const wchar_t *dir);
static bool need_quote(const wchar_t *arg);

void change_to_self_dir() {
  wchar_t dir[MAX_PATH];
  resolve_self_dir(dir);
  if (!SetCurrentDirectoryW(dir))
    error_exit("Failed to change to self directory\n");
}

void clean_shared_libs() {
  wchar_t mingw_root_dir[MAX_PATH];
  resolve_mingw_root_dir(mingw_root_dir);

  wchar_t shared_dir[MAX_PATH];
  _snwprintf(shared_dir, MAX_PATH, L"%ls/lib/shared", mingw_root_dir);

  clean_shared_libs_recursive(mingw_root_dir, shared_dir);
}

static void clean_shared_libs_recursive(const wchar_t *tar_dir,
                                        const wchar_t *ref_dir) {
  WIN32_FIND_DATAW find_data;
  wchar_t search_path[MAX_PATH];
  _snwprintf(search_path, MAX_PATH, L"%ls/*", ref_dir);

  HANDLE hFind = FindFirstFileW(search_path, &find_data);
  if (hFind == INVALID_HANDLE_VALUE)
    error_exit("Failed to walk directory\n");
  do {
    if (wcscmp(find_data.cFileName, L".") != 0 &&
        wcscmp(find_data.cFileName, L"..") != 0) {

      wchar_t ref_path[MAX_PATH];
      _snwprintf(ref_path, MAX_PATH, L"%ls/%ls", ref_dir, find_data.cFileName);

      wchar_t target_path[MAX_PATH];
      _snwprintf(target_path, MAX_PATH, L"%ls/%ls", tar_dir,
                 find_data.cFileName);

      if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        clean_shared_libs_recursive(target_path, ref_path);
      } else {
        if (GetFileAttributesW(target_path) == INVALID_FILE_ATTRIBUTES)
          continue;
        if (!DeleteFileW(target_path))
          error_exit("Failed to clean shared library\n");
      }
    }
  } while (FindNextFileW(hFind, &find_data));

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
  wchar_t mingw_root_dir[MAX_PATH];
  resolve_mingw_root_dir(mingw_root_dir);

  wchar_t shared_dir[MAX_PATH];
  _snwprintf(shared_dir, MAX_PATH, L"%ls/lib/shared", mingw_root_dir);

  install_shared_libs_recursive(mingw_root_dir, shared_dir);
}

static void install_shared_libs_recursive(const wchar_t *tar_dir,
                                          const wchar_t *ref_dir) {
  WIN32_FIND_DATAW find_data;
  wchar_t search_path[MAX_PATH];
  _snwprintf(search_path, MAX_PATH, L"%ls/*", ref_dir);

  HANDLE hFind = FindFirstFileW(search_path, &find_data);
  if (hFind == INVALID_HANDLE_VALUE)
    error_exit("Failed to walk directory\n");
  do {
    if (wcscmp(find_data.cFileName, L".") != 0 &&
        wcscmp(find_data.cFileName, L"..") != 0) {

      wchar_t ref_path[MAX_PATH];
      _snwprintf(ref_path, sizeof(ref_path), L"%ls/%ls", ref_dir,
                 find_data.cFileName);

      wchar_t tar_path[MAX_PATH];
      _snwprintf(tar_path, sizeof(tar_path), L"%ls/%ls", tar_dir,
                 find_data.cFileName);

      if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        mkdir_exist_ok(tar_path);
        install_shared_libs_recursive(tar_path, ref_path);
      } else {
        if (!CopyFileW(ref_path, tar_path, FALSE))
          error_exit("Failed to copy shared library\n");
      }
    }
  } while (FindNextFileW(hFind, &find_data));

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

void mkdir_exist_ok(const wchar_t *dir) {
  if (CreateDirectoryW(dir, NULL))
    return;
  if (GetLastError() != ERROR_ALREADY_EXISTS) {
    fprintf(stderr, "CreateDirectoryW: %ld\n", GetLastError());
    error_exit("Failed to create directory\n");
  }
  DWORD attr = GetFileAttributesW(dir);
  if (attr == INVALID_FILE_ATTRIBUTES) {
    fprintf(stderr, "GetFileAttributesW: %ld\n", GetLastError());
    error_exit("Failed to get directory attributes\n");
  }
  if (!(attr & FILE_ATTRIBUTE_DIRECTORY))
    error_exit("Directory name occupied by a file\n");
}

void mkdir_p(const wchar_t *dir) {
  wchar_t parent[MAX_PATH];
  int idx = 0;
  while (dir[idx]) {
    wchar_t cur = dir[idx];
    if (cur == L'/' || cur == L'\\') {
      parent[idx] = 0;
      mkdir_exist_ok(parent);
    }
    parent[idx] = cur;
    idx++;
  }
  parent[idx] = 0;
  mkdir_exist_ok(dir);
}

static bool need_quote(const wchar_t *arg) {
  if (*arg == 0)
    // empty string
    return true;

  while (*arg) {
    if (*arg == L' ' || *arg == L'\t' || *arg == L'\n' || *arg == L'\v' ||
        *arg == L'\"')
      return true;
    arg++;
  }
  return false;
}

void prepend_to_env_path(const wchar_t *path) {
  wchar_t env_path[MAX_ENV];
  int path_len = wcslen(path);
  wmemcpy(env_path, path, path_len);
  env_path[path_len] = ';';

  int remaining_len = MAX_ENV - path_len - 1;
  int get_len =
      GetEnvironmentVariableW(L"PATH", env_path + path_len + 1, remaining_len);
  if (get_len >= remaining_len)
    error_exit("PATH is too long\n");
  if (get_len == 0) {
    if (GetLastError() == ERROR_ENVVAR_NOT_FOUND) {
      env_path[path_len] = 0;
    } else {
      error_exit("Failed to get PATH\n");
    }
  }
  SetEnvironmentVariableW(L"PATH", env_path);
}

int resolve_mingw_bin_dir(wchar_t dir[MAX_PATH]) {
  const wchar_t *const rel_bin_dir = L"/bin";
  const int rel_len = wcslen(rel_bin_dir);

  int dir_len = resolve_mingw_root_dir(dir);
  if (dir_len + rel_len + 1 >= MAX_PATH)
    error_exit("Executable path is too long\n");

  wmemcpy(dir + dir_len, rel_bin_dir, rel_len);
  dir[dir_len + rel_len] = 0;
  return dir_len + rel_len;
}

int resolve_mingw_lib_shared_dir(wchar_t dir[MAX_PATH]) {
  const wchar_t *const rel_lib_shared_dir = L"/lib/shared";
  const int rel_len = wcslen(rel_lib_shared_dir);

  int dir_len = resolve_mingw_root_dir(dir);
  if (dir_len + rel_len + 1 >= MAX_PATH)
    error_exit("Executable path is too long\n");

  wmemcpy(dir + dir_len, rel_lib_shared_dir, rel_len);
  dir[dir_len + rel_len] = 0;
  return dir_len + rel_len;
}

int resolve_mingw_root_dir(wchar_t dir[MAX_PATH]) {
  const wchar_t *const rel_mingw_root_dir = L"/" MINGW_DIR;
  const int rel_len = wcslen(rel_mingw_root_dir);

  int dir_len = resolve_self_dir(dir);
  if (dir_len + rel_len + 1 >= MAX_PATH)
    error_exit("Executable path is too long\n");

  wmemcpy(dir + dir_len, rel_mingw_root_dir, rel_len);
  dir[dir_len + rel_len] = 0;
  return dir_len + rel_len;
}

int resolve_self_dir(wchar_t dir[MAX_PATH]) {
  DWORD dir_len = GetModuleFileNameW(NULL, dir, MAX_PATH);
  if (dir_len == 0)
    error_exit("Failed to get executable path\n");
  if (dir_len >= MAX_PATH)
    error_exit("Executable path is too long\n");

  while (dir[dir_len] != L'/' && dir[dir_len] != L'\\')
    dir_len--;
  dir[dir_len] = 0;
  return dir_len;
}

HANDLE spawn(const wchar_t *argv[]) {
  wchar_t cmdline[MAX_CMDLINE];
  int argi = 0;
  int idx = 0;
  // Everyone quotes command line arguments the wrong way
  // https://learn.microsoft.com/en-us/archive/blogs/twistylittlepassagesallalike/everyone-quotes-command-line-arguments-the-wrong-way
  for (; argv[argi]; argi++) {
    const wchar_t *arg = argv[argi];
    if (need_quote(arg)) {
      cmdline[idx++] = L'\"';
      for (const wchar_t *p = arg;; p++) {
        int n_backslash = 0;
        while (*p == L'\\') {
          p++;
          n_backslash++;
        }
        if (!*p) {
          // Escape all backslashes, but let the terminating double quotation
          // mark we add below be interpreted as a metawchar_tacter.
          for (int i = 0; i < n_backslash * 2; i++)
            cmdline[idx++] = L'\\';
        } else if (*p == L'\"') {
          // Escape all backslashes and the following double quotation mark.
          for (int i = 0; i < n_backslash * 2 + 1; i++)
            cmdline[idx++] = L'\\';
          cmdline[idx++] = L'\"';
        } else {
          // Backslashes aren't special here.
          for (int i = 0; i < n_backslash; i++)
            cmdline[idx++] = L'\\';
          cmdline[idx++] = *p;
        }
      }
      cmdline[idx++] = L'\"';
    } else {
      int arg_len = wcslen(arg);
      wmemcpy(cmdline + idx, arg, arg_len);
      idx += arg_len;
      cmdline[idx++] = ' ';
    }
  }
  cmdline[idx] = 0;

  STARTUPINFOW si = {sizeof(si)};
  PROCESS_INFORMATION pi = {0};
  DWORD exit_code = 0;
  if (!CreateProcessW(NULL, cmdline, NULL, NULL, TRUE, 0, NULL, NULL, &si,
                      &pi)) {
    fprintf(stderr, "CreateProcessW: %ld\n", GetLastError());
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
