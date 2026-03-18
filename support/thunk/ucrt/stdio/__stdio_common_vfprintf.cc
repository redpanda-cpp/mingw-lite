#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/u8crt/musl.h>

#include <stdint.h>
#include <stdio.h>

#include "@stdio.h"

namespace mingw_thunk
{
  __DEFINE_THUNK(api_ms_win_crt_stdio_l1_1_0,
                 0,
                 int,
                 __cdecl,
                 __stdio_common_vfprintf,
                 uint64_t options,
                 FILE *stream,
                 const char *format,
                 _locale_t locale,
                 va_list arglist)
  {
    int fd = _fileno(stream);
    if (!i::is_console(fd))
      return __ms___stdio_common_vfprintf(
          options, stream, format, locale, arglist);

    return musl::vfprintf(musl::g_fp_from_fd(fd), format, arglist);
  }
} // namespace mingw_thunk
