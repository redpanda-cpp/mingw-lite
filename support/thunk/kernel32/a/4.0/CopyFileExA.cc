#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  // Windows 98 (stub)
  // Windows NT 4.0
  __DEFINE_THUNK(kernel32,
                 24,
                 BOOL,
                 WINAPI,
                 CopyFileExA,
                 _In_ LPCSTR lpExistingFileName,
                 _In_ LPCSTR lpNewFileName,
                 _In_opt_ LPPROGRESS_ROUTINE lpProgressRoutine,
                 _In_opt_ LPVOID lpData,
                 _In_opt_ LPBOOL pbCancel,
                 _In_ DWORD dwCopyFlags)
  {

#if THUNK_LEVEL >= NTDDI_WIN98

    if (internal::is_nt())
      return __ms_CopyFileExA(lpExistingFileName,
                              lpNewFileName,
                              lpProgressRoutine,
                              lpData,
                              pbCancel,
                              dwCopyFlags);

#else

    if (internal::is_nt())
      return get_CopyFileExA()(lpExistingFileName,
                               lpNewFileName,
                               lpProgressRoutine,
                               lpData,
                               pbCancel,
                               dwCopyFlags);

#endif

    stl::wstring w_exist = internal::a2w(lpExistingFileName);
    stl::wstring w_new = internal::a2w(lpNewFileName);
    return CopyFileExW(w_exist.c_str(),
                       w_new.c_str(),
                       lpProgressRoutine,
                       lpData,
                       pbCancel,
                       dwCopyFlags);
  }
} // namespace mingw_thunk
