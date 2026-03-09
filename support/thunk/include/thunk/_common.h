#pragma once

#include "_dll.h"

#define __DECLARE_TRY_GET_FUNCTION(function)                                   \
  inline auto *try_get_##function() noexcept                                   \
  {                                                                            \
    return get_##function();                                                   \
  }

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
  extern "C" return_type calling_convention function(__VA_ARGS__);             \
                                                                               \
  using fn_##function##_t = decltype(function);                                \
                                                                               \
  namespace                                                                    \
  {                                                                            \
    inline auto *get_##function() noexcept                                     \
    {                                                                          \
      static auto *pfn =                                                       \
          internal::module_##module().get_function<fn_##function##_t>(         \
              #function);                                                      \
      return pfn;                                                              \
    }                                                                          \
                                                                               \
    __DECLARE_TRY_GET_FUNCTION(function)                                       \
  }                                                                            \
                                                                               \
  fn_##function##_t *dllimport_##function __asm__(                             \
      __DLLIMPORT_SYMBOL_NAME_##calling_convention(function, size)) =          \
      function;                                                                \
                                                                               \
  extern "C" return_type calling_convention function(__VA_ARGS__)

#define __DISPATCH_THUNK_2(function, condition, target, fallback)              \
  if (__builtin_expect(dllimport_##function == function, 0)) {                 \
    if (condition) {                                                           \
      dllimport_##function = target;                                           \
    } else {                                                                   \
      dllimport_##function = fallback;                                         \
    }                                                                          \
  }

#define __DISPATCH_THUNK_3(                                                    \
    function, condition_1, target_1, condition_2, target_2, fallback)          \
  if (__builtin_expect(dllimport_##function == function, 0)) {                 \
    if (condition_1) {                                                         \
      dllimport_##function = target_1;                                         \
    } else if (condition_2) {                                                  \
      dllimport_##function = target_2;                                         \
    } else {                                                                   \
      dllimport_##function = fallback;                                         \
    }                                                                          \
  }

#define __DECLARE_FORCE_OVERRIDE_MINGW_EMU(function)                           \
  int force_override_mingw_emu_##fuction __asm__(                              \
      "__force_override_mingw_emu" __DLLIMPORT_SYMBOL_NAME___cdecl(function,   \
                                                                   0)) = 0;
