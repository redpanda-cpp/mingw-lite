#pragma once

#include <sys/stat.h>
#include <sys/types.h>

#undef _stat32
#undef _stat32i64
#undef _stat64
#undef _stat64i32

struct _stat32;
struct _stat32i64;
struct _stat64;
struct _stat64i32;

extern "C"
{
  int _stat32(const char *path, struct _stat32 *buffer);
  int _stat32i64(const char *path, struct _stat32i64 *buffer);
  int _stat64(const char *path, struct _stat64 *buffer);
  int _stat64i32(const char *path, struct _stat64i32 *buffer);
}

#undef _wstat32
#undef _wstat32i64
#undef _wstat64
#undef _wstat64i32

extern "C"
{
  int _wstat32(const wchar_t *path, struct _stat32 *buffer);
  int _wstat32i64(const wchar_t *path, struct _stat32i64 *buffer);
  int _wstat64(const wchar_t *path, struct _stat64 *buffer);
  int _wstat64i32(const wchar_t *path, struct _stat64i32 *buffer);
}

#undef _stat
#undef _stati64
