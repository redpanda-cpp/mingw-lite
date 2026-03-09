#include "LoadLibraryW.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(
      kernel32, 4, HMODULE, WINAPI, LoadLibraryW, _In_ LPCWSTR lpLibFileName)
  {
    __DISPATCH_THUNK_2(
        LoadLibraryW, i::is_nt(), &__ms_LoadLibraryW, &f::win9x_LoadLibraryW);

    return dllimport_LoadLibraryW(lpLibFileName);
  }

  namespace f
  {
    HMODULE __stdcall win9x_LoadLibraryW(_In_ LPCWSTR lpLibFileName)
    {
      if (!lpLibFileName) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return nullptr;
      }

      d::a_str a_name;
      if (!a_name.from_w(lpLibFileName)) {
        SetLastError(ERROR_OUTOFMEMORY);
        return nullptr;
      }

      return __ms_LoadLibraryA(a_name.c_str());
    }
  } // namespace f
} // namespace mingw_thunk
