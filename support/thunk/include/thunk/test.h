#pragma once

#ifdef ENABLE_TEST_OVERRIDE

#include <catch_amalgamated.hpp>

namespace mingw_thunk::internal
{
  inline bool touched;
}

#define CLEAR_TOUCH_FLAG() mingw_thunk::internal::touched = false

#define REQUIRE_TOUCHED() REQUIRE(mingw_thunk::internal::touched)

#endif
