#pragma once

namespace mingw_thunk
{
  namespace internal
  {
    struct u8_envp_result
    {
      int envc;
      char **envp;
    };

    u8_envp_result u8_envp_from_wenvp(wchar_t **wenvp);
    u8_envp_result u8_envp_from_win32_env_strings();
  } // namespace internal
} // namespace mingw_thunk
