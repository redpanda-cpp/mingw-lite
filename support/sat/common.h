#pragma once

#include <stdbool.h>
#include <windef.h>

void change_to_self_dir();
void clean_shared_libs();
void error_exit(const char *msg);
void install_shared_libs();
bool is_nt();
bool lt_win98();
void mkdir_p(const char *dir);
void prepend_to_env_path(const char *path);
int resolve_mingw_bin_dir(char dir[MAX_PATH]);
int resolve_mingw_lib_shared_dir(char dir[MAX_PATH]);
int resolve_mingw_root_dir(char dir[MAX_PATH]);
int resolve_self_dir(char dir[MAX_PATH]);
HANDLE spawn(const char *argv[]);
int wait(HANDLE process);
