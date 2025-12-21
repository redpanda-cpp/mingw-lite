#include "_common.h"

#include "string.h"

#include <fileapi.h>

namespace mingw_thunk::internal
{
  inline WIN32_FIND_DATAW a2w(const WIN32_FIND_DATAA &narrow)
  {
    WIN32_FIND_DATAW res{};

    res.dwFileAttributes = narrow.dwFileAttributes;
    res.ftCreationTime = narrow.ftCreationTime;
    res.ftLastAccessTime = narrow.ftLastAccessTime;
    res.ftLastWriteTime = narrow.ftLastWriteTime;
    res.nFileSizeHigh = narrow.nFileSizeHigh;
    res.nFileSizeLow = narrow.nFileSizeLow;
    res.dwReserved0 = narrow.dwReserved0;
    res.dwReserved1 = narrow.dwReserved1;

    stl::wstring w_name = a2w(narrow.cFileName);
    if (w_name.size() >= MAX_PATH)
      w_name.resize(MAX_PATH - 1);
    wmemcpy(res.cFileName, w_name.data(), w_name.size());
    res.cFileName[w_name.size()] = '\0';

    static_assert(sizeof(narrow.cAlternateFileName) == 14);
    stl::wstring w_alt_name = a2w(narrow.cAlternateFileName);
    if (w_alt_name.size() >= 14)
      w_alt_name.resize(14 - 1);
    wmemcpy(res.cAlternateFileName, w_alt_name.data(), w_alt_name.size());
    res.cAlternateFileName[w_alt_name.size()] = '\0';

    return res;
  }

  inline WIN32_FIND_DATAA w2u(const WIN32_FIND_DATAW &wide)
  {
    WIN32_FIND_DATAA res{};

    res.dwFileAttributes = wide.dwFileAttributes;
    res.ftCreationTime = wide.ftCreationTime;
    res.ftLastAccessTime = wide.ftLastAccessTime;
    res.ftLastWriteTime = wide.ftLastWriteTime;
    res.nFileSizeHigh = wide.nFileSizeHigh;
    res.nFileSizeLow = wide.nFileSizeLow;
    res.dwReserved0 = wide.dwReserved0;
    res.dwReserved1 = wide.dwReserved1;

    stl::string a_name = w2u(wide.cFileName);
    if (a_name.size() >= MAX_PATH)
      a_name.resize(MAX_PATH - 1);
    memcpy(res.cFileName, a_name.data(), a_name.size());
    res.cFileName[a_name.size()] = '\0';

    static_assert(sizeof(res.cAlternateFileName) == 14);
    stl::string a_alt_name = w2u(wide.cAlternateFileName);
    if (a_alt_name.size() >= 14)
      a_alt_name.resize(14 - 1);
    memcpy(res.cAlternateFileName, a_alt_name.data(), a_alt_name.size());
    res.cAlternateFileName[a_alt_name.size()] = '\0';

    return res;
  }
} // namespace mingw_thunk::internal
