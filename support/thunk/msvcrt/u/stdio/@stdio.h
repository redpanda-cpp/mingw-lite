#pragma once

#include <io.h>
#include <stdio.h>

#include <windows.h>

namespace mingw_thunk
{
  namespace i
  {
    inline bool is_console(HANDLE fh) noexcept
    {
      DWORD mode;
      return GetConsoleMode(fh, &mode);
    }

    inline bool is_console(int fd) noexcept
    {
      return (fd == 0 || fd == 1 || fd == 2) && _isatty(fd) &&
             is_console((HANDLE)_get_osfhandle(fd));
    }

    inline bool is_console(FILE *fp) noexcept
    {
      return is_console(_fileno(fp));
    }
  } // namespace i
} // namespace mingw_thunk
