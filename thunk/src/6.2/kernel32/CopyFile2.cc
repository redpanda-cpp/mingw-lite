#include <thunk/_common.h>
#include <thunk/os.h>

#include <stdint.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 12,
                 HRESULT,
                 WINAPI,
                 CopyFile2,
                 _In_ PCWSTR pwszExistingFileName,
                 _In_ PCWSTR pwszNewFileName,
                 _In_opt_ COPYFILE2_EXTENDED_PARAMETERS *pExtendedParameters)
  {
    if (const auto pfn = try_get_CopyFile2())
      return pfn(pwszExistingFileName, pwszNewFileName, pExtendedParameters);

    DWORD copy_flags = 0;
    if (pExtendedParameters) {
      if (pExtendedParameters->dwCopyFlags & COPY_FILE_FAIL_IF_EXISTS)
        copy_flags |= COPY_FILE_FAIL_IF_EXISTS;
      if (pExtendedParameters->dwCopyFlags & COPY_FILE_RESTARTABLE)
        copy_flags |= COPY_FILE_RESTARTABLE;
      if (pExtendedParameters->dwCopyFlags & COPY_FILE_OPEN_SOURCE_FOR_WRITE)
        copy_flags |= COPY_FILE_OPEN_SOURCE_FOR_WRITE;
      if (pExtendedParameters->dwCopyFlags &
          COPY_FILE_ALLOW_DECRYPTED_DESTINATION)
        copy_flags |= COPY_FILE_ALLOW_DECRYPTED_DESTINATION;
      if (internal::os_geq(6, 0)) {
        if (pExtendedParameters->dwCopyFlags & COPY_FILE_COPY_SYMLINK)
          copy_flags |= COPY_FILE_COPY_SYMLINK;
        if (pExtendedParameters->dwCopyFlags & COPY_FILE_NO_BUFFERING)
          copy_flags |= COPY_FILE_NO_BUFFERING;
      }
    }

    if (CopyFileExW(pwszExistingFileName,
                    pwszNewFileName,
                    nullptr,
                    nullptr,
                    pExtendedParameters ? pExtendedParameters->pfCancel
                                        : nullptr,
                    copy_flags))
      return S_OK;
    else
      return HRESULT_FROM_WIN32(GetLastError());
  }
} // namespace mingw_thunk
