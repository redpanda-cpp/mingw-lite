#include <thunk/_common.h>
#include <thunk/string.h>

#include <errno.h>
#include <stdio.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(
      msvcrt, 0, FILE *, __cdecl, fopen, const char *path, const char *mode)
  {
    if (!path || !mode) {
      _set_errno(EINVAL);
      return nullptr;
    }

    d::w_str w_path;
    if (!w_path.from_u(path)) {
      _set_errno(ENOMEM);
      return nullptr;
    }

    d::w_str w_mode;
    while (*mode) {
      switch (*mode) {
      case 'r':
      case 'w':
      case 'a':
      case '+':
      case 't':
      case 'b':
      case 'x':
      case 'c':
      case 'n':
      case 'N':
      case 'S':
      case 'R':
      case 'T':
      case 'D':
        if (!w_mode.push_back(*mode)) {
          _set_errno(ENOMEM);
          return nullptr;
        }
        break;
      case ',':
        // ,ccs=UTF-8
        goto stop;
      }
      ++mode;
    }
  stop:
    if (!w_mode.push_back(L'b')) {
      _set_errno(ENOMEM);
      return nullptr;
    }
    return _wfopen(w_path.c_str(), w_mode.c_str());
  }
} // namespace mingw_thunk
