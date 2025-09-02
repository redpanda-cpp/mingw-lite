#pragma once

#include <stddef.h>
#include <stringapiset.h>

#include <nocrt/string.h>
#include <nocrt/wchar.h>

#include <nostl/string.h>

namespace mingw_thunk::internal
{
  inline stl::wstring widen(const char *str, size_t length)
  {
    if (length == 0)
      return {};
    int requiredLength =
        MultiByteToWideChar(CP_ACP, 0, str, length, nullptr, 0);
    stl::wstring result(requiredLength, 0);
    MultiByteToWideChar(CP_ACP, 0, str, length, result.data(), requiredLength);
    return result;
  }

  inline stl::wstring widen(const char *str) noexcept
  {
    size_t length = libc::strlen(str);
    return widen(str, length);
  }

  inline stl::string narrow(const wchar_t *str, size_t length)
  {
    if (length == 0)
      return {};
    int requiredLength = WideCharToMultiByte(
        CP_ACP, 0, str, length, nullptr, 0, nullptr, nullptr);
    stl::string result(requiredLength, 0);
    WideCharToMultiByte(CP_ACP,
                        0,
                        str,
                        length,
                        result.data(),
                        requiredLength,
                        nullptr,
                        nullptr);
    return result;
  }

  inline stl::string narrow(const wchar_t *str) noexcept
  {
    size_t length = libc::wcslen(str);
    return narrow(str, length);
  }
} // namespace mingw_thunk::internal
