#include <thunk/_common.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(
      kernel32, 4, HMODULE, WINAPI, LoadLibraryA, _In_ LPCSTR lpLibFileName)
  {
    if (!lpLibFileName) {
      SetLastError(ERROR_INVALID_PARAMETER);
      return nullptr;
    }

    d::w_str w_name;
    if (!w_name.from_u(lpLibFileName)) {
      SetLastError(ERROR_OUTOFMEMORY);
      return nullptr;
    }

    return LoadLibraryW(w_name.data());
  }
} // namespace mingw_thunk
