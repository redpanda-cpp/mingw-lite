#include "CopyFile2.h"

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
#if THUNK_LEVEL >= NTDDI_VISTA
    __DISPATCH_THUNK_2(CopyFile2,
                       const auto pfn = try_get_CopyFile2(),
                       pfn,
                       &f::vista_CopyFile2);
#else
    __DISPATCH_THUNK_3(CopyFile2,
                       const auto pfn = try_get_CopyFile2(),
                       pfn,
                       i::os_version() >= g::win32_vista,
                       &f::vista_CopyFile2,
                       &f::prevista_CopyFile2);
#endif

    return dllimport_CopyFile2(
        pwszExistingFileName, pwszNewFileName, pExtendedParameters);
  }

  namespace f
  {
    HRESULT __stdcall
    vista_CopyFile2(_In_ PCWSTR pwszExistingFileName,
                    _In_ PCWSTR pwszNewFileName,
                    _In_opt_ COPYFILE2_EXTENDED_PARAMETERS *pExtendedParameters)
    {
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
        if (pExtendedParameters->dwCopyFlags & COPY_FILE_COPY_SYMLINK)
          copy_flags |= COPY_FILE_COPY_SYMLINK;
        if (pExtendedParameters->dwCopyFlags & COPY_FILE_NO_BUFFERING)
          copy_flags |= COPY_FILE_NO_BUFFERING;
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

    HRESULT __stdcall prevista_CopyFile2(
        _In_ PCWSTR pwszExistingFileName,
        _In_ PCWSTR pwszNewFileName,
        _In_opt_ COPYFILE2_EXTENDED_PARAMETERS *pExtendedParameters)
    {
      DWORD ignore_flags = COPY_FILE_COPY_SYMLINK | COPY_FILE_NO_BUFFERING;

      COPYFILE2_EXTENDED_PARAMETERS params = {sizeof(params)};
      if (pExtendedParameters) {
        params.dwCopyFlags = pExtendedParameters->dwCopyFlags & ~ignore_flags;
        params.pfCancel = pExtendedParameters->pfCancel;
        params.pProgressRoutine = pExtendedParameters->pProgressRoutine;
        params.pvCallbackContext = pExtendedParameters->pvCallbackContext;
      }

      return vista_CopyFile2(pwszExistingFileName,
                             pwszNewFileName,
                             pExtendedParameters ? &params : nullptr);
    }
  } // namespace f
} // namespace mingw_thunk
