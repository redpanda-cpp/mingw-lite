#pragma once

#include <thunk/test.h>

#include <libloaderapi.h>

namespace mingw_thunk
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
} // namespace mingw_thunk

#ifdef ENABLE_TEST_OVERRIDE

#define __DECLARE_TRY_GET_FUNCTION(function)                                   \
  inline fn_##function##_t *try_get_##function() noexcept                      \
  {                                                                            \
    internal::touched = true;                                                  \
    return nullptr;                                                            \
  }

#else // ifdef ENABLE_TEST_OVERRIDE

#define __DECLARE_TRY_GET_FUNCTION(function)                                   \
  inline auto *try_get_##function() noexcept                                   \
  {                                                                            \
    return get_##function();                                                   \
  }

#endif // ifdef ENABLE_TEST_OVERRIDE

#ifdef _WIN64

#define __DLLIMPORT_SYMBOL_NAME___cdecl(function, size) "__imp_" #function
#define __DLLIMPORT_SYMBOL_NAME___stdcall(function, size) "__imp_" #function

#else // ifdef _WIN64

#define __DLLIMPORT_SYMBOL_NAME___cdecl(function, size) "__imp__" #function
#define __DLLIMPORT_SYMBOL_NAME___stdcall(function, size)                      \
  "__imp__" #function "@" #size

#endif // ifdef _WIN64

#define __DLLIMPORT_SYMBOL_NAME_APIENTRY(function, size)                       \
  __DLLIMPORT_SYMBOL_NAME___stdcall(function, size)
#define __DLLIMPORT_SYMBOL_NAME_WINAPI(function, size)                         \
  __DLLIMPORT_SYMBOL_NAME___stdcall(function, size)
#define __DLLIMPORT_SYMBOL_NAME_WSAAPI(function, size)                         \
  __DLLIMPORT_SYMBOL_NAME___stdcall(function, size)

#define __DEFINE_THUNK(                                                        \
    module, size, return_type, calling_convention, function, ...)              \
                                                                               \
  using fn_##function##_t = return_type calling_convention(__VA_ARGS__);       \
                                                                               \
  inline const auto &get_##module() noexcept                                   \
  {                                                                            \
    static module_handle handle{#module};                                      \
    return handle;                                                             \
  }                                                                            \
                                                                               \
  inline auto *get_##function() noexcept                                       \
  {                                                                            \
    static auto &module = get_##module();                                      \
    static auto *pfn = module.get_function<fn_##function##_t>(#function);      \
    return pfn;                                                                \
  }                                                                            \
                                                                               \
  __DECLARE_TRY_GET_FUNCTION(function)                                         \
                                                                               \
  extern "C" fn_##function##_t *dllimport_##function __asm__(                  \
      __DLLIMPORT_SYMBOL_NAME_##calling_convention(function, size)) =          \
      function;                                                                \
                                                                               \
  extern "C" return_type calling_convention function(__VA_ARGS__)
