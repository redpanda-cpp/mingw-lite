#pragma once

#include <stdbool.h>
#include <windef.h>

void change_to_self_dir();
void error_exit(const char *msg);
bool is_nt();
bool lt_win98();
void mkdir_p(const char *dir);
void prepend_to_env_path(const char *path);
void resolve_mingw_bin_dir(char dir[MAX_PATH]);
int resolve_self_dir(char dir[MAX_PATH]);
HANDLE spawn(const char *argv[]);
int wait(HANDLE process);
