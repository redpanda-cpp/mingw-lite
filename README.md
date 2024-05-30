# MinGW Lite

Extremely fast MinGW GCC build scripts for easy experiments.

## Build

```bash
podman run -it --rm -v $PWD:/mnt -w /mnt docker.io/amd64/ubuntu:20.04

# in the container
./main.sh -p <profile>
```

Available profiles:

| Profile | Arch | CRT | Threads | Exception | HOST | TARGET |
| ------- | ---- | --- | ------- | --------- | ---- | ------ |
| 64-mcf | x86_64 | UCRT | mcf | seh | NT 6.1 | NT 6.1 |
| 32-mcf | i686 | UCRT | mcf | dwarf | NT 6.1 | NT 6.1 |
| 64-ucrt | x86_64 | UCRT | posix | seh | NT 6.1 | NT 5.2 |
| 32-ucrt | i686 | UCRT | posix | dwarf | NT 6.1 | NT 5.1 |
| 64-msvcrt | x86_64 | MSVCRT | posix | seh | NT 5.2 | NT 5.2 |
| 32-msvcrt | i686 | MSVCRT | posix | dwarf | NT 5.1 | NT 5.1 |
| 32-win2000 | i686 | MSVCRT | posix | dwarf | NT 5.0 | NT 5.0 |

Notes:

- The build scripts write build files to host file system.
- For profiles with HOST set to NT 5.2 or earlier, the UTF-8 manifest is removed from GCC.
