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

  inline bool os_lt(int epoch, int major, int minor) noexcept
  {
    touched = true;
    return true;
  }

  inline bool os_lt(int major, int minor) noexcept
  {
    touched = true;
    return true;
  }

#else

  inline bool is_nt() noexcept
  {
    return os_version().dwPlatformId >= VER_PLATFORM_WIN32_NT;
  }

  inline bool os_lt(int epoch, int major, int minor) noexcept
  {
    auto &osvi = os_version();
    if (osvi.dwPlatformId < epoch)
      return true;
    if (osvi.dwPlatformId > epoch)
      return false;
    if (osvi.dwMajorVersion < major)
      return true;
    if (osvi.dwMajorVersion > major)
      return false;
    return osvi.dwMinorVersion < minor;
  }

  inline bool os_lt(int major, int minor) noexcept
  {
    return os_lt(VER_PLATFORM_WIN32_NT, major, minor);
  }

#endif

  inline bool os_geq(int major, int minor) noexcept
  {
    return !os_lt(major, minor);
  }

  inline bool os_geq(int epoch, int major, int minor) noexcept
  {
    return !os_lt(epoch, major, minor);
  }
} // namespace mingw_thunk::internal
