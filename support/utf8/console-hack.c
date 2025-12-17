/* MinGW Lite console hacks.

   This file has no copyright assigned and is placed in the Public Domain.

   1. Automatically set console code page to match ACP (especially UTF-8).
   2. Set stdio to binary mode to workaround CRT's bugged "double translation".
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
#include <windows.h>

static UINT saved_console_cp = 0;
static UINT saved_console_output_cp = 0;

static void __cdecl restore_console_cp(void)
{
  if (saved_console_cp)
    SetConsoleCP(saved_console_cp);
  if (saved_console_output_cp)
    SetConsoleOutputCP(saved_console_output_cp);
}

static void __cdecl console_hack(void)
{
  UINT acp = GetACP();
  UINT console_cp = GetConsoleCP();
  UINT console_output_cp = GetConsoleOutputCP();
  if (acp != console_cp) {
    saved_console_cp = console_cp;
    SetConsoleCP(acp);
  }
  if (acp != console_output_cp) {
    saved_console_output_cp = console_output_cp;
    SetConsoleOutputCP(acp);
  }
  atexit(restore_console_cp);

  _setmode(_fileno(stdin), _O_BINARY);
  _setmode(_fileno(stdout), _O_BINARY);
  _setmode(_fileno(stderr), _O_BINARY);
}

__auto_type __mingw_lite_console_hack
  __attribute__ ((section (".CRT$XCML"), used))
  = &console_hack;
