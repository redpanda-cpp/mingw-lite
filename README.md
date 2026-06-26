# MinGW Lite

- Fast MinGW-w64 GCC build scripts for easy experiments.
- MinGW-w64 GCC distribution with minimal dependencies.
- Modern C++ for the good old OSes (back to Windows NT 4.0 and Windows 98).

## Use

- Native toolchain: the toolchain is static by default.
  - The toolchain provides optional shared runtime libraries.
  - To opt-in shared runtime libraries, copy `$prefix/lib/shared/*` or `$prefix/lib/shared-unstable/*` to `$prefix/`.
  - Do not mix shared runtime libraries from different branches or profiles.
  - If the shared runtime libraries are marked “unstable” (i.e. branch is not frozen, or [thunks](./doc/thunk.md) are applied to target CRT), do not mix them from different release versions.
- Cross toolchain: tools are built for Ubuntu 20.04 (glibc 2.31) and organized by package. Mount required packages to `/usr/local`:
  ```bash
  layers=(
    /path/to/mingw/AAB/{binutils,crt-target,gcc,gcc-lib,headers,mcfgthread,winpthreads}/usr/local
    # optional shared layers
    /path/to/mingw/AAB/{crt-shared,gcc-lib-shared,mcfgthread-shared,winpthreads-shared}/usr/local
  )
  lowerdir=$(IFS=:; echo "${layers[*]}")
  sudo mount -t overlay none /usr/local -o lowerdir=$lowerdir
  ```
  For container environment, `CAP_SYS_ADMIN` required (`--cap-add=sys_admin`).

  > Note: [There are two variants of CRT libraries, host CRT (`AAB/crt-host`) and target CRT (`AAB/crt-target`).](./doc/internal-build.md) The target CRT is always preferred for general integration, such as building static or shared libraries, and building applications. The shared runtime libraries are linked with target CRT, so never use them with host CRT.

## Build

There are several options for build environment:

| Host OS | Technology | Build OS |
| ------- | ---------- | -------- |
| Linux | Rootless container (podman) | Ubuntu 20.04 (default)<br>Alpine Linux 3.23<br>Arch Linux<br>Debian ≥ 11<br>Ubuntu ≥ 20.04 |
| Windows | WSL | Ubuntu 20.04 |

Steps:

0. Special note for Windows host: symbolic link required.
   - Windows privilege: enable “Developer Mode”.
   - Git: `git config --global core.symlinks true`, or `git clone --config core.symlinks=true`.
1. Prepare build environment. Linux rootless container:
   ```bash
   podman build -t mingw-lite/buildenv support/buildenv
   ```
   Windows WSL:
   ```powershell
   ./support/buildenv/wsl/setup.ps1

   # options:
   #   -Distro: WSL distro name (default: mlenv)
   #   -Root: WSL install location (default: ./env)
   #   -Version: WSL version (default: 2)
   ```
2. Launch build environment. Linux rootless container:
   ```bash
   podman run -it --rm \
     --cap-add=sys_admin \
     -v $PWD:/mnt -w /mnt \
     mingw-lite/buildenv
   ```
   To expose build directories for debugging:
   ```bash
   podman run -it --rm \
     --cap-add=sys_admin \
     -v $PWD:/mnt -w /mnt \
     -v $PWD/build:/tmp/build \
     -v $PWD/layer:/tmp/layer \
     mingw-lite/buildenv
   ```
   Windows WSL:
   ```powershell
   wsl -d mlenv
   ```
3. In the build environment, run:
   ```bash
   ./main.py -b <branch> -p <profile>
   ```
   Run `./main.py -h` for help.

Available branches:

| Branch | GCC version | MinGW | Binutils | GDB | Optimize | TLS |
| ------ | ----------- | ----- | -------- | --- | -------- | --- |
| next | 17-20260621 | 14.0.0 | 2.46.0 | 17.1 | Speed | Native |
| current | 16-20260620 | 14.0.0 | 2.46.0 | 17.1 | Speed | Native |
| 16 | 16.1.0 | 14.0.0 | 2.46.0 | 17.1 | Speed | Native |
| 15 ❄️ | 15.3.0 | 13.0.0 | 2.45.1 | 17.1 | Size | Emulated |
| 14 ❄️ | 14.4.0 | 12.0.0 | 2.43.1 | 15.2 | Size | Emulated |
| 13 ❄️ | 13.4.0 | 11.0.1 | 2.41 | 14.2 | Size | Emulated |

