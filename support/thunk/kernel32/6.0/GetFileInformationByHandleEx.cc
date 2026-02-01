#include "GetFileInformationByHandleEx.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/ddk/wdm.h>
#include <thunk/os.h>
#include <thunk/yy/yy.h>

#include <ntstatus.h>
#include <windows.h>
#include <winternl.h>

namespace mingw_thunk
{
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
    if (const auto pfn = try_get_GetFileInformationByHandleEx())
      return pfn(hFile, FileInformationClass, lpFileInformation, dwBufferSize);

    if (internal::is_nt())
      return impl::winnt_get_file_information_by_handle_ex(
          hFile, FileInformationClass, lpFileInformation, dwBufferSize);

    return impl::win9x_get_file_information_by_handle_ex(
        hFile, FileInformationClass, lpFileInformation, dwBufferSize);
  }

  namespace impl
  {
    BOOL win9x_get_file_information_by_handle_ex(
        HANDLE hFile,
        FILE_INFO_BY_HANDLE_CLASS FileInformationClass,
        LPVOID lpFileInformation,
        DWORD dwBufferSize)
    {
      switch (FileInformationClass) {
      case FileBasicInfo: {
        auto *basic_info = (FILE_BASIC_INFORMATION *)lpFileInformation;
        BY_HANDLE_FILE_INFORMATION info;
        bool ok = GetFileInformationByHandle(hFile, &info);
        if (!ok)
          return FALSE;
        basic_info->CreationTime = {
            .LowPart = info.ftCreationTime.dwLowDateTime,
            .HighPart = LONG(info.ftCreationTime.dwHighDateTime),
        };
        basic_info->LastAccessTime = {
            .LowPart = info.ftLastAccessTime.dwLowDateTime,
            .HighPart = LONG(info.ftLastAccessTime.dwHighDateTime),
        };
        basic_info->LastWriteTime = {
            .LowPart = info.ftLastWriteTime.dwLowDateTime,
            .HighPart = LONG(info.ftLastWriteTime.dwHighDateTime),
        };
        basic_info->ChangeTime = {
            .LowPart = info.ftLastWriteTime.dwLowDateTime,
            .HighPart = LONG(info.ftLastWriteTime.dwHighDateTime),
        };
        basic_info->FileAttributes = info.dwFileAttributes;
        return TRUE;
      }
      case FileAttributeTagInfo: {
        auto *tag_info = (FILE_ATTRIBUTE_TAG_INFO *)lpFileInformation;
        BY_HANDLE_FILE_INFORMATION info;
        bool ok = GetFileInformationByHandle(hFile, &info);
        if (!ok)
          return FALSE;
        tag_info->FileAttributes = info.dwFileAttributes;
        tag_info->ReparseTag = 0;
        return TRUE;
      }
      case FileIdInfo: {
        auto *id_info = (FILE_ID_INFO *)lpFileInformation;
        BY_HANDLE_FILE_INFORMATION info;
        bool ok = GetFileInformationByHandle(hFile, &info);
        if (!ok)
          return FALSE;
        id_info->VolumeSerialNumber = info.dwVolumeSerialNumber;
        id_info->FileId = {
            BYTE(info.nFileIndexLow),
            BYTE(info.nFileIndexLow >> 8),
            BYTE(info.nFileIndexLow >> 16),
            BYTE(info.nFileIndexLow >> 24),
            BYTE(info.nFileIndexHigh),
            BYTE(info.nFileIndexHigh >> 8),
            BYTE(info.nFileIndexHigh >> 16),
            BYTE(info.nFileIndexHigh >> 24),
        };
        return TRUE;
      }
      default:
        SetLastError(ERROR_INVALID_FUNCTION);
        return FALSE;
      }
    }

    // YY-Thunks 1.1.6
    // Windows Vista,  Windows Server 2008
    BOOL winnt_get_file_information_by_handle_ex(
        HANDLE hFile,
        FILE_INFO_BY_HANDLE_CLASS FileInformationClass,
        LPVOID lpFileInformation,
        DWORD dwBufferSize)
    {
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
#if THUNK_LEVEL >= NTDDI_WIN4
        const auto NtQueryDirectoryFile = __ms_NtQueryDirectoryFile;
#else
        const auto NtQueryDirectoryFile = ntdll_NtQueryDirectoryFile();
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
#if THUNK_LEVEL >= NTDDI_WIN4
        const auto NtQueryInformationFile = __ms_NtQueryInformationFile;
#else
        const auto NtQueryInformationFile = ntdll_NtQueryInformationFile();
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
  } // namespace impl
} // namespace mingw_thunk
