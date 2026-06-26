#include "__p__environ.h"

#include <thunk/_common.h>
#include <thunk/string.h>

#include <stdlib.h>
#include <string.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt, 0, char ***, __cdecl, __p__environ)
  {
    while (!__atomic_test_and_set(&internal::u8_environ_lock, __ATOMIC_ACQUIRE))
      Sleep(0);

    if (internal::u8_environ == nullptr) {
      internal::u8_environ = (char **)malloc(sizeof(char *));
      internal::u8_environ[0] = nullptr;
    };

    __atomic_clear(&internal::u8_environ_lock, __ATOMIC_RELEASE);
    return &internal::u8_environ;
  }

  namespace internal
  {
    char **u8_dup_envp(char **envp, size_t size)
    {
      char **new_environ = (char **)malloc(sizeof(char *) * (size + 1));
      for (size_t i = 0; i < size; ++i)
        new_environ[i] = _strdup(envp[i]);
      new_environ[size] = nullptr;
      return new_environ;
    }

    void u8_environ_assign(const d::u_str &key, const d::u_str &value)
    {
      const char *name = key.c_str();
      size_t name_len = key.size();
      size_t value_len = value.size();

      while (!__atomic_test_and_set(&u8_environ_lock, __ATOMIC_ACQUIRE))
        Sleep(0);

      if (u8_environ == u8_envp)
        u8_environ = u8_dup_envp(u8_environ, u8_environ_size);

      if (value_len == 0) {
        // remove
        for (int i = 0; i < (int)u8_environ_size; i++) {
          if (_strnicmp(u8_environ[i], name, name_len) == 0 &&
              u8_environ[i][name_len] == '=') {
            free(u8_environ[i]);
            for (int j = i; j < (int)u8_environ_size - 1; j++)
              u8_environ[j] = u8_environ[j + 1];
            u8_environ[u8_environ_size - 1] = nullptr;
            u8_environ_size--;
            break;
          }
        }
      } else {
        char *new_entry = (char *)malloc(name_len + 1 + value_len + 1);
        memcpy(new_entry, name, name_len);
        new_entry[name_len] = '=';
        memcpy(new_entry + name_len + 1, value.c_str(), value_len);
        new_entry[name_len + 1 + value_len] = 0;

        // update?
        int i = 0;
        for (; i < (int)u8_environ_size; i++) {
          if (_strnicmp(u8_environ[i], name, name_len) == 0 &&
              u8_environ[i][name_len] == '=') {
            free(u8_environ[i]);
            u8_environ[i] = new_entry;
            break;
          }
        }

        // add
        if (i == (int)u8_environ_size) {
          char **new_envp = (char **)realloc(
              u8_environ, sizeof(char *) * (u8_environ_size + 2));
          new_envp[u8_environ_size] = new_entry;
          new_envp[u8_environ_size + 1] = nullptr;
          u8_environ = new_envp;
          u8_environ_size++;
        }
      }

      __atomic_clear(&u8_environ_lock, __ATOMIC_RELEASE);
    }

    char **u8_envp = nullptr;

    char **u8_environ = nullptr;
    size_t u8_environ_size = 0;
    bool u8_environ_lock = false;
  } // namespace internal

  __DECLARE_FORCE_OVERRIDE_MINGW_EMU(__p__environ)
} // namespace mingw_thunk
