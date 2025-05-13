#pragma once

#include <thunk/_common.h>
#include <thunk/ddk/ntifs.h>

#include <ntdef.h>
#include <ntsecapi.h>
#include <windows.h>
#include <winternl.h>

namespace mingw_thunk
{
#define __DECLARE_NON_THUNK_TRY_GET_FUNCTION(module, name)                     \
  inline auto *try_get_##name() noexcept                                       \
  {                                                                            \
    static auto *pfn =                                                         \
        internal::module_##module.get_function<decltype(::name)>(#name);       \
    return pfn;                                                                \
  }

  namespace
  {
    __DECLARE_NON_THUNK_TRY_GET_FUNCTION(advapi32, SystemFunction036)

    __DECLARE_NON_THUNK_TRY_GET_FUNCTION(ntdll, NtQueryDirectoryFile)
    __DECLARE_NON_THUNK_TRY_GET_FUNCTION(ntdll, NtQueryInformationFile)
    __DECLARE_NON_THUNK_TRY_GET_FUNCTION(ntdll, NtQueryObject)
    __DECLARE_NON_THUNK_TRY_GET_FUNCTION(ntdll, NtSetInformationFile)
    __DECLARE_NON_THUNK_TRY_GET_FUNCTION(ntdll, RtlDosPathNameToNtPathName_U)
    __DECLARE_NON_THUNK_TRY_GET_FUNCTION(ntdll, RtlFreeUnicodeString)
    __DECLARE_NON_THUNK_TRY_GET_FUNCTION(ntdll, RtlNtStatusToDosError)
    __DECLARE_NON_THUNK_TRY_GET_FUNCTION(ntdll, RtlPcToFileHeader)
  } // namespace

#undef __DECLARE_NON_THUNK_TRY_GET_FUNCTION

} // namespace mingw_thunk
