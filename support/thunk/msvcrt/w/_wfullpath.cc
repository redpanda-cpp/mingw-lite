#include "_wfullpath.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/wntcrt/errno.h>
#include <thunk/wntcrt/time.h>

#include <errno.h>
#include <stdlib.h>
#include <wchar.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt,
                 0,
                 wchar_t *,
                 __cdecl,
                 _wfullpath,
                 wchar_t *absPath,
                 const wchar_t *relPath,
                 size_t maxLength)
  {
    __DISPATCH_THUNK_2(
        _wfullpath, i::is_nt(), &__ms__wfullpath, &f::win9x__wfullpath);

    return dllimport__wfullpath(absPath, relPath, maxLength);
  }

  namespace f
  {
    wchar_t *
    win9x__wfullpath(wchar_t *absPath, const wchar_t *relPath, size_t maxLength)
    {
      wchar_t buffer[MAX_PATH];
      DWORD len = GetFullPathNameW(relPath, MAX_PATH, buffer, nullptr);

      if (len == 0) {
        internal::dosmaperr(GetLastError());
        return nullptr;
      }

      if (len >= MAX_PATH) {
        _set_errno(ENAMETOOLONG);
        return nullptr;
      }

      if (absPath && len >= maxLength) {
        _set_errno(ERANGE);
        return nullptr;
      }

      if (!absPath)
        absPath = (wchar_t *)malloc(sizeof(wchar_t) * (len + 1));
      wmemcpy(absPath, buffer, len);
      absPath[len] = 0;
      return absPath;
    }
  } // namespace f
} // namespace mingw_thunk
