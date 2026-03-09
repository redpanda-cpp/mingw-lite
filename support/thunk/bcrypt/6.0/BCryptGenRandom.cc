#include "BCryptGenRandom.h"

#include <thunk/_common.h>
#include <thunk/rand.h>
#include <thunk/string.h>

#include <stdint.h>

#include <bcrypt.h>
#include <ntstatus.h>
#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(bcrypt,
                 16,
                 NTSTATUS,
                 WINAPI,
                 BCryptGenRandom,
                 _In_opt_ BCRYPT_ALG_HANDLE hAlgorithm,
                 _Out_writes_bytes_(_cbBuffer) PUCHAR pbBuffer,
                 _In_ ULONG cbBuffer,
                 _In_ ULONG dwFlags)
  {
    __DISPATCH_THUNK_2(BCryptGenRandom,
                       const auto pfn = try_get_BCryptGenRandom(),
                       pfn,
                       &f::fallback_BCryptGenRandom);

    return dllimport_BCryptGenRandom(hAlgorithm, pbBuffer, cbBuffer, dwFlags);
  }

  namespace f
  {
    NTSTATUS __stdcall
    fallback_BCryptGenRandom(_In_opt_ BCRYPT_ALG_HANDLE hAlgorithm,
                             _Out_writes_bytes_(_cbBuffer) PUCHAR pbBuffer,
                             _In_ ULONG cbBuffer,
                             _In_ ULONG dwFlags)
    {
      if (!pbBuffer)
        return STATUS_INVALID_PARAMETER;

      // Unaligned buffer is rare, here we simply ignore it.

      constexpr size_t block_size = sizeof(uint32_t);
      const size_t blocks = cbBuffer / block_size;
      const size_t remainder = cbBuffer % block_size;

      uint32_t *buffer = reinterpret_cast<uint32_t *>(pbBuffer);

      for (size_t i = 0; i < blocks; ++i)
        buffer[i] = i::__rand32();

      if (remainder) {
        uint32_t value = i::__rand32();
        c::memcpy(buffer + blocks, &value, remainder);
      }

      return STATUS_SUCCESS;
    }
  } // namespace f
} // namespace mingw_thunk
