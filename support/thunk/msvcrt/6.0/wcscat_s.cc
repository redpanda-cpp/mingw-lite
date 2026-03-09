#include "wcscat_s.h"

#include <thunk/_common.h>
#include <thunk/string.h>

#include <errno.h>
#include <stdint.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt,
                 0,
                 errno_t,
                 __cdecl,
                 wcscat_s,
                 wchar_t *strDestination,
                 size_t numberOfElements,
                 const wchar_t *strSource)
  {
    __DISPATCH_THUNK_2(wcscat_s,
                       const auto pfn = try_get_wcscat_s(),
                       pfn,
                       &f::fallback_wcscat_s);

    return dllimport_wcscat_s(strDestination, numberOfElements, strSource);
  }

  namespace f
  {
    errno_t fallback_wcscat_s(wchar_t *strDestination,
                              size_t numberOfElements,
                              const wchar_t *strSource)
    {
      constexpr size_t rsize_max = SIZE_MAX >> 1;
      constexpr size_t max_len = rsize_max / sizeof(wchar_t);

      if (!strDestination || !numberOfElements || numberOfElements >= max_len) {
        _set_errno(EINVAL);
        return EINVAL;
      }

      if (!strSource) {
        *strDestination = 0;
        _set_errno(EINVAL);
        return EINVAL;
      }

      size_t dest_len = c::wcsnlen(strDestination, numberOfElements);
      if (dest_len >= numberOfElements) {
        *strDestination = 0;
        _set_errno(EINVAL);
        return EINVAL;
      }

      int err = wcscpy_s(
          strDestination + dest_len, numberOfElements - dest_len, strSource);
      if (err)
        *strDestination = 0;
      return err;
    }
  } // namespace f
} // namespace mingw_thunk
