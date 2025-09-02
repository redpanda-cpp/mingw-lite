#include <thunk/test.h>

#include <errno.h>
#include <stdio.h>

TEST_CASE("_wfopen")
{
  const wchar_t* filename = L"test-_wfopen.txt";

  CLEAR_TOUCH_FLAG();
  FILE *fp = _wfopen(filename, L"w");
  REQUIRE_TOUCHED();

  REQUIRE(fp);

  fclose(fp);
}
