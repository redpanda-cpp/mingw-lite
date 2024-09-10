FROM docker.io/amd64/ubuntu:24.04

RUN apt-get update && \
    env DEBIAN_FRONTEND=noninteractive \
      apt-get install --no-install-recommends -y \
        build-essential flex meson \
        7zip ca-certificates curl file gawk libarchive-tools python3 python3-packaging zstd
