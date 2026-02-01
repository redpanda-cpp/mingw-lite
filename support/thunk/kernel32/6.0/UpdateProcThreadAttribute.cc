#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 28,
                 BOOL,
                 WINAPI,
                 UpdateProcThreadAttribute,
                 _Inout_ LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList,
                 _In_ DWORD dwFlags,
                 _In_ DWORD_PTR Attribute,
                 _In_ PVOID lpValue,
                 _In_ SIZE_T cbSize,
                 _Out_opt_ PVOID lpPreviousValue,
                 _In_opt_ PSIZE_T lpReturnSize)
  {
    if (const auto pfn = try_get_UpdateProcThreadAttribute())
      return pfn(lpAttributeList,
                 dwFlags,
                 Attribute,
                 lpValue,
                 cbSize,
                 lpPreviousValue,
                 lpReturnSize);

    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
  }
} // namespace mingw_thunk