A profile is composed of bitness and predefined ABI variant. The combination of CRT, thread model and exception model cannot be freely configured.

| ABI variant \ Bitness | 64 (seh) | 32 (dwarf) |
| --------------------- | -------- | ---------- |
| mcf (ucrt, mcf) | 64-mcf | 32-mcf |
| win32 (ucrt, win32) | 64-win32 | 32-win32 |
| ucrt (ucrt, posix) | 64-ucrt | 32-ucrt |
| msvcrt (msvcrt, posix) | 64-msvcrt | 32-msvcrt |

## Micro Architecture

By default, MinGW Lite is very conservative in micro architecture -- the “64”-bit profiles target the baseline “x86-64” (sse2, 2003); the “32”-bit profiles target “pentium4” (sse2, 2000).

For better performance, there are “64”-bit “x86-64-v2” (sse4.2, 2008) variants: 64_v2-mcf, 64_v2-win32, 64_v2-ucrt, 64_v2-msvcrt. In addition, they have LTO enabled for GCC and Binutils (and thus optimized for speed in old branches).

To work with even older CPUs, there are “32”-bit “i686” (cmov, 1995), “i486” [atomic (bswap, cmpxchg, xadd), 1989] and the baseline “i386” (1985) variants for earlier Windows versions (see below).

## Supported OS Versions

The default `_WIN32_WINNT` value for each branch is based on the earliest Windows version that is supported at the freeze point, the winter solstice after GCC’s release. (Currently 0x0A00 for all branches.)

Python (GDB scripting engine) often limits the toolchain’s minimum supported OS. However, Python is sometimes a bit aggressive, so we use some [thunks](./doc/thunk.md) to bring back support for earlier Windows versions. The minimum supported OS is the one where the shared runtime libraries are thunk-free.

| Profile | Min OS 16+ | Min OS 15, 14, 13 |
| ------- | ---------- | ----------------- |
| *-mcf | NT 6.1 (7) | NT 6.1 (7) |
| *-win32 | NT 6.0 (Vista) | NT 6.0 (Vista) |
| {64,64_v2}-{ucrt,msvcrt} | NT 6.0 (Vista) | NT 5.2 (Server 2003) |
| 32-{ucrt,msvcrt} | NT 6.0 (Vista) | NT 5.1 (XP) |

Some profiles have variants for even earlier Windows versions (and possibly older CPUs), as follows.

| Profile variant | Min OS 16+ | Min OS 15, 14, 13 |
| --------------- | ---------- | ----------------- |
| 64-ucrt_ws2003        | NT 5.2 (Server 2003) | |
| 64-msvcrt_ws2003      | NT 5.2 (Server 2003) | |
| 32-ucrt_winxp         | NT 5.1 (XP) | |
| 32-msvcrt_win2000     | NT 5.0 (2000) | NT 5.0 (2000) |
| 32_686-msvcrt_win98   | NT 4.0, 4.10 (98) | NT 4.0, 4.10 (98) |
| 32_486-msvcrt_win98   | NT 4.0, 4.10 (98) | NT 4.0, 4.10 (98) |
| 32_386-msvcrt_win95   | | NT 4.0, 4.10 (98)<br>4.00 (95, limited) |

Limitations on Windows 95:

- Prerequisite: Windows socket 2 update (for `msvcrt.dll` and `ws2_32.dll`).
- Loading a dll that has static TLS is impossible. ([KB118816](./doc/kb-118816.md))
  - Unfortunately, GNU toolchain always add static TLS without checking necessity.
  - GCC needs `-fno-lto` to prevent the linker from dynamically loading the LTO plugin.
  - The profile is no longer available since branch 16 as we are switching to native TLS.
- Atomic operations will introduce observable overhead by calling libatomic subroutines.

## Beyond MinGW Lite

There are profiles that reuse MinGW Lite’s build scripts, but have characteristics that are not adoptable in MinGW Lite.

- [MinGW ∞ (mingw-infinity)](https://github.com/redpanda-cpp/mingw-infinity) (64-u8crt, 32-u8crt): derived from ucrt profiles, they reinterpret strings as UTF-8 in CRT functions (stdio, filesystem, etc.).
