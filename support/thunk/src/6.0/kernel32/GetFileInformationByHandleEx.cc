#include <thunk/_common.h>
#include <thunk/ddk/wdm.h>
#include <thunk/try_get.h>
#include <thunk/yy/yy.h>

#include <ntstatus.h>
#include <windows.h>
#include <winternl.h>

namespace mingw_thunk
{
  // YY-Thunks 1.1.6
  // Windows Vista,  Windows Server 2008
  __DEFINE_THUNK(kernel32,
                 16,
                 _Success_(return) BOOL,
                 WINAPI,
                 GetFileInformationByHandleEx,
                 _In_ HANDLE hFile,
                 _In_ FILE_INFO_BY_HANDLE_CLASS FileInformationClass,
                 _Out_writes_bytes_(dwBufferSize) LPVOID lpFileInformation,
                 _In_ DWORD dwBufferSize)
  {
    if (auto const pGetFileInformationByHandleEx =
            try_get_GetFileInformationByHandleEx()) {
      return pGetFileInformationByHandleEx(
          hFile, FileInformationClass, lpFileInformation, dwBufferSize);
    }

    FILE_INFORMATION_CLASS NtFileInformationClass;
    DWORD cbMinBufferSize;
    bool bNtQueryDirectoryFile = false;
    BOOLEAN RestartScan = false;

    switch (FileInformationClass) {
    case FileBasicInfo:
      NtFileInformationClass = FileBasicInformation;
      cbMinBufferSize = sizeof(FILE_BASIC_INFO);
      break;
    case FileStandardInfo:
      NtFileInformationClass = FileStandardInformation;
      cbMinBufferSize = sizeof(FILE_STANDARD_INFO);
      break;
    case FileNameInfo:
      NtFileInformationClass = FileNameInformation;
      cbMinBufferSize = sizeof(FILE_NAME_INFO);
      break;
    case FileStreamInfo:
      NtFileInformationClass = FileStreamInformation;
      cbMinBufferSize = sizeof(FILE_STREAM_INFO);
      break;
    case FileCompressionInfo:
      NtFileInformationClass = FileCompressionInformation;
      cbMinBufferSize = sizeof(FILE_COMPRESSION_INFO);
      break;
    case FileAttributeTagInfo:
      NtFileInformationClass = FileAttributeTagInformation;
      cbMinBufferSize = sizeof(FILE_ATTRIBUTE_TAG_INFO);
      break;
    case FileIdBothDirectoryRestartInfo:
      RestartScan = true;
    case FileIdBothDirectoryInfo:
      NtFileInformationClass = FileIdBothDirectoryInformation;
      cbMinBufferSize = sizeof(FILE_ID_BOTH_DIR_INFO);
      bNtQueryDirectoryFile = true;
      break;
    case FileRemoteProtocolInfo:
      NtFileInformationClass = FileRemoteProtocolInformation;
      cbMinBufferSize = sizeof(FILE_REMOTE_PROTOCOL_INFO);
      break;
    default:
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
      break;
    }

    if (cbMinBufferSize > dwBufferSize) {
      SetLastError(ERROR_BAD_LENGTH);
      return FALSE;
    }

    IO_STATUS_BLOCK IoStatusBlock;
    NTSTATUS Status;

    if (bNtQueryDirectoryFile) {
#if !defined(__USING_NTDLL_LIB)
      const auto NtQueryDirectoryFile = try_get_NtQueryDirectoryFile();
      if (!NtQueryDirectoryFile) {
        SetLastError(ERROR_INVALID_FUNCTION);
        return FALSE;
      }
#endif

      Status = NtQueryDirectoryFile(hFile,
                                    nullptr,
                                    nullptr,
                                    nullptr,
                                    &IoStatusBlock,
                                    lpFileInformation,
                                    dwBufferSize,
                                    NtFileInformationClass,
                                    false,
                                    nullptr,
                                    RestartScan);

      if (STATUS_PENDING == Status) {
        if (WaitForSingleObjectEx(hFile, 0, FALSE) == WAIT_FAILED) {
          // WaitForSingleObjectEx会设置LastError
          return FALSE;
        }

        Status = IoStatusBlock.Status;
      }
    } else {
#if !defined(__USING_NTDLL_LIB)
      const auto NtQueryInformationFile = try_get_NtQueryInformationFile();
      if (!NtQueryInformationFile) {
        SetLastError(ERROR_INVALID_FUNCTION);
        return FALSE;
      }
#endif

      Status = NtQueryInformationFile(hFile,
                                      &IoStatusBlock,
                                      lpFileInformation,
                                      dwBufferSize,
                                      NtFileInformationClass);
    }

    if (Status >= STATUS_SUCCESS) {
      if (FileStreamInfo == FileInformationClass &&
          IoStatusBlock.Information == 0) {
        SetLastError(ERROR_HANDLE_EOF);
        return FALSE;
      } else {
        return TRUE;
      }
    } else {
      internal::BaseSetLastNTError(Status);
      return FALSE;
    }
  }
} // namespace mingw_thunk
