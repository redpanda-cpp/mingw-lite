#include "CryptMsgClose.h"

#include <thunk/_common.h>

#include <windows.h>

// after <windows.h>
#include <wincrypt.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(crypt32,
                 4,
                 BOOL,
                 WINAPI,
                 CryptMsgClose,
                 _In_ HCRYPTMSG hCryptMsg)
  {
    __DISPATCH_THUNK_2(CryptMsgClose,
                       const auto pfn = try_get_CryptMsgClose(),
                       pfn,
                       &f::unimplemented_CryptMsgClose);

    return dllimport_CryptMsgClose(hCryptMsg);
  }

  namespace f
  {
    BOOL __stdcall unimplemented_CryptMsgClose(_In_ HCRYPTMSG hCryptMsg)
    {
      // don't fail on close
      return TRUE;
    }
  } // namespace f
} // namespace mingw_thunk
