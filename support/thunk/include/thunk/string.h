#pragma once

#include <thunk/stl/string.h>

#include <stddef.h>
#include <stringapiset.h>

namespace mingw_thunk::internal
{
  inline wstring widen(const char *str, size_t length)
  {
    if (length == 0)
      return {};
    int requiredLength =
        MultiByteToWideChar(CP_ACP, 0, str, length, nullptr, 0);
    wstring result(requiredLength, 0);
    if (!result.valid())
      return result;
    MultiByteToWideChar(CP_ACP, 0, str, length, result.data(), requiredLength);
    return result;
  }

  inline wstring widen(const char *str) noexcept
  {
    size_t length = _strlen(str);
    return widen(str, length);
  }

  inline string narrow(const wchar_t *str, size_t length)
  {
    if (length == 0)
      return {};
    int requiredLength = WideCharToMultiByte(
        CP_ACP, 0, str, length, nullptr, 0, nullptr, nullptr);
    string result(requiredLength, 0);
    if (!result.valid())
      return result;
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

  inline string narrow(const wchar_t *str) noexcept
  {
    size_t length = _strlen(str);
    return narrow(str, length);
  }
} // namespace mingw_thunk::internal
