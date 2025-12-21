#include <thunk/_common.h>
#include <thunk/string.h>

#include <errno.h>
#include <stdlib.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt,
                 0,
                 char *,
                 __cdecl,
                 _fullpath,
                 char *absPath,
                 const char *relPath,
                 size_t maxLength)
  {
    stl::wstring w_relPath = internal::u2w(relPath);
    wchar_t *w_res = _wfullpath(nullptr, w_relPath.c_str(), 0);

    if (w_res == nullptr)
      return nullptr;

    stl::string res = internal::w2u(w_res);
    free(w_res);

    if (absPath && res.size() >= maxLength) {
      _set_errno(ERANGE);
      return nullptr;
    }

    if (!absPath)
      absPath = (char *)malloc(res.size() + 1);

    memcpy(absPath, res.c_str(), res.size());
    absPath[res.size()] = 0;
    return absPath;
  }
} // namespace mingw_thunk
