#pragma once

#include <sys/stat.h>
#include <sys/types.h>

// old version of mingw-w64 redirect `_stat32i64` to `_stati64`,
// also make `struct _stat32i64` actually named `struct _stati64`.
#ifdef _stat32i64
#define MINGW_THUNK_WORKAROUND_DEFINED_STAT32I64
#endif

#undef _fstat
#undef _fstat32
#undef _fstat32i64
#undef _fstat64
#undef _fstat64i32
#undef _fstati64
#undef fstat
#undef fstat64

#undef _stat
#undef _stat32
#undef _stat32i64
#undef _stat64
#undef _stat64i32
#undef _stati64
#undef stat
#undef stat64

#undef _wstat
#undef _wstat32
#undef _wstat32i64
#undef _wstat64
#undef _wstat64i32
#undef _wstati64
#undef wstat
#undef wstat64

struct _stat32;
struct _stat64;
struct _stat64i32;

#ifdef MINGW_THUNK_WORKAROUND_DEFINED_STAT32I64
#undef MINGW_THUNK_WORKAROUND_DEFINED_STAT32I64
struct _stat32i64 : _stati64 {};
#else
struct _stat32i64;
#endif

extern "C"
{
  __attribute__((__dllimport__)) int _fstat32(int fd, struct _stat32 *buffer);
  __attribute__((__dllimport__)) int _fstat32i64(int fd,
                                                 struct _stat32i64 *buffer);
  __attribute__((__dllimport__)) int _fstat64(int fd, struct _stat64 *buffer);
  __attribute__((__dllimport__)) int _fstat64i32(int fd,
                                                 struct _stat64i32 *buffer);

  __attribute__((__dllimport__)) int _stat32(const char *path,
                                             struct _stat32 *buffer);
  __attribute__((__dllimport__)) int _stat32i64(const char *path,
                                                struct _stat32i64 *buffer);
  __attribute__((__dllimport__)) int _stat64(const char *path,
                                             struct _stat64 *buffer);
  __attribute__((__dllimport__)) int _stat64i32(const char *path,
                                                struct _stat64i32 *buffer);

  __attribute__((__dllimport__)) int _wstat32(const wchar_t *path,
                                              struct _stat32 *buffer);
  __attribute__((__dllimport__)) int _wstat32i64(const wchar_t *path,
                                                 struct _stat32i64 *buffer);
  __attribute__((__dllimport__)) int _wstat64(const wchar_t *path,
                                              struct _stat64 *buffer);
  __attribute__((__dllimport__)) int _wstat64i32(const wchar_t *path,
                                                 struct _stat64i32 *buffer);
}
