#include "CancelIoEx.h"

#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  // YY-Thunks 1.1.6
  // Windows Vista [desktop apps | UWP apps]
  // Windows Server 2008 [desktop apps | UWP apps]
  __DEFINE_THUNK(kernel32,
                 8,
                 BOOL,
                 WINAPI,
                 CancelIoEx,
                 _In_ HANDLE hFile,
                 _In_opt_ LPOVERLAPPED lpOverlapped)
  {
    __DISPATCH_THUNK_2(CancelIoEx,
                       const auto pfn = try_get_CancelIoEx(),
                       pfn,
                       &f::fallback_CancelIoEx);

    return dllimport_CancelIoEx(hFile, lpOverlapped);
  }

  namespace f
  {
    BOOL __stdcall fallback_CancelIoEx(_In_ HANDLE hFile,
                                       _In_opt_ LPOVERLAPPED lpOverlapped)
    {
      // downlevel逻辑会把该文件所有IO动作给取消掉！凑合用吧。
      return CancelIo(hFile);
    }
  } // namespace f
} // namespace mingw_thunk
