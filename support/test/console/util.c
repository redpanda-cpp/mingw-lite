#include "util.h"

#include <windows.h>

void emu_input(const wchar_t *s) {
  HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
  size_t len = wcslen(s);
  INPUT_RECORD records[len * 2];
  for (size_t i = 0; i < len; i++) {
    BOOL is_enter = (s[i] == L'\n');
    records[i * 2].EventType = KEY_EVENT;
    records[i * 2].Event.KeyEvent.bKeyDown = TRUE;
    records[i * 2].Event.KeyEvent.wRepeatCount = 1;
    records[i * 2].Event.KeyEvent.wVirtualKeyCode = is_enter ? VK_RETURN : 0;
    records[i * 2].Event.KeyEvent.wVirtualScanCode = is_enter ? 28 : 0;
    records[i * 2].Event.KeyEvent.uChar.UnicodeChar = is_enter ? L'\r' : s[i];
    records[i * 2].Event.KeyEvent.dwControlKeyState = 0;
    records[i * 2 + 1] = records[i * 2];
    records[i * 2 + 1].Event.KeyEvent.bKeyDown = FALSE;
  }
  DWORD written;
  WriteConsoleInputW(hStdin, records, len * 2, &written);
}

void write_output(const wchar_t *s) {
  HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
  size_t len = wcslen(s);
  DWORD written;
  WriteConsoleW(hStdout, L"[note] ", 7, &written, NULL);
  WriteConsoleW(hStdout, s, len, &written, NULL);
  WriteConsoleW(hStdout, L"\n", 1, &written, NULL);
}
