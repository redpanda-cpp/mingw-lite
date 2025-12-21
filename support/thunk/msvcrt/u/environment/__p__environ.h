#pragma once

#include <stddef.h>

namespace mingw_thunk
{
  namespace internal
  {
    extern char **u8_dup_envp(char **envp, size_t size);

    extern char **u8_envp;

    extern char **u8_environ;
    extern size_t u8_environ_size;
    extern bool u8_environ_lock;
  } // namespace internal
} // namespace mingw_thunk
