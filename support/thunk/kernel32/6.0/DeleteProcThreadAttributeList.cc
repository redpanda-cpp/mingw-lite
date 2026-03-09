#include "DeleteProcThreadAttributeList.h"

#include <thunk/_common.h>

#include <processthreadsapi.h>
#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 4,
                 VOID,
                 WINAPI,
                 DeleteProcThreadAttributeList,
                 _Inout_ LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList)
  {
    __DISPATCH_THUNK_2(DeleteProcThreadAttributeList,
                       const auto pfn = try_get_DeleteProcThreadAttributeList(),
                       pfn,
                       &f::fallback_DeleteProcThreadAttributeList);

    return dllimport_DeleteProcThreadAttributeList(lpAttributeList);
  }

  namespace f
  {
    VOID __stdcall fallback_DeleteProcThreadAttributeList(
        _Inout_ LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList)
    {
      (void)lpAttributeList;
    }
  } // namespace f
} // namespace mingw_thunk
