/* MinGW Lite console mode hack.

   This file has no copyright assigned and is placed in the Public Domain.

   Set stdio to binary mode to workaround CRT's bugged "double translation".
   a. UCRT randomly eats linefeeds when piping on Windows Vista.
      Compiling any non-trivial program compiled with '-g3 -pipe'
      is likely to fail.
   b. MSVCRT, and UCRT prior to 10.0.14393, cannot write multi-byte
      characters one-char-by-one-char after set_locale(LC_ALL, "").
        int main()
        {
          setlocale(LC_ALL, "");
          for (auto ch : "你好")
            fputc(ch, stdout);
        }
      This is exactly how MinGW-w64's printf and toolchain's NLS work.
 */

#include <fcntl.h>
#include <io.h>
#include <stdio.h>

static void __cdecl console_mode_hack(void) {
  _setmode(_fileno(stdin), _O_BINARY);
  _setmode(_fileno(stdout), _O_BINARY);
  _setmode(_fileno(stderr), _O_BINARY);
}

__auto_type __mingw_lite_console_mode_hack
    __attribute__((section(".CRT$XCML"), used)) = &console_mode_hack;
