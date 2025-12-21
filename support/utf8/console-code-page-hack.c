/* MinGW Lite console code page hack.

   This file has no copyright assigned and is placed in the Public Domain.

   Automatically set console code page to match ACP (especially UTF-8).
 */

#include <stdlib.h>
#include <windows.h>

static UINT saved_console_cp = 0;
static UINT saved_console_output_cp = 0;

static void __cdecl restore_console_cp(void) {
  if (saved_console_cp)
    SetConsoleCP(saved_console_cp);
  if (saved_console_output_cp)
    SetConsoleOutputCP(saved_console_output_cp);
}

static void __cdecl console_code_page_hack(void) {
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
}

__auto_type __mingw_lite_console_code_page_hack
    __attribute__((section(".CRT$XCML"), used)) = &console_code_page_hack;
