#pragma once

#include <windows.h>
#include <synchapi.h>

// Windows 98; stub in Windows 9x
static BOOL Win32Thunk_TryEnterCriticalSection(
  _Inout_ LPCRITICAL_SECTION lpCriticalSection
) {
  // Avoid static import. On Windows 9x,
  // previous calls to wide API should have failed.
  typedef typeof (&TryEnterCriticalSection) type;
  static type real = 0;
  static int realInit = 0;
  if (!realInit) {
    real = (type)GetProcAddress(GetModuleHandleA("kernel32.dll"), "TryEnterCriticalSection");
    realInit = 1;
  }
  if (real)
    return real(lpCriticalSection);

  return FALSE;
}

#define TryEnterCriticalSection Win32Thunk_TryEnterCriticalSection
