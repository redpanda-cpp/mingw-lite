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

#if 0
  // usage:
  constexpr_if_consteval return_type op()
  {
    if_consteval
    {
      // slow, compile-time implementation
    }
    else
    {
      // fast, run-time implementation
    }
  }
#endif
#ifdef __cpp_if_consteval
  // okay, consteval is supported
  #define if_consteval if consteval
  #define constexpr_if_consteval constexpr
#elif defined(__has_builtin)
  // cannot use `defined(...) && __has_builtin(...)`
  // error: missing binary operator before token "("
  #if __has_builtin(__builtin_is_constant_evaluated)
    // okay, alternative syntax
    #define if_consteval if (__builtin_is_constant_evaluated())
    #define constexpr_if_consteval constexpr
  #else
    #define if_consteval if constexpr (true)
    #define constexpr_if_consteval constexpr
  #endif
#elif defined(__cpp_if_constexpr)
  // keep constexpr, but always use the slow path
  #define if_consteval if constexpr (true)
  #define constexpr_if_consteval constexpr
#else
  // constexpr is impossible, use fast path
  #define if_consteval if (false)
  #define constexpr_if_consteval
#endif
