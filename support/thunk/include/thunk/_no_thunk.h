#pragma once

#include "_dll.h"
#include "ddk/ntifs.h"

#include <ntdef.h>
#include <shlobj.h>
#include <windows.h>
#include <winternl.h>

// after <windows.h>
#include <shellapi.h>

#include <direct.h>
#include <io.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/utime.h>

namespace mingw_thunk
{
#define __DECLARE_NON_THUNK_FUNCTION(module, name)                             \
  inline auto *module##_##name() noexcept                                      \
  {                                                                            \
    static auto *pfn =                                                         \
        internal::module_##module().get_function<decltype(::name)>(#name);     \
    return pfn;                                                                \
  }

  namespace
  {
    __DECLARE_NON_THUNK_FUNCTION(kernel32, CreateWaitableTimerA)
    __DECLARE_NON_THUNK_FUNCTION(kernel32, GetFileAttributesExA)

    __DECLARE_NON_THUNK_FUNCTION(ntdll, NtQueryDirectoryFile)
    __DECLARE_NON_THUNK_FUNCTION(ntdll, NtQueryInformationFile)
    __DECLARE_NON_THUNK_FUNCTION(ntdll, NtSetInformationFile)
    __DECLARE_NON_THUNK_FUNCTION(ntdll, RtlDosPathNameToNtPathName_U)
    __DECLARE_NON_THUNK_FUNCTION(ntdll, RtlFreeUnicodeString)
    __DECLARE_NON_THUNK_FUNCTION(ntdll, RtlPcToFileHeader)
  } // namespace

#undef __DECLARE_NON_THUNK_FUNCTION

#define __DECLARE_MS_IMPORT(name)                                              \
  extern "C" __attribute__((dllimport)) decltype(::name) __ms_##name;

  // kernel32
  __DECLARE_MS_IMPORT(CopyFileA)
  __DECLARE_MS_IMPORT(CopyFileExA)
  __DECLARE_MS_IMPORT(CopyFileW)
  __DECLARE_MS_IMPORT(CreateDirectoryA)
  __DECLARE_MS_IMPORT(CreateDirectoryW)
  __DECLARE_MS_IMPORT(CreateEventA)
  __DECLARE_MS_IMPORT(CreateEventW)
  __DECLARE_MS_IMPORT(CreateFileA)
  __DECLARE_MS_IMPORT(CreateFileW)
  __DECLARE_MS_IMPORT(CreateProcessA)
  __DECLARE_MS_IMPORT(CreateProcessW)
  __DECLARE_MS_IMPORT(CreateSemaphoreA)
  __DECLARE_MS_IMPORT(CreateSemaphoreW)
  __DECLARE_MS_IMPORT(CreateThread)
  __DECLARE_MS_IMPORT(CreateWaitableTimerA)
  __DECLARE_MS_IMPORT(CreateWaitableTimerW)
  __DECLARE_MS_IMPORT(DeleteFileA)
  __DECLARE_MS_IMPORT(DeleteFileW)
  __DECLARE_MS_IMPORT(FindFirstFileA)
  __DECLARE_MS_IMPORT(FindFirstFileW)
  __DECLARE_MS_IMPORT(FindNextFileA)
  __DECLARE_MS_IMPORT(FindNextFileW)
  __DECLARE_MS_IMPORT(FreeEnvironmentStringsA)
  __DECLARE_MS_IMPORT(FreeEnvironmentStringsW)
  __DECLARE_MS_IMPORT(GetCurrentDirectoryA)
  __DECLARE_MS_IMPORT(GetCurrentDirectoryW)
  __DECLARE_MS_IMPORT(GetEnvironmentStrings)
  __DECLARE_MS_IMPORT(GetEnvironmentStringsW)
  __DECLARE_MS_IMPORT(GetEnvironmentVariableA)
  __DECLARE_MS_IMPORT(GetEnvironmentVariableW)
  __DECLARE_MS_IMPORT(GetFileAttributesA)
  __DECLARE_MS_IMPORT(GetFileAttributesExA)
  __DECLARE_MS_IMPORT(GetFileAttributesExW)
  __DECLARE_MS_IMPORT(GetFileAttributesW)
  __DECLARE_MS_IMPORT(GetFullPathNameA)
  __DECLARE_MS_IMPORT(GetFullPathNameW)
  __DECLARE_MS_IMPORT(GetHandleInformation)
  __DECLARE_MS_IMPORT(GetModuleFileNameA)
  __DECLARE_MS_IMPORT(GetModuleFileNameW)
  __DECLARE_MS_IMPORT(GetTempPathA)
  __DECLARE_MS_IMPORT(GetTempPathW)
  __DECLARE_MS_IMPORT(LCMapStringW)
  __DECLARE_MS_IMPORT(LoadLibraryW)
  __DECLARE_MS_IMPORT(LockFileEx)
  __DECLARE_MS_IMPORT(MoveFileExA)
  __DECLARE_MS_IMPORT(MultiByteToWideChar)
  __DECLARE_MS_IMPORT(RemoveDirectoryA)
  __DECLARE_MS_IMPORT(RemoveDirectoryW)
  __DECLARE_MS_IMPORT(SearchPathA)
  __DECLARE_MS_IMPORT(SearchPathW)
  __DECLARE_MS_IMPORT(SetCurrentDirectoryA)
  __DECLARE_MS_IMPORT(SetCurrentDirectoryW)
  __DECLARE_MS_IMPORT(SetEnvironmentVariableA)
  __DECLARE_MS_IMPORT(SetEnvironmentVariableW)
  __DECLARE_MS_IMPORT(SetHandleInformation)
  __DECLARE_MS_IMPORT(TryEnterCriticalSection)
  __DECLARE_MS_IMPORT(UnlockFileEx)
  __DECLARE_MS_IMPORT(WideCharToMultiByte)
  __DECLARE_MS_IMPORT(WriteConsoleA)
  __DECLARE_MS_IMPORT(WriteConsoleW)

