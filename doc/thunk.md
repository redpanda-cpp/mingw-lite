# MinGW Thunk

Inspired by [YY-Thunks](https://github.com/Chuyu-Team/YY-Thunks), MinGW Lite uses thunks for legacy OS support and special purposes (e.g. u8crt).

A thunk is small piece of code that wrap the original Win32 or CRT function, providing alternative implementation when the function is not available on the target OS (legacy OS support), or completely overwriting original function (special purposes).

## Use

Thunks are built into import libraries. To use thunks, no extra operation is required.

To revert thunks (use Microsoft’s original version), use `__ms_`-prefixed ones. Type deduction is recommended to declare them for correct calling conventions. In C,

```c
typeof(CopyFileW) __ms_CopyFileW;

__ms_CopyFileW(L"old.txt", L"new.txt", FALSE);
```

And in C++,

```c++
extern "C"
{
  decltype(CopyFileW) __ms_CopyFileW;
}

__ms_CopyFileW(L"old.txt", L"new.txt", FALSE);
```

The complete list of thunks is available at `share/doc/crt/thunk-revert-map.json`.

## Technical Details

Here are pseudocode how thunks are implemented.

### Legacy OS Missing Function

Dynamically load original function, and use fallback function when failed.

```c++
inline HMODULE module_kernel32 = LoadLibraryA("kernel32.dll");

// dynamically load original function
decltype(&GetTickCount64) try_get_GetTickCount64()
{
  static auto *pfn = (decltype(&GetTickCount64))
    GetProcAddress(module_kernel32, "GetTickCount64");
  return pfn;
}

// fallback function
decltype(GetTickCount64) fallback_GetTickCount64;

// overwrite `__imp_` symbol
decltype(&GetTickCount64) dllimport_GetTickCount64
  __asm__("__imp_GetTickCount64") = &GetTickCount64;

// overwrite original symbol
extern "C"
ULONGLONG WINAPI GetTickCount64()
{
  if (dllimport_GetTickCount64 == &GetTickCount64) [[unlikely]] {
    // first call: detect and choose which version to use
    if (const auto pfn = try_get_GetTickCount64())
      dllimport_GetTickCount64 = pfn;
    else
      dllimport_GetTickCount64 = &fallback_GetTickCount64;
  }

  return dllimport_GetTickCount64();
}

ULONGLONG WINAPI fallback_GetTickCount64()
{
  return GetTickCount();
}
```

### Windows 9x Stub Function

Check system version, and use original function directly, or fallback function.

```c++
// rename original function
// an alias is declared in the `.def` file
//   __ms_CopyFileW@12 = CopyFileW
extern "C" decltype(CopyFileW) __ms_CopyFileW;

decltype(CopyFileW) win9x_CopyFileW;

decltype(&CopyFileW) dllimport_CopyFileW
  __asm__("__imp__CopyFileW@12") = &CopyFileW;

extern "C"
BOOL WINAPI CopyFileW(LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName, BOOL bFailIfExists)
{
  if (dllimport_CopyFileW == &CopyFileW) [[unlikely]] {
    if (current_os_is_nt_family())
      dllimport_CopyFileW = &__ms_CopyFileW;
    else
      dllimport_CopyFileW = &win9x_CopyFileW;
  }

  return dllimport_CopyFileW(lpExistingFileName, lpNewFileName, bFailIfExists);
}

BOOL WINAPI win9x_CopyFileW(LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName, BOOL bFailIfExists)
{
  string a_existing = wide_to_ansi(lpExistingFileName);
  string a_new = wide_to_ansi(lpNewFileName);
  return CopyFileA(a_existing.c_str(), a_new.c_str(), bFailIfExists);
}
```

### Completely Overwrite Original Function

```c++
decltype(&CopyFileA) dllimport_CopyFileA
  __asm__("__imp_CopyFileA") = &CopyFileA;

extern "C"
BOOL WINAPI CopyFileA(LPCSTR lpExistingFileName, LPCSTR lpNewFileName, BOOL bFailIfExists)
{
  wstring w_existing = utf8_to_wide(lpExistingFileName);
  wstring w_new = utf8_to_wide(lpNewFileName);
  return CopyFileW(w_existing.c_str(), w_new.c_str(), bFailIfExists);
}
```
