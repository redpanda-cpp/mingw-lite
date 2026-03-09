#include "wcsncpy_s.h"

#include <thunk/_common.h>

#include <errno.h>
#include <stdint.h>

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
    __DISPATCH_THUNK_2(wcsncpy_s,
                       const auto pfn = try_get_wcsncpy_s(),
                       pfn,
                       &f::fallback_wcsncpy_s);

    return dllimport_wcsncpy_s(strDest, numberOfElements, strSource, count);
  }

  namespace f
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

      size_t len = wcsnlen(strSource, count);

      if (len >= numberOfElements) {
        *strDest = 0;
        _set_errno(ERANGE);
        return ERANGE;
      }

      wmemcpy(strDest, strSource, len);
      strDest[len] = 0;
      return 0;
    }
  } // namespace f
} // namespace mingw_thunk
