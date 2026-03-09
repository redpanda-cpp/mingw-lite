#pragma once

#include "_no_thunk.h"

#include <stdint.h>

#include <sysinfoapi.h>

namespace mingw_thunk
{
  namespace d
  {
    struct win32_version
    {
      uint32_t platform;
      uint32_t major;
      uint32_t minor;

      bool operator==(const win32_version &rhs) const noexcept
      {
        return platform == rhs.platform && major == rhs.major &&
               minor == rhs.minor;
      }

      bool operator!=(const win32_version &rhs) const noexcept
      {
        return !(*this == rhs);
      }

      bool operator<(const win32_version &rhs) const noexcept
      {
        if (platform < rhs.platform)
          return true;
        if (platform > rhs.platform)
          return false;
        if (major < rhs.major)
          return true;
        if (major > rhs.major)
          return false;
        return minor < rhs.minor;
      }

      bool operator>=(const win32_version &rhs) const noexcept
      {
        return !(*this < rhs);
      }

      bool operator>(const win32_version &rhs) const noexcept
      {
        return rhs < *this;
      }

      bool operator<=(const win32_version &rhs) const noexcept
      {
        return !(rhs < *this);
      }
    };
  } // namespace d

  namespace g
  {
    constexpr d::win32_version win32_win98 = {1, 4, 10};
    constexpr d::win32_version win32_winxp = {2, 5, 1};
    constexpr d::win32_version win32_vista = {2, 6, 0};
  } // namespace g

  namespace i
  {
    inline const d::win32_version os_version() noexcept
    {
      static OSVERSIONINFOA osvi = {sizeof(OSVERSIONINFOA)};
      if (!osvi.dwMajorVersion) {
        GetVersionExA(&osvi);
#if THUNK_LEVEL >= NTDDI_WINXP
        const auto RtlGetNtVersionNumbers = &__ms_RtlGetNtVersionNumbers;
#else
        const auto RtlGetNtVersionNumbers = ntdll_RtlGetNtVersionNumbers();
#endif
        if (RtlGetNtVersionNumbers)
          RtlGetNtVersionNumbers(
              &osvi.dwMajorVersion, &osvi.dwMinorVersion, &osvi.dwBuildNumber);
      }
      return {osvi.dwPlatformId, osvi.dwMajorVersion, osvi.dwMinorVersion};
    }

    inline bool is_nt() noexcept
    {
      return os_version().platform >= VER_PLATFORM_WIN32_NT;
    }
  } // namespace i
} // namespace mingw_thunk
