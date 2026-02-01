#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace impl
  {
    HRESULT
    winnt6_CopyFile2(_In_ PCWSTR pwszExistingFileName,
                  _In_ PCWSTR pwszNewFileName,
                  _In_opt_ COPYFILE2_EXTENDED_PARAMETERS *pExtendedParameters);

    HRESULT
    prent6_CopyFile2(_In_ PCWSTR pwszExistingFileName,
                   _In_ PCWSTR pwszNewFileName,
                   _In_opt_ COPYFILE2_EXTENDED_PARAMETERS *pExtendedParameters);
  } // namespace impl
} // namespace mingw_thunk
