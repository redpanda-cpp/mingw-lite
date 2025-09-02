#include <thunk/_common.h>

#include <errno.h>
#include <stdint.h>

#include <nocrt/wchar.h>

namespace mingw_thunk
{
  __DEFINE_CRT_THUNK(errno_t,
                     wcscat_s,
                     wchar_t *strDestination,
                     size_t numberOfElements,
                     const wchar_t *strSource)
  {
    if (const auto pfn = try_get_wcscat_s())
      return pfn(strDestination, numberOfElements, strSource);

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

    size_t dest_len = libc::wcsnlen(strDestination, numberOfElements);
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
} // namespace mingw_thunk
