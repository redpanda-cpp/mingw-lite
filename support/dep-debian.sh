#!/bin/sh

set -ex

apt-get update
env DEBIAN_FRONTEND=noninteractive \
  apt-get install --no-install-recommends -y \
  build-essential flex libgmp-dev libmpc-dev libmpfr-dev meson \
  ca-certificates curl file gawk libarchive-tools p7zip-full python3 python3-packaging zstd
