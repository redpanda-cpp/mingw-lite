#pragma once

namespace mingw_thunk
{
  namespace c
  {
    inline int isalpha(int c)
    {
      return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }
  } // namespace c
} // namespace mingw_thunk
