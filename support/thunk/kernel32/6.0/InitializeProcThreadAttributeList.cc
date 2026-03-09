#include "InitializeProcThreadAttributeList.h"

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
    __DISPATCH_THUNK_2(InitializeProcThreadAttributeList,
                       const auto pfn =
                           try_get_InitializeProcThreadAttributeList(),
                       pfn,
                       &f::fallback_InitializeProcThreadAttributeList);

    return dllimport_InitializeProcThreadAttributeList(
        lpAttributeList, dwAttributeCount, dwFlags, lpSize);
  }

  namespace f
  {
    BOOL __stdcall fallback_InitializeProcThreadAttributeList(
        _Out_opt_ LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList,
        _In_ DWORD dwAttributeCount,
        DWORD dwFlags,
        _Inout_ PSIZE_T lpSize)
    {
      SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
      return FALSE;
    }
  } // namespace f
} // namespace mingw_thunk
