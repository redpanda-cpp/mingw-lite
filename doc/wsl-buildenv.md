# Prepare WSL Build Environment

The build scripts make use of overlayfs and require root privileges, so AN EXCLUSIVE WSL 2 DISTRO is required.

## Install WSL

See [Microsoft Learn](https://learn.microsoft.com/en-us/windows/wsl/install).

## Create an Exclusive Distro

1. Download Ubuntu 24.04 WSL rootfs tarball from [Ubuntu CD Images](https://cdimages.ubuntu.com/ubuntu-wsl/noble/daily-live/current/).
2. Import it into WSL, for example:
   ```pwsh
   wsl --import mingw-lite-buildenv C:\wsl\mingw-lite-buildenv Downloads\noble-wsl-amd64.wsl
   ```
3. Restart Windows Terminal. “mingw-lite-buildenv” now appears in the dropdown list.

## Install Dependencies

1. Launch the exclusive distro (launch “Terminal”, select “mingw-lite-buildenv” from the dropdown list).
2. Install required packages:
   ```bash
   apt update
   apt install --no-install-recommends -y \
     autoconf automake build-essential cmake flex gettext libtool llvm meson texinfo xmake \
     7zip ca-certificates curl file gawk libarchive-tools python3 python3-libarchive-c python3-packaging zstd
   ```
3. Set environment variables:
   ```bash
   echo "export XMAKE_ROOT=y" >>~/.bashrc
   ```

## Clone the Repository

Cloning to Windows filesystem is recommended. For example with Windows Git Bash:
```bash
git clone https://github.com/redpanda-cpp/mingw-lite.git /d/mingw-lite --config core.autocrlf=false --config core.symlinks=true
```

`core.autocrlf` must be disabled, and `core.symlinks` must be enabled. To convert an existing repo, run:
```bash
# save workspace
git add *
git stash

# do convertion
git config core.autocrlf false
git config core.symlinks true
git reset --hard

# restore workspace
git stash pop
```
