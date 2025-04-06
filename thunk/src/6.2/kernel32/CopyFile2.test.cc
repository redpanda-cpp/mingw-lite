#include <thunk/test.h>

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

  RESET_TOUCH_FLAG();
  HRESULT hr = CopyFile2(source, dest, &params);
  REQUIRE_TOUCHED();

  REQUIRE(SUCCEEDED(hr));
}
