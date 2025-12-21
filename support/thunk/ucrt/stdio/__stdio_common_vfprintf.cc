#include <thunk/_common.h>
#include <thunk/_no_thunk.h>

#include <stdio.h>

#include "@console_buffer.h"

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
    if (!internal::is_console(fd))
      return __ms___stdio_common_vfprintf(
          options, stream, format, locale, arglist);

    int bytes =
        __stdio_common_vsprintf(options, nullptr, 0, format, locale, arglist);

    if (bytes <= 0)
      return bytes;

    stl::string buf(bytes, 0);
    __stdio_common_vsprintf(
        options, buf.data(), buf.size(), format, locale, arglist);

    auto &buffer = internal::stdio_buffer[fd];
    buffer.append(buf.data(), buf.size());

    if (internal::is_buffered(stream)) {
      buffer.flush_complete_line(fd);
    } else {
      buffer.flush_complete_sequence(fd);
    }
    return bytes;
  }
} // namespace mingw_thunk
