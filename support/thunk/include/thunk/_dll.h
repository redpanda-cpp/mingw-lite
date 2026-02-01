#pragma once

#include <libloaderapi.h>

namespace mingw_thunk::internal
{
  struct module_handle
  {
    HMODULE module;

    module_handle(const char *module_name) noexcept
    {
      module = LoadLibraryA(module_name);
    }

    module_handle(const module_handle &) = delete;

    ~module_handle() noexcept
    {
      if (module)
        FreeLibrary(module);
    }

    template <typename Fn>
    Fn *get_function(const char *function_name) const noexcept
    {
      return module
                 ? reinterpret_cast<Fn *>(GetProcAddress(module, function_name))
                 : nullptr;
    }
  };

#define __DECLARE_SIMPLE_EXPLICIT_DLL_MODULE(name)                             \
  inline const module_handle &module_##name()                                  \
  {                                                                            \
    static module_handle module{#name ".dll"};                                 \
    return module;                                                             \
  }

  __DECLARE_SIMPLE_EXPLICIT_DLL_MODULE(advapi32)
  __DECLARE_SIMPLE_EXPLICIT_DLL_MODULE(bcrypt)
  __DECLARE_SIMPLE_EXPLICIT_DLL_MODULE(kernel32)
  __DECLARE_SIMPLE_EXPLICIT_DLL_MODULE(msvcrt)
  __DECLARE_SIMPLE_EXPLICIT_DLL_MODULE(ntdll)
  __DECLARE_SIMPLE_EXPLICIT_DLL_MODULE(shell32)
  __DECLARE_SIMPLE_EXPLICIT_DLL_MODULE(ws2_32)

#undef __DECLARE_SIMPLE_EXPLICIT_DLL_MODULE

#define __DECLARE_CANONICAL_EXPLICIT_DLL_MODULE(identifier, filename)          \
  inline const module_handle &module_##identifier()                            \
  {                                                                            \
    static module_handle module{filename};                                     \
    return module;                                                             \
  }

  __DECLARE_CANONICAL_EXPLICIT_DLL_MODULE(api_ms_win_core_path_l1_1_0,
                                          "api-ms-win-core-path-l1-1-0.dll")
  __DECLARE_CANONICAL_EXPLICIT_DLL_MODULE(
      api_ms_win_crt_filesystem_l1_1_0, "api-ms-win-crt-filesystem-l1-1-0.dll")
  __DECLARE_CANONICAL_EXPLICIT_DLL_MODULE(api_ms_win_crt_stdio_l1_1_0,
                                          "api-ms-win-crt-stdio-l1-1-0.dll")

#undef __DECLARE_CANONICAL_EXPLICIT_DLL_MODULE

} // namespace mingw_thunk::internal
