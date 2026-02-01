#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/string.h>
#include <thunk/wntcrt/errno.h>

#include <errno.h>
#include <stdlib.h>
#include <wchar.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(
      msvcrt, 0, wchar_t *, __cdecl, _wgetcwd, wchar_t *buffer, int maxlen)
  {
    if (internal::is_nt())
      return __ms__wgetcwd(buffer, maxlen);

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
} // namespace mingw_thunk
