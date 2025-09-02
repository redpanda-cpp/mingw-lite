#include <thunk/test.h>

#include <errno.h>
#include <wchar.h>

TEST_CASE("_wputenv_s")
{
  const auto &name = L"TEST_ENVIRONMENT";
  const auto &value = L"TEST_VALUE";

  SECTION("normal case")
  {
    CLEAR_TOUCH_FLAG();
    errno_t err = _wputenv_s(name, value);
    REQUIRE_TOUCHED();

    REQUIRE(err == 0);
  }

  SECTION("error: null name")
  {
    errno_t err = _wputenv_s(nullptr, value);

    REQUIRE(err == EINVAL);
  }

  SECTION("error: null value")
  {
    errno_t err = _wputenv_s(name, nullptr);

    REQUIRE(err == EINVAL);
  }
}
