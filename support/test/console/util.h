#pragma once

#include <wchar.h>

#ifdef __cplusplus
extern "C" {
#endif

void emu_input(const wchar_t *s);
void write_output(const wchar_t *s);

#ifdef __cplusplus
}
#endif
