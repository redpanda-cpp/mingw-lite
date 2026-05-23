#include "MapViewOfFileExNuma.h"

#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 28,
                 LPVOID,
                 WINAPI,
                 MapViewOfFileExNuma,
                 _In_ HANDLE hFileMappingObject,
                 _In_ DWORD dwDesiredAccess,
                 _In_ DWORD dwFileOffsetHigh,
                 _In_ DWORD dwFileOffsetLow,
                 _In_ SIZE_T dwNumberOfBytesToMap,
                 _In_opt_ LPVOID lpBaseAddress,
                 _In_ DWORD nndPreferred)
  {
    __DISPATCH_THUNK_2(MapViewOfFileExNuma,
                       const auto pfn = try_get_MapViewOfFileExNuma(),
                       pfn,
                       &f::fallback_MapViewOfFileExNuma);

    return dllimport_MapViewOfFileExNuma(hFileMappingObject,
                                         dwDesiredAccess,
                                         dwFileOffsetHigh,
                                         dwFileOffsetLow,
                                         dwNumberOfBytesToMap,
                                         lpBaseAddress,
                                         nndPreferred);
  }

  namespace f
  {
    LPVOID __stdcall
    fallback_MapViewOfFileExNuma(_In_ HANDLE hFileMappingObject,
                                 _In_ DWORD dwDesiredAccess,
                                 _In_ DWORD dwFileOffsetHigh,
                                 _In_ DWORD dwFileOffsetLow,
                                 _In_ SIZE_T dwNumberOfBytesToMap,
                                 _In_opt_ LPVOID lpBaseAddress,
                                 _In_ DWORD nndPreferred)
    {
      return MapViewOfFileEx(hFileMappingObject,
                             dwDesiredAccess,
                             dwFileOffsetHigh,
                             dwFileOffsetLow,
                             dwNumberOfBytesToMap,
                             lpBaseAddress);
    }
  } // namespace f
} // namespace mingw_thunk
