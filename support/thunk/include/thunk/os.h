#pragma once

#include <thunk/test.h>

#include <sysinfoapi.h>

namespace mingw_thunk::internal
{
  inline const OSVERSIONINFOA &os_version() noexcept
  {
    static OSVERSIONINFOA osvi = {sizeof(OSVERSIONINFOA)};
    if (!osvi.dwMajorVersion)
      GetVersionExA(&osvi);
    return osvi;
  }

#ifdef ENABLE_TEST_OVERRIDE

  inline bool is_nt() noexcept
  {
    touched = true;
    return false;
  }

#else

  inline bool is_nt() noexcept
  {
    return os_version().dwPlatformId >= VER_PLATFORM_WIN32_NT;
  }

#endif

  inline bool os_lt(int major, int minor) noexcept
  {
    auto osvi = os_version();
    if (!is_nt())
      return true;
    return osvi.dwMajorVersion < major ||
           (osvi.dwMajorVersion == major && osvi.dwMinorVersion < minor);
  }

  inline bool os_geq(int major, int minor) noexcept
  {
    return !os_lt(major, minor);
  }
} // namespace mingw_thunk::internal
