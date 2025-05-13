#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32, 0, ULONGLONG, WINAPI, GetTickCount64)
  {
    if (const auto pfn = try_get_GetTickCount64())
      return pfn();

    return GetTickCount();
  }
} // namespace mingw_thunk
