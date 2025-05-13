#pragma once

#include <windows.h>

// YY-Thunk 1.1.6
namespace mingw_thunk
{
  namespace Downlevel
  {
    // 我们Windows 7以后才引入了组的概念，因此我们可以统一的假定
    // 只有一组，并且不支持CPU热插拔。
    static DWORD WINAPI GetProcessorCount(_In_ WORD GroupNumber)
    {
      if (GroupNumber == 0 || GroupNumber == ALL_PROCESSOR_GROUPS) {
        SYSTEM_INFO SystemInfo;
        GetSystemInfo(&SystemInfo);

        return SystemInfo.dwNumberOfProcessors;
      }

      SetLastError(ERROR_INVALID_PARAMETER);

      return 0;
    }
  } // namespace Downlevel
} // namespace mingw_thunk
