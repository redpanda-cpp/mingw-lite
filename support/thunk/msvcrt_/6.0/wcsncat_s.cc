#include <thunk/_common.h>

#include <errno.h>
#include <stdint.h>

#include <nocrt/wchar.h>

namespace mingw_thunk
{
  __DEFINE_CRT_THUNK(errno_t,
                     wcsncat_s,
                     wchar_t *strDest,
                     size_t numberOfElements,
                     const wchar_t *strSource,
                     size_t count)
  {
    if (const auto pfn = try_get_wcsncat_s())
      return pfn(strDest, numberOfElements, strSource, count);

    constexpr size_t rsize_max = SIZE_MAX >> 1;
    constexpr size_t max_len = rsize_max / sizeof(wchar_t);

    if (!strDest || !numberOfElements || numberOfElements >= max_len) {
      _set_errno(EINVAL);
      return EINVAL;
    }

    if (!strSource || !count || count >= max_len ||
        // overlap
        (strDest <= strSource && strDest + numberOfElements > strSource) ||
        (strSource <= strDest && strSource + count > strDest)) {
      *strDest = 0;
      _set_errno(EINVAL);
      return EINVAL;
    }

    size_t dest_len = libc::wcsnlen(strDest, numberOfElements);
    if (dest_len >= numberOfElements) {
      *strDest = 0;
      _set_errno(EINVAL);
      return EINVAL;
    }

    int err = wcsncpy_s(
        strDest + dest_len, numberOfElements - dest_len, strSource, count);
    if (err)
      *strDest = 0;
    return err;
  }
} // namespace mingw_thunk
