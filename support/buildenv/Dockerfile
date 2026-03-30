FROM docker.io/library/ubuntu:24.04

RUN apt-get update && \
    env DEBIAN_FRONTEND=noninteractive \
      apt-get install --no-install-recommends -y \
        autoconf automake build-essential cmake flex gettext libtool llvm ninja-build texinfo \
        7zip ca-certificates curl file gawk libarchive-tools python3 python3-libarchive-c python3-packaging zstd
