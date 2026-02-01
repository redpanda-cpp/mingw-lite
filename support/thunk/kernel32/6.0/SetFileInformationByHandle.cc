#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/ddk/wdm.h>
#include <thunk/yy/yy.h>

#include <ntstatus.h>
#include <windows.h>
#include <winternl.h>

namespace mingw_thunk
{
  // YY-Thunks 1.1.6
  // Modification: use `GetProcessHeap()` instead of reserved PEB field.
  // Windows Vista,  Windows Server 2008
  __DEFINE_THUNK(kernel32,
                 16,
                 BOOL,
                 WINAPI,
                 SetFileInformationByHandle,
                 _In_ HANDLE hFile,
                 _In_ FILE_INFO_BY_HANDLE_CLASS FileInformationClass,
                 _In_reads_bytes_(dwBufferSize) LPVOID lpFileInformation,
                 _In_ DWORD dwBufferSize)
  {
    if (auto pSetFileInformationByHandle =
            try_get_SetFileInformationByHandle()) {
      return pSetFileInformationByHandle(
          hFile, FileInformationClass, lpFileInformation, dwBufferSize);
    }

#if THUNK_LEVEL >= NTDDI_WIN4
    const auto NtSetInformationFile = __ms_NtSetInformationFile;
#else
    const auto NtSetInformationFile = ntdll_NtSetInformationFile();
    if (!NtSetInformationFile) {
      SetLastError(ERROR_INVALID_FUNCTION);
      return FALSE;
    }
#endif

    const auto ProcessHeap = GetProcessHeap();
    FILE_INFORMATION_CLASS NtFileInformationClass;
    DWORD cbMinBufferSize;
    bool bFreeFileInformation = false;

    switch (FileInformationClass) {
    case FileBasicInfo:
      NtFileInformationClass = FileBasicInformation;
      cbMinBufferSize = sizeof(FILE_BASIC_INFO);
      break;
    case FileRenameInfo:
      NtFileInformationClass = FileRenameInformation;
      cbMinBufferSize = sizeof(FILE_RENAME_INFO);

      if (cbMinBufferSize > dwBufferSize) {
        SetLastError(ERROR_BAD_LENGTH);
        return FALSE;
      }

      if (lpFileInformation == nullptr) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
      } else {
        auto pRenameInfo = (FILE_RENAME_INFO *)lpFileInformation;

        if (pRenameInfo->FileNameLength < sizeof(wchar_t) ||
            pRenameInfo->FileName[0] != L':') {
#if THUNK_LEVEL >= NTDDI_WIN4
          const auto RtlDosPathNameToNtPathName_U =
              __ms_RtlDosPathNameToNtPathName_U;
          const auto RtlFreeUnicodeString = __ms_RtlFreeUnicodeString;
#else
          const auto RtlDosPathNameToNtPathName_U =
              ntdll_RtlDosPathNameToNtPathName_U();
          const auto RtlFreeUnicodeString = ntdll_RtlFreeUnicodeString();

          if (RtlDosPathNameToNtPathName_U == nullptr ||
              RtlFreeUnicodeString == nullptr) {
            SetLastError(ERROR_INVALID_FUNCTION);
            return FALSE;
          }
#endif

          UNICODE_STRING NtName = {};

          if (!RtlDosPathNameToNtPathName_U(
                  pRenameInfo->FileName, &NtName, nullptr, nullptr)) {
            SetLastError(ERROR_INVALID_PARAMETER);

            return FALSE;
          }

          DWORD dwNewBufferSize = sizeof(FILE_RENAME_INFO) + NtName.Length;

          auto NewRenameInfo =
              (FILE_RENAME_INFO *)HeapAlloc(ProcessHeap, 0, dwNewBufferSize);
          if (!NewRenameInfo) {
            auto lStatus = GetLastError();

            RtlFreeUnicodeString(&NtName);

            SetLastError(lStatus);
            return FALSE;
          }

          bFreeFileInformation = true;

          NewRenameInfo->ReplaceIfExists = pRenameInfo->ReplaceIfExists;
          NewRenameInfo->RootDirectory = pRenameInfo->RootDirectory;
          NewRenameInfo->FileNameLength = NtName.Length;

          memcpy(NewRenameInfo->FileName, NtName.Buffer, NtName.Length);

          *(wchar_t *)((byte *)NewRenameInfo->FileName + NtName.Length) = L'\0';

          lpFileInformation = NewRenameInfo;
          dwBufferSize = dwNewBufferSize;

          RtlFreeUnicodeString(&NtName);
        }
      }
      break;
    case FileDispositionInfo:
      NtFileInformationClass = FileDispositionInformation;
      cbMinBufferSize = sizeof(FILE_DISPOSITION_INFO);
      break;
    case FileAllocationInfo:
      NtFileInformationClass = FileAllocationInformation;
      cbMinBufferSize = sizeof(FILE_ALLOCATION_INFO);
      break;
    case FileEndOfFileInfo:
      NtFileInformationClass = FileEndOfFileInformation;
      cbMinBufferSize = sizeof(FILE_END_OF_FILE_INFO);
      break;
    case FileIoPriorityHintInfo:
      NtFileInformationClass = FileIoPriorityHintInformation;
      cbMinBufferSize = sizeof(FILE_IO_PRIORITY_HINT_INFO);

      // 长度检查，微软原版似乎没有该安全检查
      if (cbMinBufferSize > dwBufferSize) {
        SetLastError(ERROR_BAD_LENGTH);
        return FALSE;
      }

      if (lpFileInformation == nullptr ||
          ((FILE_IO_PRIORITY_HINT_INFO *)lpFileInformation)->PriorityHint >=
              MaximumIoPriorityHintType) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
      }

      break;
    default:
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
      break;
    }

    if (cbMinBufferSize > dwBufferSize) {
      if (bFreeFileInformation) {
        HeapFree(ProcessHeap, 0, lpFileInformation);
      }

      SetLastError(ERROR_BAD_LENGTH);
      return FALSE;
    }

    IO_STATUS_BLOCK IoStatusBlock;

    LONG Status = NtSetInformationFile(hFile,
                                       &IoStatusBlock,
                                       lpFileInformation,
                                       dwBufferSize,
                                       NtFileInformationClass);

    if (bFreeFileInformation) {
      HeapFree(ProcessHeap, 0, lpFileInformation);
    }

    if (Status >= STATUS_SUCCESS)
      return TRUE;

    internal::BaseSetLastNTError(Status);
    return FALSE;
  }
} // namespace mingw_thunk
