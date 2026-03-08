#include <thunk/_common.h>
#include <thunk/string.h>

#include <errno.h>
#include <stdlib.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(api_ms_win_crt_filesystem_l1_1_0,
                 0,
                 char *,
                 __cdecl,
                 _fullpath,
                 char *absPath,
                 const char *relPath,
                 size_t maxLength)
  {
    if (!relPath) {
      _set_errno(EINVAL);
      return nullptr;
    }

    d::w_str w_rel_path;
    if (!w_rel_path.from_u(relPath)) {
      _set_errno(ENOMEM);
      return nullptr;
    }

    wchar_t *w_res = _wfullpath(nullptr, w_rel_path.c_str(), 0);

    if (w_res == nullptr)
      return nullptr;

    d::u_str res;
    if (!res.from_w(w_res)) {
      free(w_res);
      _set_errno(ENOMEM);
      return nullptr;
    }

    free(w_res);

    if (absPath && res.size() >= maxLength) {
      _set_errno(ERANGE);
      return nullptr;
    }

    if (!absPath) {
      if (res.size() >= maxLength)
        maxLength = res.size() + 1;
      absPath = (char *)malloc(maxLength);
    }

    memcpy(absPath, res.c_str(), res.size());
    absPath[res.size()] = 0;
    return absPath;
  }
} // namespace mingw_thunk
