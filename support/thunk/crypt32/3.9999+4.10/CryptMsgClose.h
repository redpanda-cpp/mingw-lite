#pragma once

#include <windows.h>

// after <windows.h>
#include <wincrypt.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall unimplemented_CryptMsgClose(_In_ HCRYPTMSG hCryptMsg);
  }
} // namespace mingw_thunk
