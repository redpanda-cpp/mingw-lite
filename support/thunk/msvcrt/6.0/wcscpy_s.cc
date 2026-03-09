#include "wcscpy_s.h"

#include <thunk/_common.h>

#include <errno.h>
#include <stdint.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt,
                 0,
                 errno_t,
                 __cdecl,
                 wcscpy_s,
                 wchar_t *dest,
                 rsize_t dest_size,
                 const wchar_t *src)
  {
    __DISPATCH_THUNK_2(wcscpy_s,
                       const auto pfn = try_get_wcscpy_s(),
                       pfn,
                       &f::fallback_wcscpy_s);

    return dllimport_wcscpy_s(dest, dest_size, src);
  }

  namespace f
  {
    errno_t
    fallback_wcscpy_s(wchar_t *dest, rsize_t dest_size, const wchar_t *src)
    {
      constexpr size_t rsize_max = SIZE_MAX >> 1;
      constexpr size_t max_len = rsize_max / sizeof(wchar_t);

      if (!dest || !dest_size || dest_size >= max_len) {
        _set_errno(EINVAL);
        return EINVAL;
      }

      if (!src) {
        *dest = 0;
        _set_errno(EINVAL);
        return EINVAL;
      }

      size_t len = wcsnlen(src, dest_size);

      if (len >= dest_size) {
        *dest = 0;
        _set_errno(ERANGE);
        return ERANGE;
      }

      wmemcpy(dest, src, len);
      dest[len] = 0;
      return 0;
    }
  } // namespace f
} // namespace mingw_thunk
