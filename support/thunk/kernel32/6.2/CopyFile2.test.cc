#include "CopyFile2.h"

#include <catch_amalgamated.hpp>

#include <stdio.h>

#include <windows.h>
#include <winerror.h>

TEST_CASE("CopyFile2")
{
  const wchar_t *source = L"test-CopyFile2.txt";

  SECTION("create test file")
  {
    FILE *fp = _wfopen(source, L"w");
    fclose(fp);
  }

  const wchar_t *dest = L"test-CopyFile2-copy.txt";

  COPYFILE2_EXTENDED_PARAMETERS params{sizeof(params)};

  HRESULT hr = mingw_thunk::impl::winnt6_CopyFile2(source, dest, &params);

  REQUIRE(SUCCEEDED(hr));
}
