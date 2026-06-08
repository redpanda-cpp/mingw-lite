#include <string.h>

#include <windows.h>

int main()
{
  const char *s = "天地玄黄\n";
  size_t len = strlen(s);

  HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD written;
  WriteFile(hStdOut, s, len, &written, NULL);

  return !(len == written);
}
