FROM docker.io/library/ubuntu:20.04

RUN apt-get update && \
    env DEBIAN_FRONTEND=noninteractive \
      apt-get install --no-install-recommends -y \
        build-essential cmake flex gettext libtool llvm m4 ninja-build texinfo \
        ca-certificates curl file gawk libarchive-tools p7zip-full python3 python3-packaging zstd
