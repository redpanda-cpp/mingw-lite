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
    d::w_str::best_effort_from_a(res.cFileName, MAX_PATH, narrow.cFileName);

    static_assert(sizeof(narrow.cAlternateFileName) == 14);
    d::w_str::best_effort_from_a(
        res.cAlternateFileName, 14, narrow.cAlternateFileName);

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
    d::u_str::best_effort_from_w(res.cFileName, MAX_PATH, wide.cFileName);

    static_assert(sizeof(res.cAlternateFileName) == 14);
    d::u_str::best_effort_from_w(
        res.cAlternateFileName, 14, wide.cAlternateFileName);

    return res;
  }
} // namespace mingw_thunk::internal
