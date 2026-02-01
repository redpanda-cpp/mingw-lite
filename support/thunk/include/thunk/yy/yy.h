#pragma once

#include "../_no_thunk.h"

#include <stddef.h>
#include <windows.h>
#include <winternl.h>

#define _In_NLS_string_(s)

#define __WarningMessage__(...)

#define MAXIMUM_FILENAME_LENGTH 256

namespace mingw_thunk
{
  using byte = unsigned char;
}

namespace mingw_thunk::internal
{
  inline int __ascii_towlower(int c) noexcept
  {
    if (c >= 'A' && c <= 'Z')
      return c + ('a' - 'A');
    else
      return c;
  }
} // namespace mingw_thunk::internal

// YY-Thunks 1.1.6
namespace mingw_thunk::internal
{
  // Implements wcsncpmp for ASCII chars only.
  // NOTE: We can't use wcsncmp in this context because we may end up trying to
  // modify locale data structs or even calling the same function in NLS code.
  template <typename Char1, typename Char2>
  static int __fastcall StringCompareIgnoreCaseByAscii(const Char1 *string1,
                                                       const Char2 *string2,
                                                       size_t count) noexcept
  {
    wchar_t f, l;
    int result = 0;

    if (count) {
      /* validation section */
      do {
        f = __ascii_towlower(*string1);
        l = __ascii_towlower(*string2);
        string1++;
        string2++;
      } while ((--count) && f && (f == l));

      result = (int)(f - l);
    }

    return result;
  }

  static DWORD __fastcall NtStatusToDosError(_In_ NTSTATUS Status)
  {
    if (STATUS_TIMEOUT == Status) {
      /*
      https://github.com/Chuyu-Team/YY-Thunks/issues/10

      用户报告，Windows XP 无法转换
      STATUS_TIMEOUT。实际结果也是rubin，因此，特殊处理一下。
      */
      return ERROR_TIMEOUT;
    }

    return __ms_RtlNtStatusToDosError(Status);
  }

  static DWORD __fastcall BaseSetLastNTError(_In_ NTSTATUS Status)
  {
    auto lStatus = NtStatusToDosError(Status);
    SetLastError(lStatus);
    return lStatus;
  }

} // namespace mingw_thunk::internal
