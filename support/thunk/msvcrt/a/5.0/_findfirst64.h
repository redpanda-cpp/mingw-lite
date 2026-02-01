#pragma once

#include <io.h>

namespace mingw_thunk
{
  namespace impl
  {
    intptr_t time32__findfirst64(const char *filespec,
                                 struct __finddata64_t *fileinfo);
  } // namespace impl
} // namespace mingw_thunk
