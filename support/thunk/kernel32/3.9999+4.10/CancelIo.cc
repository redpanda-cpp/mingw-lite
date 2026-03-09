#include "CancelIo.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32, 4, BOOL, WINAPI, CancelIo, _In_ HANDLE hFile)
  {
    __DISPATCH_THUNK_2(CancelIo,
                       const auto pfn = try_get_CancelIo(),
                       pfn,
                       &f::fallback_CancelIo);

    return dllimport_CancelIo(hFile);
  }

  namespace f
  {
    BOOL __stdcall fallback_CancelIo(_In_ HANDLE hFile)
    {
      SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
      return FALSE;
    }
  } // namespace f
} // namespace mingw_thunk
