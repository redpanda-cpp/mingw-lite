#pragma once

#ifndef NS_NOSTL
  #define NS_NOSTL nostl
#endif

#ifdef NOSTL_NOCRT
  #ifndef _WIN32
    #error "Building without CRT is only supported on Windows"
  #endif
  #define NS_NOSTL_CRT NS_NOCRT
#else
  #define NS_NOSTL_CRT
#endif

#ifndef NOSTL_RAISE_BAD_ALLOC
  #ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
      #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOMINMAX
      #define NOMINMAX
    #endif
    #include <windows.h>
    #define NOSTL_RAISE_BAD_ALLOC()                                            \
      RaiseException(STATUS_NO_MEMORY, 0, 0, nullptr)
  #else
    #include <signal.h>
    #define NOSTL_RAISE_BAD_ALLOC() raise(SIGSEGV)
  #endif
#endif

#ifdef __cpp_if_consteval
  #define if_consteval if consteval
  #define if_not_consteval if !consteval
#elif __has_builtin(__builtin_is_constant_evaluated)
  #define if_consteval if (__builtin_is_constant_evaluated())
  #define if_not_consteval if (!__builtin_is_constant_evaluated())
#else
  #define if_consteval if (false)
  #define if_not_consteval if (true)
#endif
