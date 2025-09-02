#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <fcntl.h>
#include <io.h>
#include <stdarg.h>
#include <wchar.h>

namespace mingw_thunk
{
  namespace internal
  {
    int _wopen(const wchar_t *filename, int oflag);
    int _wopen(const wchar_t *filename, int oflag, int pmode);
  } // namespace internal

  __DEFINE_CRT_THUNK(int, _wopen, const wchar_t *filename, int oflag, ...)
  {
    if (oflag & _O_CREAT) {
      va_list args;
      va_start(args, oflag);
      int res = internal::_wopen(filename, oflag, va_arg(args, int));
      va_end(args);
      return res;
    } else {
      return internal::_wopen(filename, oflag);
    }
  }

  namespace internal
  {
    int _wopen(const wchar_t *filename, int oflag)
    {
      if (is_nt())
        return get__wopen()(filename, oflag);

      stl::string a_name = narrow(filename);
      return _open(a_name.c_str(), oflag);
    }

    int _wopen(const wchar_t *filename, int oflag, int pmode)
    {
      if (is_nt())
        return get__wopen()(filename, oflag, pmode);

      stl::string a_name = narrow(filename);
      return _open(a_name.c_str(), oflag, pmode);
    }
  } // namespace internal
} // namespace mingw_thunk
