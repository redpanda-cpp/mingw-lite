# MinGW Lite

Extremely fast MinGW GCC build scripts for easy experiments.

## Build

```bash
podman run -it --rm -v $PWD:/mnt -w /mnt docker.io/amd64/ubuntu:24.04

# in the container
./main.sh -b <branch> -p <profile>
```

Available branches:

| Branch | GCC version | GCC status |
| ------- | ----------- | ---------- |
| 14 | 14.2.0 | latest |
| 15 | 15-20240804 | development |

Available profiles:

| Profile | Arch | CRT | Threads | Exception | HOST | TARGET |
| ------- | ---- | --- | ------- | --------- | ---- | ------ |
| 64-mcf | x86_64 | UCRT | mcf | seh | NT 6.1 | NT 6.1 |
| 32-mcf | i686 | UCRT | mcf | dwarf | NT 6.1 | NT 6.1 |
| 64-ucrt | x86_64 | UCRT | posix | seh | NT 6.1 | NT 5.2 |
| 32-ucrt | i686 | UCRT | posix | dwarf | NT 6.1 | NT 5.1 |
| 64-msvcrt | x86_64 | MSVCRT | posix | seh | NT 5.2 | NT 5.2 |
| 32-msvcrt | i686 | MSVCRT | posix | dwarf | NT 5.0 | NT 5.0 |

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

## Patch List

| Family | Patch | Applies to | Description |
| ------ | ----- | ---------- | ----------- |
| Binutils | Fix path corruption | Any | [niXman/mingw-builds#649](https://github.com/niXman/mingw-builds/issues/649) |
| CRT | Fix missing function | TARGET < NT 6.0 | The thunk for `_wassert` is marked as x86 only. `_wassert` is not implemented in Windows Server 2003 R2 x64 either. |
| winpthreads | Disable VEH | TARGET < NT 5.1 | MinGW-w64’s winpthreads use VEH (vectored exception handler, since Windows XP) to set thread name. |
| GCC | Fix VT sequence | Any | Fix VT sequence confliction with UTF-8 prefix. |
| GCC | Fix locale directory | Any | [niXman/mingw-builds#666](https://github.com/niXman/mingw-builds/issues/666) |
| GCC | Parser-friendly diagnostics | Any | Keep the phrases “error:” and “warning:” unlocalized to make diagnostic messages parser-friendly. |
| GCC | Fix console code page | HOST ≥ NT 6.0 | Corresponding to UTF-8 manifest. |
| GCC | Remove UTF-8 manifest | HOST < NT 6.0 | [skeeto/w64devkit#58](https://github.com/skeeto/w64devkit/issues/58) |
| GCC | Disable `_aligned_malloc` | TARGET < NT 5.1 | libstdc++ and gomp prefer it but provide fallbacks. |
| GDB | Fix XP Vista x64 | HOST < NT 6.1 | Do not touch threads that are not suspended. |
| GDB | Kernel32 thunk | HOST < NT 5.1 | Thunks for `GetConsoleProcessList` and `GetSystemWow64DirectoryA`. |
| GDB | IPv6 thunk | HOST < NT 5.1 | Thunks for `getaddrinfo`, `getnameinfo`, `freeaddrinfo`. |
