#pragma once

#include <io.h>

namespace mingw_thunk
{
  namespace f
  {
    intptr_t win9x__wfindfirsti64(const wchar_t *filespec,
                                  struct _wfinddata32i64_t *fileinfo);
  } // namespace f
} // namespace mingw_thunk
