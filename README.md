# MinGW Lite

Extremely fast MinGW GCC build scripts for easy experiments.

## Build

```bash
podman run -it --rm -v $PWD:/mnt -w /mnt docker.io/amd64/ubuntu:24.04

# in the container
./support/dep-debian.sh
./main.py -b <branch> -p <profile>
```

Available branches:

| Branch | GCC version | GCC status |
| ------- | ----------- | ---------- |
| 14 | 14.2.0 | latest |
| 15 | 15-20240804 | development |

Available profiles:

| Profile | Arch | CRT | Threads | Exception | HOST | TARGET |
| ------- | ---- | --- | ------- | --------- | ---- | ------ |
| 64-mcf | x86_64 | ucrt | mcf | seh | 7+ | 7+ |
| 64-ucrt | x86_64 | ucrt | posix | seh | 7+ | 2003+ |
| 64-msvcrt | x86_64 | msvcrt | posix | seh | 2003+ | 2003+ |
| | | | | | | |
| 32-mcf | i686 | ucrt | mcf | dwarf | 7+ | 7+ |
| 32-ucrt | i686 | ucrt | posix | dwarf | 7+ | XP+ |
| 32-msvcrt | i686 | msvcrt | posix | dwarf | 2000+ | 2000+ |
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
    docker.io/amd64/ubuntu:24.04
  ```
- The “32-legacy” profile partially runs on Windows 9x host.
  - GDB does not work.
  - On Windows 95, where dlopen-ing dlls with `.tls` section is impossible, GCC should be called with `-fno-lto` to ensure `ld` would not load LTO plugin. ([KB118816](https://www.betaarchive.com/wiki/index.php/Microsoft_KB_Archive/118816))

## Patch List

| Family | Patch | Applies to | Description |
| ------ | ----- | ---------- | ----------- |
| Binutils | Fix path corruption | Any | [niXman/mingw-builds#649](https://github.com/niXman/mingw-builds/issues/649) |
| Binutils | Ignore long path | (H) 9x | Do not resolve filename to UNC path on Windows 9x. |
| CRT | Fix missing function | 2003- | The thunk for `_wassert` is marked as x86 only. `_wassert` is not implemented in Windows Server 2003 R2 x64 either. |
| CRT | Allow skip space check in `ftruncate64` | NT- | The space check calls find volume APIs. |
| CRT | Use ANSI API | 9x | MinGW-w64 uses `GetModuleHandleW` to load `msvcrt.dll` in thunks. |
| winpthreads | Disable VEH | 2000- | MinGW-w64’s winpthreads use VEH (vectored exception handler, since Windows XP) to set thread name. |
| winpthreads | Fix thread | 9x | Fix invalid args calling `_beginthreadex` on Windows 9x. |
| winpthreads | Kernel32 thunk | 9x | Thunks for critical section APIs and misc. |
| gettext | Kernel32 thunk | (H) 9x | Thunks for critical section APIs. |
| GCC | Fix VT sequence | Any | Fix VT sequence confliction with UTF-8 prefix. |
| GCC | Fix locale directory | Any | [niXman/mingw-builds#666](https://github.com/niXman/mingw-builds/issues/666) |
| GCC | Parser-friendly diagnostics | Any | Keep the phrases “error:” and “warning:” unlocalized to make diagnostic messages parser-friendly. |
| GCC | Fix console code page | (H) Vista+ | Corresponding to UTF-8 manifest. |
| GCC | Disable `_aligned_malloc` | 2000- | libstdc++ and gomp prefer it but provide fallbacks. |
| GCC | Fix libbacktrace (NT 5.0) | ≥ 15, (H) 2000- | Missing library loader APIs. |
| GCC | Fix libbacktrace (NT 4.0) | ≥ 15, (H) NT- | Missing tool help APIs. Actually not required on 9x. |
| GCC | `<filesystem>` fallbacks | NT- | Allow `<filesystem>` to use ANSI API on Windows 9x. Also fixes misc missing APIs on NT 4.0. |
| GCC | `<print>` fallbacks to ANSI API | ≥ 14, TARGET < NT 4.0 | `<print>` assumes UTF-8 `string`s and thus calls Win32 Unicode API. |
| GCC | `<fstream>` fallbacks to ANSI API | TARGET < NT 4.0 | To support `basic_fstream(const filesystem::path::value_type *filename, ios_base::openmode mode)`. |
| GDB | Fix thread | (H) Vista- | Do not touch threads that are not suspended, which leads to crash on 2003/Vista x64. |
| GDB | Kernel32 thunk | (H) 2000- | Thunks for `GetConsoleProcessList` and `GetSystemWow64DirectoryA`. |
| GDB | IPv6 thunk | (H) 2000- | Thunks for `getaddrinfo`, `getnameinfo`, `freeaddrinfo`. |
| GDB | Fix VC6 CRT compatibility | (H) NT- | Disable time64 and largefile. |

Note:

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
