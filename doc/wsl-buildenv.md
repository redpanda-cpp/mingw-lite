# Prepare WSL Build Environment

The build script requires root privileges, so AN EXCLUSIVE DISTRO is highly recommended.

## Install WSL

- WSL 1: check “Windows Subsystem for Linux” in “Turn Windows features on or off” dialog is sufficient.
- WSL 2: see [Microsoft Learn](https://learn.microsoft.com/en-us/windows/wsl/install).

## Create an Exclusive Distro

1. Download Ubuntu 24.04 WSL rootfs tarball from [Ubuntu Cloud Images](https://cloud-images.ubuntu.com/wsl/noble/current/).
2. Import it into WSL, for example:
   ```pwsh
   wsl --import mingw-lite-buildenv C:\wsl\mingw-lite-buildenv Downloads\ubuntu-noble-wsl-amd64-ubuntu.rootfs.tar.gz
   ```
3. Restart Windows Terminal. “mingw-lite-buildenv” now appears in the dropdown list.

## Install Dependencies

1. Launch the exclusive distro (launch “Terminal”, select “mingw-lite-buildenv” from the dropdown list).
2. Install required packages:
   ```bash
   apt update
   apt install --no-install-recommends -y \
     autoconf automake build-essential flex libtool meson texinfo \
     7zip ca-certificates curl file gawk libarchive-tools python3 python3-packaging zstd
   ```

## Clone the Repository

Cloning to Windows filesystem is recommended. For example with Windows Git Bash:
```bash
git clone https://github.com/redpanda-cpp/mingw-lite.git /d/mingw-lite --config=core.autocrlf=false
```

Auto-converting line endings must be disabled. To disable in an existing repo, run:
```bash
# save workspace
git add *
git stash

# do convertion
git config core.autocrlf false
git reset --hard

# restore workspace
git stash pop
```
