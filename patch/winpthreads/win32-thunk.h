#pragma once

#include <windows.h>
#include <synchapi.h>

static int Win32Thunk_Detail_IsNt()
{
  static int isNt = 0;
  static int isNtInit = 0;

  if (!isNtInit) {
    OSVERSIONINFOA osvi = { sizeof (OSVERSIONINFOA) };
    GetVersionExA (&osvi);
    isNt = osvi.dwPlatformId == VER_PLATFORM_WIN32_NT;
    isNtInit = 1;
  }

  return isNt;
}

// corresponding to TryEnterCriticalSection implementation
static void Win32Thunk_DeleteCriticalSection(
  _Inout_ LPCRITICAL_SECTION lpCriticalSection
) {
  if (Win32Thunk_Detail_IsNt())
    return DeleteCriticalSection(lpCriticalSection);

  HANDLE h = *(HANDLE *)lpCriticalSection;
  CloseHandle(h);
  return;
}

// corresponding to TryEnterCriticalSection implementation
static void Win32Thunk_EnterCriticalSection(
  _Inout_ LPCRITICAL_SECTION lpCriticalSection
) {
  if (Win32Thunk_Detail_IsNt())
    return EnterCriticalSection(lpCriticalSection);

  HANDLE h = *(HANDLE *)lpCriticalSection;
  DWORD r = WaitForSingleObject(h, INFINITE);
  if (r != WAIT_OBJECT_0)
    // EXCEPTION_POSSIBLE_DEADLOCK
    RaiseException((LONG)0xC0000194, 0, 0, NULL);
  return;
}

// not work in Windows 9x
static BOOL Win32Thunk_GetHandleInformation(
  _In_ HANDLE hObject,
  _Out_ LPDWORD lpdwFlags
) {
  if (Win32Thunk_Detail_IsNt())
    return GetHandleInformation(hObject, lpdwFlags);

  // only return value matters
  return TRUE;
}

// corresponding to TryEnterCriticalSection implementation
static void Win32Thunk_InitializeCriticalSection(
  _Out_ LPCRITICAL_SECTION lpCriticalSection
) {
  if (Win32Thunk_Detail_IsNt())
    return InitializeCriticalSection(lpCriticalSection);

  HANDLE h = CreateMutex(NULL, FALSE, NULL);
  *(HANDLE *)lpCriticalSection = h;
  return;
}

// Windows 98
static BOOL Win32Thunk_IsDebuggerPresent()
{
  typedef typeof (&IsDebuggerPresent) type;
  static type real = 0;
  static int realInit = 0;

  if (!realInit) {
    real = (type)GetProcAddress(GetModuleHandleA("kernel32.dll"), "IsDebuggerPresent");
    realInit = 1;
  }
  if (real)
    return real();

  return FALSE;
}

// corresponding to TryEnterCriticalSection implementation
static void Win32Thunk_LeaveCriticalSection(
  _Inout_ LPCRITICAL_SECTION lpCriticalSection
) {
  if (Win32Thunk_Detail_IsNt())
    return LeaveCriticalSection(lpCriticalSection);

  HANDLE h = *(HANDLE *)lpCriticalSection;
  ReleaseMutex(h);
}

// Windows 98
static BOOL Win32Thunk_SetProcessAffinityMask(
  _In_ HANDLE hProcess,
  _In_ DWORD_PTR dwProcessAffinityMask
) {
  typedef typeof (&SetProcessAffinityMask) type;
  static type real = 0;
  static int realInit = 0;

  if (!realInit) {
    real = (type)GetProcAddress(GetModuleHandleA("kernel32.dll"), "SetProcessAffinityMask");
    realInit = 1;
  }
  if (real)
    return real(hProcess, dwProcessAffinityMask);

  SetLastError(ERROR_NOT_SUPPORTED);
  return FALSE;
}

// Windows 98; not work in Windows 9x
static BOOL Win32Thunk_TryEnterCriticalSection(
  _Inout_ LPCRITICAL_SECTION lpCriticalSection
) {
  typedef typeof (&TryEnterCriticalSection) type;
  static type real = 0;
  static int realInit = 0;

  if (Win32Thunk_Detail_IsNt()) {
    if (!realInit) {
      real = (type)GetProcAddress(GetModuleHandleA("kernel32.dll"), "TryEnterCriticalSection");
      realInit = 1;
    }
    return real(lpCriticalSection);
  }

  // use kernel-mode mutex
  HANDLE h = *(HANDLE *)lpCriticalSection;
  DWORD r = WaitForSingleObject(h, 0);
  return r == WAIT_OBJECT_0;
}

#define DeleteCriticalSection Win32Thunk_DeleteCriticalSection
#define EnterCriticalSection Win32Thunk_EnterCriticalSection
#define GetHandleInformation Win32Thunk_GetHandleInformation
#define InitializeCriticalSection Win32Thunk_InitializeCriticalSection
#define IsDebuggerPresent Win32Thunk_IsDebuggerPresent
#define LeaveCriticalSection Win32Thunk_LeaveCriticalSection
#define SetProcessAffinityMask Win32Thunk_SetProcessAffinityMask
#define TryEnterCriticalSection Win32Thunk_TryEnterCriticalSection
