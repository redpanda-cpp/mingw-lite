# MinGW Lite

Extremely fast MinGW GCC build scripts for easy experiments.

## Use

- Native toolchain: same as [other distributions](https://www.mingw-w64.org/downloads/).
- Cross toolchain: tools are organized by package. Mount required packages to `/usr/local`:
  ```bash
  layers=(/path/to/mingw/AAB/{binutils,crt,gcc,headers}/usr/local)
  lowerdir=$(IFS=:; echo "${layers[*]}")
  sudo mount -t overlay none /usr/local -o lowerdir=$lowerdir
  ```
  For container environment, `CAP_SYS_ADMIN` required (`--cap-add=sys_admin`).

## Build

1. Prepare build environment. Linux:
   ```bash
   podman build -t mingw-lite/buildenv-ubuntu support/buildenv-ubuntu
   ```
   For Windows host, [create an exclusive WSL distro for mingw-lite](doc/wsl-buildenv.md).
2. Launch build environment. Linux:
   ```bash
   podman run -it --rm \
     --cap-add=sys_admin \
     -v $PWD:/mnt -w /mnt \
     mingw-lite/buildenv-ubuntu
   ```
   To expose build directories for debugging:
   ```bash
   podman run -it --rm \
     --cap-add=sys_admin \
     -v $PWD:/mnt -w /mnt \
     -v $PWD/build:/tmp/build \
     -v $PWD/layer:/tmp/layer \
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
| 16 | 16-20250608 | 12.0.0 | 2.44 | 16.3 |
| 15 | 15.1.0 | 12.0.0 | 2.44 | 16.3 |
| 14 | 14.3.0 | 12.0.0 ❄️ | 2.43.1 ❄️ | 15.2 ❄️ |
| 13 | 13.3.0 | 11.0.1 ❄️ | 2.41 ❄️ | 14.2 ❄️ |

A profile is composed of bitness and predefined ABI variant. The combination of CRT, thread model and exception model cannot be freely configured.

| ABI variant \ Bitness | 64 (seh) | 32 (dwarf) |
| --------------------- | -------- | ---------- |
| mcf (ucrt, mcf) | 64-mcf | 32-mcf |
| win32 (ucrt, win32) | 64-win32 | 32-win32 |
| ucrt (ucrt, posix) | 64-ucrt | 32-ucrt |
| msvcrt (msvcrt, posix) | 64-msvcrt | 32-msvcrt |

## Supported OS Versions

The default `_WIN32_WINNT` value for each branch is based on the earliest Windows version that is supported at the freeze point, the winter solstice after GCC’s release. (Currently 0x0A00 for all branches.)

Python (GDB scripting engine) often limits the toolchain’s minimum supported OS. However, Python is sometimes a bit aggressive, so we use some thunks to bring back support for earlier Windows versions. There is no well-defined rule for minimum supported OS; the one that costs “reasonable” effort is chosen.

| ABI variant | Minimum supported OS |
| ----------- | -------------------- |
| mcf | Windows 7 (NT 6.1) |
| win32 | Windows Vista (NT 6.0) |
| ucrt | Windows Vista (NT 6.0) |
| msvcrt | Windows Vista (NT 6.0) |

Some profiles have variants for even earlier Windows versions, as follows.

| Profile variant | Minimum supported OS |
| --------------- | -------------------- |
| 64-ucrt_ws2003 | Windows Server 2003 (NT 5.2) |
| 64-msvcrt_ws2003 | Windows Server 2003 (NT 5.2) |
| 32-ucrt_winxp | Windows XP (NT 5.1) |
| 32-msvcrt_winnt40 | Windows NT 4.0 |

**Technical notes**: inspired by [YY-Thunks](https://github.com/Chuyu-Team/YY-Thunks), our legacy OS support is achieved by thunks. A thunk is small piece of code that wrap the original Win32 or CRT function, providing alternative implementation when the function is not available on the target OS. Absolutely necessary thunks that support C++ standard library are built into import libraries. No extra operation is required.
