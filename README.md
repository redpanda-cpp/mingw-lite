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
- For profiles with HOST set to NT 5.2 or earlier, the UTF-8 manifest is removed from GCC.
