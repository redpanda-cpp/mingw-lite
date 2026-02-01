#include <thunk/_common.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 4,
                 BOOL,
                 WINAPI,
                 NeedCurrentDirectoryForExePathA,
                 _In_ LPCSTR ExeName)
  {
    if (const auto pfn = try_get_NeedCurrentDirectoryForExePathA())
      return pfn(ExeName);

    stl::wstring w_exe_name = internal::a2w(ExeName);
    return NeedCurrentDirectoryForExePathW(w_exe_name.c_str());
  }
} // namespace mingw_thunk
