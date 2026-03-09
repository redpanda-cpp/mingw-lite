#include "wcstok_s.h"

#include <thunk/_common.h>

#include <errno.h>
#include <wchar.h>

namespace mingw_thunk
{
  // mingw-w64 13
  __DEFINE_THUNK(msvcrt,
                 0,
                 wchar_t *,
                 __cdecl,
                 wcstok_s,
                 wchar_t *s,
                 const wchar_t *sep,
                 wchar_t **p)
  {
    __DISPATCH_THUNK_2(wcstok_s,
                       const auto pfn = try_get_wcstok_s(),
                       pfn,
                       &f::fallback_wcstok_s);

    return dllimport_wcstok_s(s, sep, p);
  }

  namespace f
  {
    wchar_t *fallback_wcstok_s(wchar_t *s, const wchar_t *sep, wchar_t **p)
    {
      if (!p || !sep) {
        _set_errno(EINVAL);
        return NULL;
      } /* added for wcstok_s */
      if (!s && !(s = *p)) {
        _set_errno(EINVAL);
        return NULL;
      }
      s += wcsspn(s, sep);
      if (!*s)
        return *p = NULL;
      *p = s + wcscspn(s, sep);
      if (**p)
        *(*p)++ = 0;
      else
        *p = 0;
      return s;
    }
  } // namespace f
} // namespace mingw_thunk
