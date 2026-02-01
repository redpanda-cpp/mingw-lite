#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace impl
  {
    inline BOOL winnt_get_file_information_by_handle_ex(
        HANDLE hFile,
        FILE_INFO_BY_HANDLE_CLASS FileInformationClass,
        LPVOID lpFileInformation,
        DWORD dwBufferSize);

    inline BOOL win9x_get_file_information_by_handle_ex(
        HANDLE hFile,
        FILE_INFO_BY_HANDLE_CLASS FileInformationClass,
        LPVOID lpFileInformation,
        DWORD dwBufferSize);
  } // namespace impl
} // namespace mingw_thunk
