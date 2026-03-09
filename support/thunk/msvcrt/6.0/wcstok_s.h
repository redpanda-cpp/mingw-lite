#pragma once

#include <wchar.h>

namespace mingw_thunk
{
  namespace f
  {
    wchar_t *fallback_wcstok_s(wchar_t *s, const wchar_t *sep, wchar_t **p);
  } // namespace f
} // namespace mingw_thunk
