#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 16,
                 BOOL,
                 WINAPI,
                 InitializeProcThreadAttributeList,
                 _Out_opt_ LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList,
                 _In_ DWORD dwAttributeCount,
                 DWORD dwFlags,
                 _Inout_ PSIZE_T lpSize)
  {
    if (const auto pfn = try_get_InitializeProcThreadAttributeList())
      return pfn(lpAttributeList, dwAttributeCount, dwFlags, lpSize);

    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
  }
} // namespace mingw_thunk
