#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32, 0, UINT, WINAPI, GetOEMCP)
  {
    return CP_UTF8;
  }
} // namespace mingw_thunk
