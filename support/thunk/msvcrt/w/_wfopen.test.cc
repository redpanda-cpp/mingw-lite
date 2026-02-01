#include "_wfopen.h"

#include <catch_amalgamated.hpp>

#include <stdio.h>

TEST_CASE("_wfopen")
{
  const wchar_t *filename = L"test-_wfopen.txt";

  FILE *fp = mingw_thunk::impl::win9x__wfopen(filename, L"w");
  REQUIRE(fp);

  fclose(fp);
}
