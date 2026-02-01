#pragma once

#include <io.h>

namespace mingw_thunk
{
  namespace impl
  {
    intptr_t time32__wfindfirst64(const wchar_t *filespec,
                                  struct _wfinddata64_t *fileinfo);
  } // namespace impl
} // namespace mingw_thunk
