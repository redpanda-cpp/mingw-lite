#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <libloaderapi.h>
#include <shellapi.h>
#include <winbase.h>

#include <nostl/string.h>
#include <nostl/vector.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(shell32,
                 8,
                 LPWSTR *,
                 WINAPI,
                 CommandLineToArgvW,
                 _In_ LPCWSTR lpCmdLine,
                 _Out_ int *pNumArgs)
  {
    if (internal::is_nt())
      return get_CommandLineToArgvW()(lpCmdLine, pNumArgs);

    if (*lpCmdLine == 0) {
      *pNumArgs = 1;

      // argv[0], nullptr, path to the executable
      DWORD size = sizeof(LPWSTR) * 2 + sizeof(wchar_t) * MAX_PATH;
      LPWSTR *argv = (LPWSTR *)LocalAlloc(LMEM_FIXED, size);
      if (!argv) {
        SetLastError(ERROR_OUTOFMEMORY);
        return nullptr;
      }
      wchar_t *payload = (wchar_t *)(argv + 2);

      DWORD len = GetModuleFileNameW(nullptr, payload, MAX_PATH);
      if (len == 0) {
        LocalFree(argv);
        return nullptr;
      }

      argv[0] = payload;
      argv[1] = nullptr;
      return argv;
    }

    stl::vector<stl::wstring> args;
    stl::wstring current;
    int backslash_count = 0;
    bool in_quotes = false;
    const wchar_t *p = lpCmdLine;
    while (true) {
      if (*p == 0) {
        args.push_back(stl::move(current));
        break;
      } else if (!in_quotes && (*p == ' ' || *p == '\t')) {
        args.push_back(stl::move(current));
        current.clear();
        // eat following spaces
        while (*p == ' ' || *p == '\t')
          p++;
        // special case: no more args
        if (*p == 0)
          break;

        backslash_count = 0;
        continue;
      } else if (*p == '\\') {
        backslash_count++;
      } else if (*p == '"') {
        if (backslash_count / 2 > 0)
          // 2n backslashes and...
          current.append(backslash_count / 2, '\\');
        if (backslash_count % 2 == 0)
          // a quote that toggles in quotes mode
          in_quotes = !in_quotes;
        else
          // an escaped quote
          current.push_back('"');
        backslash_count = 0;
      } else {
        if (backslash_count > 0) {
          // backslashes aren't special here
          current.append(backslash_count, '\\');
          backslash_count = 0;
        }
        current.push_back(*p);
      }
      p++;
    }

    size_t payload_size = 0;
    for (auto &arg : args)
      payload_size += sizeof(wchar_t) * (arg.size() + 1);
    DWORD size = sizeof(LPWSTR) * (args.size() + 1) + payload_size;
    LPWSTR *argv = (LPWSTR *)LocalAlloc(LMEM_FIXED, size);
    if (!argv) {
      SetLastError(ERROR_OUTOFMEMORY);
      return nullptr;
    }
    wchar_t *payload = (wchar_t *)(argv + args.size() + 1);
    for (size_t i = 0; i < args.size(); i++) {
      argv[i] = payload;
      libc::wmemcpy(payload, args[i].c_str(), args[i].size());
      payload[args[i].size()] = 0;
      payload += args[i].size() + 1;
    }
    argv[args.size()] = nullptr;
    *pNumArgs = args.size();
    return argv;
  }
} // namespace mingw_thunk