  // ntdll
  __DECLARE_MS_IMPORT(NtQueryDirectoryFile)
  __DECLARE_MS_IMPORT(NtQueryInformationFile)
  __DECLARE_MS_IMPORT(NtSetInformationFile)
  __DECLARE_MS_IMPORT(RtlDosPathNameToNtPathName_U)
  __DECLARE_MS_IMPORT(RtlFreeUnicodeString)
  __DECLARE_MS_IMPORT(RtlNtStatusToDosError)
  __DECLARE_MS_IMPORT(RtlPcToFileHeader)

  // shell32
  __DECLARE_MS_IMPORT(CommandLineToArgvW)
  __DECLARE_MS_IMPORT(SHGetPathFromIDListA)
  __DECLARE_MS_IMPORT(SHGetPathFromIDListW)

  // crt: environment
  __DECLARE_MS_IMPORT(_wgetcwd)

  // crt: filesystem
  __DECLARE_MS_IMPORT(_chdir)
  __DECLARE_MS_IMPORT(_chmod)
  __DECLARE_MS_IMPORT(_mkdir)
#ifndef _WIN64
  __DECLARE_MS_IMPORT(_stat32)
  __DECLARE_MS_IMPORT(_stat32i64)
#endif
  __DECLARE_MS_IMPORT(_unlink)
  __DECLARE_MS_IMPORT(_wchdir)
  __DECLARE_MS_IMPORT(_wchmod)
#ifndef _WIN64
  __DECLARE_MS_IMPORT(_wfindfirst32)
  __DECLARE_MS_IMPORT(_wfindfirst32i64)
  __DECLARE_MS_IMPORT(_wfindnext32)
  __DECLARE_MS_IMPORT(_wfindnext32i64)
#endif
  __DECLARE_MS_IMPORT(_wfullpath)
  __DECLARE_MS_IMPORT(_wmkdir)
  __DECLARE_MS_IMPORT(_wremove)
  __DECLARE_MS_IMPORT(_wrename)
#ifndef _WIN64
  __DECLARE_MS_IMPORT(_wstat32)
  __DECLARE_MS_IMPORT(_wstat32i64)
#endif
  __DECLARE_MS_IMPORT(_wunlink)
  __DECLARE_MS_IMPORT(remove)
  __DECLARE_MS_IMPORT(rename)

  // crt: runtime
  __DECLARE_MS_IMPORT(_beginthreadex)

  // crt: stdio
#ifdef _UCRT
  __DECLARE_MS_IMPORT(__stdio_common_vfprintf)
#endif
  __DECLARE_MS_IMPORT(_open)
  __DECLARE_MS_IMPORT(_wfopen)
  __DECLARE_MS_IMPORT(_wopen)
  __DECLARE_MS_IMPORT(fflush)
  __DECLARE_MS_IMPORT(fopen)
  __DECLARE_MS_IMPORT(fputc)
  __DECLARE_MS_IMPORT(fputs)
  __DECLARE_MS_IMPORT(puts)

  // crt: time
#ifndef _WIN64
  __DECLARE_MS_IMPORT(_wutime32)
  __DECLARE_MS_IMPORT(_utime32)
#endif

#undef __DECLARE_MS_IMPORT

} // namespace mingw_thunk

#define NTDDI_WIN4 0x04000000
#define NTDDI_WINME (0x03FF0000 + 3000)
#define NTDDI_WIN98 (0x03FF0000 + 1998)
#define NTDDI_WIN95 (0x03FF0000 + 950)
