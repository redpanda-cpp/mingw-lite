#include "_wputenv_s.h"

#include <catch_amalgamated.hpp>

#include <errno.h>
#include <wchar.h>

TEST_CASE("_wputenv_s")
{
  const auto &name = L"TEST_ENVIRONMENT";
  const auto &value = L"TEST_VALUE";

  SECTION("normal case")
  {
    errno_t err = mingw_thunk::impl::fallback__wputenv_s(name, value);

    REQUIRE(err == 0);
  }

  SECTION("error: null name")
  {
    errno_t err = mingw_thunk::impl::fallback__wputenv_s(nullptr, value);

    REQUIRE(err == EINVAL);
  }

  SECTION("error: null value")
  {
    errno_t err = mingw_thunk::impl::fallback__wputenv_s(name, nullptr);

    REQUIRE(err == EINVAL);
  }
}
