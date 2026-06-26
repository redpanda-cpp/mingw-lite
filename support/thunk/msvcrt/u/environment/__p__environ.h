#pragma once

#include <stddef.h>

namespace mingw_thunk
{
  namespace d
  {
    class u_str;
  }

  namespace internal
  {
    extern char **u8_dup_envp(char **envp, size_t size);

    extern void u8_environ_assign(const d::u_str &key, const d::u_str &value);

    extern char **u8_envp;

    extern char **u8_environ;
    extern size_t u8_environ_size;
    extern bool u8_environ_lock;
  } // namespace internal
} // namespace mingw_thunk
