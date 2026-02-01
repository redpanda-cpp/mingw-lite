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
    if (const auto pfn = try_get_CopyFile2())
      return pfn(pwszExistingFileName, pwszNewFileName, pExtendedParameters);

#if THUNK_LEVEL >= NTDDI_VISTA
    return impl::winnt6_CopyFile2(
        pwszExistingFileName, pwszNewFileName, pExtendedParameters);
#else
    if (internal::os_geq(6, 0))
      return impl::winnt6_CopyFile2(
          pwszExistingFileName, pwszNewFileName, pExtendedParameters);
    else
      return impl::prent6_CopyFile2(
          pwszExistingFileName, pwszNewFileName, pExtendedParameters);
#endif
  }

  namespace impl
  {
    HRESULT
    winnt6_CopyFile2(
        _In_ PCWSTR pwszExistingFileName,
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

    HRESULT
    prent6_CopyFile2(
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

      return winnt6_CopyFile2(pwszExistingFileName,
                              pwszNewFileName,
                              pExtendedParameters ? &params : nullptr);
    }
  } // namespace impl
} // namespace mingw_thunk
