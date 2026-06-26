#include "GetCurrentProcessorNumber.h"

#include <thunk/_common.h>

#include <cpuid.h>
#include <stdint.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32, 0, DWORD, WINAPI, GetCurrentProcessorNumber)
  {
    __DISPATCH_THUNK_3(GetCurrentProcessorNumber,
                       const auto pfn = try_get_GetCurrentProcessorNumber(),
                       pfn,
                       __get_cpuid_max(0, nullptr),
                       &f::fallback_GetCurrentProcessorNumber,
                       &f::zero_GetCurrentProcessorNumber);

    return dllimport_GetCurrentProcessorNumber();
  }

  namespace f
  {
    DWORD __stdcall fallback_GetCurrentProcessorNumber()
    {
      // Windows < NT 5.2 is x86-32 only (stop reviving IA-64) and supports no
      // more than 32 logical processors (according to Task Manager Processor
      // Affinity dialog). No more checks required.
      uint32_t eax, ebx, ecx, edx;
      __cpuid(1, eax, ebx, ecx, edx);
      return ebx >> 24;
    }

    DWORD __stdcall zero_GetCurrentProcessorNumber()
    {
      return 0;
    }
  } // namespace f
} // namespace mingw_thunk
