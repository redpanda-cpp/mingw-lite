#include <thunk/_common.h>
#include <thunk/string.h>

#include <fcntl.h>
#include <io.h>
#include <stdarg.h>

namespace mingw_thunk
{
  namespace internal
  {
    int _open(const char *path, int flags);
    int _open(const char *path, int flags, int mode);
  } // namespace internal

  __DEFINE_THUNK(
      msvcrt, 0, int, __cdecl, _open, const char *path, int flags, ...)
  {
    if (flags & _O_CREAT) {
      va_list args;
      va_start(args, flags);
      int res = internal::_open(path, flags, va_arg(args, int));
      va_end(args);
      return res;
    } else {
      return internal::_open(path, flags);
    }
  }

  namespace internal
  {
    int _open(const char *path, int flags)
    {
      stl::wstring w_path = internal::u2w(path);
      return _wopen(w_path.c_str(), flags);
    }

    int _open(const char *path, int flags, int mode)
    {
      stl::wstring w_path = internal::u2w(path);
      return _wopen(w_path.c_str(), flags, mode);
    }
  } // namespace internal
} // namespace mingw_thunk
