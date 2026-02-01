#include "wcsncpy_s.h"

#include <thunk/_common.h>

#include <errno.h>
#include <stdint.h>

#include <nocrt/wchar.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt,
                 0,
                 errno_t,
                 __cdecl,
                 wcsncpy_s,
                 wchar_t *strDest,
                 size_t numberOfElements,
                 const wchar_t *strSource,
                 size_t count)
  {
    if (const auto pfn = try_get_wcsncpy_s())
      return pfn(strDest, numberOfElements, strSource, count);

    return impl::fallback_wcsncpy_s(
        strDest, numberOfElements, strSource, count);
  }

  namespace impl
  {
    errno_t fallback_wcsncpy_s(wchar_t *strDest,
                               size_t numberOfElements,
                               const wchar_t *strSource,
                               size_t count)
    {
      constexpr size_t rsize_max = SIZE_MAX >> 1;
      constexpr size_t max_len = rsize_max / sizeof(wchar_t);

      if (!strDest || !numberOfElements || numberOfElements >= rsize_max) {
        _set_errno(EINVAL);
        return EINVAL;
      }

      if (!strSource || !count || count >= rsize_max ||
          // overlap
          (strDest <= strSource && strDest + numberOfElements > strSource) ||
          (strSource <= strDest && strSource + count > strDest)) {
        *strDest = 0;
        _set_errno(EINVAL);
        return EINVAL;
      }

      size_t len = libc::wcsnlen(strSource, count);

      if (len >= numberOfElements) {
        *strDest = 0;
        _set_errno(ERANGE);
        return ERANGE;
      }

      libc::wmemcpy(strDest, strSource, len);
      strDest[len] = 0;
      return 0;
    }
  } // namespace impl
} // namespace mingw_thunk
