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
    if (const auto pfn = try_get_DeleteProcThreadAttributeList())
      return pfn(lpAttributeList);
  }
} // namespace mingw_thunk
