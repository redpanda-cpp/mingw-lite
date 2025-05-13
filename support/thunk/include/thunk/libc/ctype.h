#pragma once

namespace mingw_thunk::internal
{
  inline int isalpha(int c)
  {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
  }
} // namespace mingw_thunk::internal
