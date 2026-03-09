#pragma once

#include <windows.h>

// after <windows.h>
#include <bcrypt.h>

namespace mingw_thunk
{
  namespace f
  {
    NTSTATUS __stdcall
    fallback_BCryptGenRandom(_In_opt_ BCRYPT_ALG_HANDLE hAlgorithm,
                             _Out_writes_bytes_(_cbBuffer) PUCHAR pbBuffer,
                             _In_ ULONG cbBuffer,
                             _In_ ULONG dwFlags);
  }
} // namespace mingw_thunk
