#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    HRESULT __stdcall vista_CopyFile2(
        _In_ PCWSTR pwszExistingFileName,
        _In_ PCWSTR pwszNewFileName,
        _In_opt_ COPYFILE2_EXTENDED_PARAMETERS *pExtendedParameters);

    HRESULT __stdcall prevista_CopyFile2(
        _In_ PCWSTR pwszExistingFileName,
        _In_ PCWSTR pwszNewFileName,
        _In_opt_ COPYFILE2_EXTENDED_PARAMETERS *pExtendedParameters);
  } // namespace f
} // namespace mingw_thunk
