# MinGW Lite

Extremely fast MinGW GCC build scripts for easy experiments.

## Build

1. Prepare build environment. Linux:
   ```bash
   podman build -t mingw-lite/buildenv-ubuntu support/buildenv-ubuntu
   ```
   For Windows host, [create an exclusive WSL distro for mingw-lite](doc/wsl-buildenv.md).
2. Launch build environment. Linux:
   ```bash
   podman run -it --rm -v $PWD:/mnt -w /mnt mingw-lite/buildenv-ubuntu
   ```
   To expose build directories for debugging:
   ```bash
   podman run -it --rm \
     -v $PWD:/mnt -w /mnt \
     -v $PWD/build:/tmp/build \
     -v $PWD/pkg:/opt \
     mingw-lite/buildenv-ubuntu
   ```
   Windows: in “Terminal”, select “mingw-lite-buildenv” from the dropdown list.
3. In the build environment, run:
   ```bash
   ./main.py -b <branch> -p <profile>
   ```

Available branches:

| Branch | GCC version | MinGW | Binutils | GDB |
| ------ | ----------- | ----- | -------- | --- |
| 15 | 15-20250330 | 12.0.0 | 2.44 | 16.2 |
| 14 | 14.2.0 | 12.0.0 | 2.43.1 | 15.2 |
| 13 | 13.3.0 | 11.0.1 | 2.41 | 14.2 |

A profile is composed of bitness and predefined ABI variant. The combination of CRT, thread model and exception model cannot be freely configured.

| ABI variant \ Bitness | 64 (seh) | 32 (dwarf) |
| --------------------- | -------- | ---------- |
| mcf (ucrt, mcf) | 64-mcf | 32-mcf |
| win32 (ucrt, win32) | 64-win32 | 32-win32 |
| ucrt (ucrt, posix) | 64-ucrt | 32-ucrt |
| msvcrt (msvcrt, posix) | 64-msvcrt | 32-msvcrt |

## Supported OS Versions

The default `_WIN32_WINNT` value is based on the earliest Windows version that is supported at the end of the GCC’s release year. (Currently 0x0A00 for all branches.)

By default, the toolchain’s minimum supported OS is based on the earliest Windows version that is suported at the publication date of the ISO C++ standard related to GCC’s default C++ standard. Some variants extend the minimum supported OS to earlier Windows versions.

Currently GCCs in all branches default to `-std=gnu++17`, with related ISO C++ standard published on 2017-12. That is to say, the default minimum supported OS is Windows 7 (NT 6.1).
