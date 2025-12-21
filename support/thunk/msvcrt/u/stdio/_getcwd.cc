#include <thunk/_common.h>
#include <thunk/string.h>

#include <direct.h>
#include <errno.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt, 0, char *, __cdecl, _getcwd, char *buf, int size)
  {
    wchar_t *w_res = _wgetcwd(nullptr, 0);

    if (w_res == nullptr)
      return nullptr;

    stl::string res = internal::w2u(w_res);
    free(w_res);

    if (buf && res.size() >= size) {
      _set_errno(ERANGE);
      return nullptr;
    }

    if (!buf) {
      if (res.size() >= size)
        size = res.size() + 1;
      buf = (char *)malloc(size);
    }

    memcpy(buf, res.c_str(), res.size());
    buf[res.size()] = 0;
    return buf;
  }
} // namespace mingw_thunk
