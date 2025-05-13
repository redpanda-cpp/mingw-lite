#pragma once

#include <minwindef.h>

namespace mingw_thunk::internal
{
  struct find_volume_data
  {
    DWORD volumes;
    DWORD current;
  };
} // namespace mingw_thunk::internal
