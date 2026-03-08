#include "CommandLineToArgvW.h"

#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/string.h>

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
      return __ms_CommandLineToArgvW(lpCmdLine, pNumArgs);

    return impl::win9x_CommandLineToArgvW(lpCmdLine, pNumArgs);
  }

  namespace impl
  {
    namespace
    {
      void parse_arguments(const wchar_t *lpCmdLine,
                           wchar_t **argv,
                           wchar_t *payload,
                           int *arg_count,
                           size_t *payload_size)
      {
        const wchar_t *src = lpCmdLine;
        wchar_t *dst = payload;
        int argc = 1;
        size_t size = 0;
        int in_quotes = 0;

        if (argv)
          argv[0] = payload;

        // special handling argv[0], assuming there's no quotes in filename.
        // that's not true, WSL interop may yield such case.
        if (*src == L'"') {
          src++;
          while (*src != L'"') {
            if (!*src)
              break;
            size++;
            if (dst)
              *dst++ = *src;
            src++;
          }
          size++;
          if (dst)
            *dst++ = L'\0';
          if (*src == L'"')
            src++;
        } else {
          wchar_t ch;
          do {
            ch = *src;
            size++;
            if (dst)
              *dst++ = ch;
            src++;
          } while (ch > L' ' && ch != L'\0');
          if (ch) {
            if (dst)
              *(dst - 1) = L'\0';
          } else {
            src--;
          }
        }

        // remaining arguments
        while (*src) {
          while (*src == L' ' || *src == L'\t')
            src++;

          if (!*src)
            break;

          if (argv)
            argv[argc] = dst;
          argc++;

          while (*src) {
            int backslash_count = 0;
            while (*src == L'\\') {
              backslash_count++;
              src++;
            }

            bool copy_char = true;
            if (*src == L'"') {
              if (backslash_count % 2 == 0) {
                if (in_quotes && src[1] == L'"')
                  src++;
                else
                  copy_char = false;
                in_quotes = !in_quotes;
              }
              backslash_count /= 2;
            }

            while (backslash_count > 0) {
              size++;
              if (dst)
                *dst++ = L'\\';
              backslash_count--;
            }

            if (!*src || (!in_quotes && (*src == L' ' || *src == L'\t')))
              break;

            if (copy_char) {
              size++;
              if (dst)
                *dst++ = *src;
            }
            src++;
          }

          size++;
          if (dst)
            *dst++ = L'\0';
        }

        *arg_count = argc;
        *payload_size = size;
      }
    } // namespace

    LPWSTR *win9x_CommandLineToArgvW(_In_ LPCWSTR lpCmdLine,
                                     _Out_ int *pNumArgs)
    {
      if (!pNumArgs) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return nullptr;
      }

      if (*lpCmdLine == 0) {
        *pNumArgs = 1;

        DWORD size = sizeof(LPWSTR) * 2 + sizeof(wchar_t) * MAX_PATH;
        LPWSTR *argv = (LPWSTR *)LocalAlloc(LMEM_FIXED, size);
        if (!argv) {
          SetLastError(ERROR_NOT_ENOUGH_MEMORY);
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

      int argc = 0;
      size_t payload_size = 0;
      parse_arguments(lpCmdLine, nullptr, nullptr, &argc, &payload_size);

      size_t buffer_size =
          sizeof(LPWSTR) * (argc + 1) + payload_size * sizeof(wchar_t);
      LPWSTR *argv = (LPWSTR *)LocalAlloc(LMEM_FIXED, buffer_size);
      if (!argv) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return nullptr;
      }
      wchar_t *payload = (wchar_t *)(argv + argc + 1);

      parse_arguments(lpCmdLine, argv, payload, &argc, &payload_size);

      argv[argc] = nullptr;
      *pNumArgs = argc;
      return argv;
    }
  } // namespace impl
} // namespace mingw_thunk
