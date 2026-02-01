#pragma once

#include "_no_thunk.h"

#include <stddef.h>
#include <stringapiset.h>

#include <nocrt/string.h>
#include <nocrt/wchar.h>

#include <nostl/string.h>

namespace mingw_thunk::internal
{
  inline stl::wstring a2w(const char *str, size_t length)
  {
    // MultiByteToWideChar can be thunk'ed to treat CP_ACP as CP_UTF8
    // GetACP is also thunk'ed to report CP_UTF8
    if (length == 0)
      return {};
    int requiredLength =
        __ms_MultiByteToWideChar(CP_ACP, 0, str, length, nullptr, 0);
    stl::wstring result(requiredLength, 0);
    __ms_MultiByteToWideChar(
        CP_ACP, 0, str, length, result.data(), requiredLength);
    return result;
  }

  inline stl::wstring a2w(const char *str) noexcept
  {
    size_t length = libc::strlen(str);
    return a2w(str, length);
  }

  inline stl::wstring u2w(const char *str, size_t length) noexcept
  {
    if (length == 0)
      return {};
    int requiredLength =
        MultiByteToWideChar(CP_UTF8, 0, str, length, nullptr, 0);
    stl::wstring result(requiredLength, 0);
    MultiByteToWideChar(CP_UTF8, 0, str, length, result.data(), requiredLength);
    return result;
  }

  inline stl::wstring u2w(const char *str) noexcept
  {
    size_t length = libc::strlen(str);
    return u2w(str, length);
  }

  inline stl::string w2a(const wchar_t *str, size_t length)
  {
    // WideCharToMultiByte can be thunk'ed to treat CP_ACP as CP_UTF8
    // GetACP is also thunk'ed to report CP_UTF8
    if (length == 0)
      return {};
    int requiredLength = __ms_WideCharToMultiByte(
        CP_ACP, 0, str, length, nullptr, 0, nullptr, nullptr);
    stl::string result(requiredLength, 0);
    __ms_WideCharToMultiByte(CP_ACP,
                             0,
                             str,
                             length,
                             result.data(),
                             requiredLength,
                             nullptr,
                             nullptr);
    return result;
  }

  inline stl::string w2a(const wchar_t *str) noexcept
  {
    size_t length = libc::wcslen(str);
    return w2a(str, length);
  }

  inline stl::string w2u(const wchar_t *str, size_t length) noexcept
  {
    if (length == 0)
      return {};
    int requiredLength = WideCharToMultiByte(
        CP_UTF8, 0, str, length, nullptr, 0, nullptr, nullptr);
    stl::string result(requiredLength, 0);
    WideCharToMultiByte(CP_UTF8,
                        0,
                        str,
                        length,
                        result.data(),
                        requiredLength,
                        nullptr,
                        nullptr);
    return result;
  }

  inline stl::string w2u(const wchar_t *str) noexcept
  {
    size_t length = libc::wcslen(str);
    return w2u(str, length);
  }
} // namespace mingw_thunk::internal
