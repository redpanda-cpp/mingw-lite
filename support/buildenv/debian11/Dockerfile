FROM docker.io/library/debian:11

RUN apt-get update && \
    env DEBIAN_FRONTEND=noninteractive \
      apt-get install --no-install-recommends -y \
        autoconf automake build-essential cmake flex gettext libtool llvm ninja-build texinfo \
        ca-certificates curl file gawk libarchive-tools p7zip-full python3 python3-packaging zstd
