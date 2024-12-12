# MinGW Lite

Extremely fast MinGW GCC build scripts for easy experiments.

## Build

```bash
podman build -t mingw-lite/buildenv-ubuntu support/buildenv-ubuntu

podman run -it --rm -v $PWD:/mnt -w /mnt mingw-lite/buildenv-ubuntu

# in the container
./main.py -b <branch> -p <profile>
```

Available branches:

| Branch | GCC version | GCC adoption | MinGW | Binutils |
| ------ | ----------- | ------------ | ----- | -------- |
| 15 | 15-20241208 (exp) | | 12.0.0 | 2.43.1 |
| 14 | 14.2.0 | Debian 13, EL 10 | 12.0.0 | 2.43.1 |
| 13 | 13.3.0 | Ubuntu 24.04 | 11.0.1 | 2.41 |
| 12 | 12.4.0 | Debian 12 | 10.0.0 | 2.39 |
| 11 | 11.5.0 (EOL) | Ubuntu 22.04, EL 9 | 9.0.0 | 2.37 |
| 10 | 10.5.0 (EOL) | Debian 11 | 8.0.3 | 2.35.2 |
| 9 | 9.5.0 (EOL) | Ubuntu 20.04 | 7.0.0 | 2.33.1 |
| 8 | 8.5.0 (EOL) | Debian 10, EL 8 | 6.0.1 | 2.31.1 |
| 7 | 7.5.0 (EOL) | Ubuntu 18.04 | 5.0.5 | 2.29.1 |
| 6 | 6.5.0 (EOL) | Debian 9 | 5.0.5 | 2.27 |
| 5 | 5.5.0 (EOL) | Ubuntu 16.04 | 4.0.6 | 2.25.1 |
| 4.9 | 4.9.4 (EOL) | Debian 8 | 3.3.0 | 2.25.1 |
| 4.8 | 4.8.5 (EOL) | Ubuntu 14.04, EL 7 | 3.3.0 | 2.24 |

Available profiles:

| Profile | Arch | CRT | Threads | Exception | HOST | TARGET |
| ------- | ---- | --- | ------- | --------- | ---- | ------ |
| 64-mcf | x86_64 | ucrt | mcf | seh | 7+ | 7+ |
| 64-ucrt | x86_64 | ucrt | posix | seh | 7+ | 2003+ |
| 64-msvcrt | x86_64 | msvcrt | posix | seh | 2003+ | 2003+ |
| | | | | | | |
| 32-mcf | i686 | ucrt | mcf | dwarf | 7+ | 7+ |
| 32-ucrt | i686 | ucrt | posix | dwarf | 7+ | XP+ |
| 32-msvcrt | i686 | msvcrt | posix | dwarf | 2000+ | NT+ |
| | | | | | | |
| 32-legacy | i486 | msvcrt | posix | dwarf | NT+ | 9x, NT+ |

Notes:

- To expose build directories for debugging:
  ```bash
  mkdir -p build pkg

  podman run -it --rm \
    -v $PWD:/mnt -w /mnt \
    -v $PWD/build:/tmp/build \
    -v $PWD/pkg:/opt \
    mingw-lite/buildenv-ubuntu
  ```
- Version “freeze” happens at next 01-01 after GCC’s release.
- The “32-legacy” profile partially runs on Windows 9x host.
  - GDB does not work.
  - On Windows 95, where dlopen-ing dlls with `.tls` section is impossible, GCC should be called with `-fno-lto` to ensure `ld` would not load LTO plugin. ([KB118816](https://www.betaarchive.com/wiki/index.php/Microsoft_KB_Archive/118816))

## Patch List

| Family | Patch | Applies to | Description |
| ------ | ----- | ---------- | ----------- |
| Binutils | Fix path corruption | ≥ 2.39 | [niXman/mingw-builds#649](https://github.com/niXman/mingw-builds/issues/649) |
| Binutils | Ignore long path | (H) 9x | Do not resolve filename to UNC path on Windows 9x. |
| CRT | Fix missing function | 2003- | The thunk for `_wassert` is marked as x86 only. `_wassert` is not implemented in Windows Server 2003 R2 x64 either. |
| CRT | Allow skip space check in `ftruncate64` | NT- | The space check calls find volume APIs. |
| CRT | Use ANSI API | 9x | MinGW-w64 uses `GetModuleHandleW` to load `msvcrt.dll` in thunks. |
| winpthreads | Disable VEH | ≥ 5, 2000- | MinGW-w64’s winpthreads use VEH (vectored exception handler, since Windows XP) to set thread name. |
| winpthreads | Fix thread | 9x | Fix invalid args calling `_beginthreadex` on Windows 9x. |
| winpthreads | Kernel32 thunk | 9x | Thunks for critical section APIs and misc. |
| gettext | Kernel32 thunk | (H) 9x | Thunks for critical section APIs. |
| GCC | Fix VT sequence | ≥ 8 | Fix VT sequence confliction with UTF-8 prefix. |
| GCC | Fix locale directory | Any | [niXman/mingw-builds#666](https://github.com/niXman/mingw-builds/issues/666) |
| GCC | Parser-friendly diagnostics | Any | Keep the phrases “error:” and “warning:” unlocalized to make diagnostic messages parser-friendly. |
| GCC | Fix console code page | ≥ 13, (H) Vista+ | GCC 13 introduced UTF-8 manifest for i18n support, making GCC’s ACP differ from console code page. |
| GCC | Disable `_aligned_malloc` | 2000- | libstdc++ and gomp prefer it but provide fallbacks. |
| GCC | Fix libbacktrace | ≥ 15, (H) 2000- | 2000: missing library loader APIs. NT-: plus tool help APIs (actually not required on 9x). |
| GCC | libstdc++ Win32 thunk | NT- | NT: thunks for `CreateHardLinkW` and `_wstat64`. 9x: plus Win32 Unicode APIs. |
| GDB | Fix thread | (H) Vista- | Do not touch threads that are not suspended, which leads to crash on 2003/Vista x64. |
| GDB | Kernel32 thunk | ≥ 10, (H) 2000- | Thunks for `GetConsoleProcessList` and `GetSystemWow64DirectoryA`. |
| GDB | IPv6 thunk | ≥ 8.3, (H) 2000- | Thunks for `getaddrinfo`, `getnameinfo`, `freeaddrinfo`. |
| GDB | Fix VC6 CRT compatibility | (H) NT- | Disable time64 and largefile. |

Note:

- Patches to fix build errors are not listed.
- “(H)” for HOST.
- Internally, and in this patch list, releases of Windows are treated _totally ordered_ as follows:
  | Version | Product name | `_WIN32_WINNT` |
  | ------- | ------------ | -------------- |
  | NT 6.0 | Vista | 0x0600 |
  | NT 5.2 | 2003 | 0x0502 |
  | NT 5.1 | XP | 0x0501 |
  | NT 5.0 | 2000 | 0x0500 |
  | NT 4.0 | NT | 0x0400 |
  | 4.90, 4.10, 4.00 | 9x | 0x03FF |
