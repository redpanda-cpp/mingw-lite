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
    if (const auto pfn = try_get_wcstok_s())
      return pfn(s, sep, p);

    if (!p || !sep) {
      errno = EINVAL;
      return NULL;
    } /* added for wcstok_s */
    if (!s && !(s = *p)) {
      errno = EINVAL;
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
} // namespace mingw_thunk
