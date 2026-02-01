#include <thunk/_common.h>

#include <nocrt/stdlib.h>
#include <nocrt/string.h>

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
    if (const auto pfn = try_get_BCryptGenRandom())
      return pfn(hAlgorithm, pbBuffer, cbBuffer, dwFlags);

    if (!pbBuffer)
      return STATUS_INVALID_PARAMETER;

    // Unaligned buffer is rare, here we simply ignore it.

    constexpr size_t block_size = sizeof(uint32_t);
    const size_t blocks = cbBuffer / block_size;
    const size_t remainder = cbBuffer % block_size;

    uint32_t *buffer = reinterpret_cast<uint32_t *>(pbBuffer);

    for (size_t i = 0; i < blocks; ++i)
      buffer[i] = libc::__rand32();

    if (remainder) {
      uint32_t value = libc::__rand32();
      libc::memcpy(buffer + blocks, &value, remainder);
    }

    return STATUS_SUCCESS;
  }
} // namespace mingw_thunk
