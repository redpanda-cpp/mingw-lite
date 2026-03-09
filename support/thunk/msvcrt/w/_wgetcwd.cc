#include "_wgetcwd.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>
#include <thunk/wntcrt/errno.h>

#include <errno.h>
#include <stdlib.h>
#include <wchar.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(
      msvcrt, 0, wchar_t *, __cdecl, _wgetcwd, wchar_t *buffer, int maxlen)
  {
    __DISPATCH_THUNK_2(
        _wgetcwd, i::is_nt(), &__ms__wgetcwd, &f::win9x__wgetcwd);

    return dllimport__wgetcwd(buffer, maxlen);
  }

  namespace f
  {
    wchar_t *win9x__wgetcwd(wchar_t *buffer, int maxlen)
    {
      wchar_t win32_buffer[MAX_PATH];
      DWORD len = GetCurrentDirectoryW(MAX_PATH, win32_buffer);
      if (len == 0) {
        internal::dosmaperr(GetLastError());
        return nullptr;
      } else if (len > MAX_PATH) {
        _set_errno(ENAMETOOLONG);
        return nullptr;
      } else {
        if (buffer) {
          if (len + 1 <= maxlen) {
            wmemcpy(buffer, win32_buffer, len);
            buffer[len] = 0;
            return buffer;
          } else {
            _set_errno(ERANGE);
            return nullptr;
          }
        } else {
          if (len + 1 > maxlen)
            maxlen = len + 1;
          buffer = (wchar_t *)malloc(maxlen * sizeof(wchar_t));
          if (!buffer) {
            _set_errno(ENOMEM);
            return nullptr;
          }
          wmemcpy(buffer, win32_buffer, len);
          buffer[len] = 0;
          return buffer;
        }
      }
    }
  } // namespace f
} // namespace mingw_thunk
