#pragma once

#include <winternl.h>

extern "C"
{
  NTSYSCALLAPI NTSTATUS NTAPI
  NtQueryDirectoryFile(_In_ HANDLE FileHandle,
                       _In_opt_ HANDLE Event OPTIONAL,
                       _In_opt_ PIO_APC_ROUTINE ApcRoutine OPTIONAL,
                       _In_opt_ PVOID ApcContext OPTIONAL,
                       _Out_ PIO_STATUS_BLOCK IoStatusBlock,
                       _Out_ PVOID FileInformation,
                       _In_ ULONG Length,
                       _In_ FILE_INFORMATION_CLASS FileInformationClass,
                       _In_ BOOLEAN ReturnSingleEntry,
                       _In_opt_ PUNICODE_STRING FileName,
                       _In_ BOOLEAN RestartScan);

  NTSYSCALLAPI VOID NTAPI RtlGetNtVersionNumbers(DWORD *major,
                                                 DWORD *minor,
                                                 DWORD *build);
}

namespace mingw_thunk
{
  namespace g
  {
    constexpr FILE_INFORMATION_CLASS FileIoPriorityHintInformation =
        FILE_INFORMATION_CLASS(43);
    constexpr FILE_INFORMATION_CLASS FileRemoteProtocolInformation =
        FILE_INFORMATION_CLASS(55);
  } // namespace g
} // namespace mingw_thunk
