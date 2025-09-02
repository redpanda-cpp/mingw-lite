#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(
      kernel32, 4, BOOL, WINAPI, DeleteFileW, _In_ LPCWSTR lpFileName)
  {
    if (internal::is_nt())
      return get_DeleteFileW()(lpFileName);

    auto aname = internal::narrow(lpFileName);
    return DeleteFileA(aname.c_str());
  }
} // namespace mingw_thunk
