FROM docker.io/library/debian:13

RUN apt-get update && \
    env DEBIAN_FRONTEND=noninteractive \
      apt-get install --no-install-recommends -y \
        autoconf automake build-essential cmake flex gettext libtool llvm ninja-build texinfo \
        7zip ca-certificates curl file gawk libarchive-tools python3 python3-packaging zstd
