FROM docker.io/amd64/alpine:3.20

RUN apk update && \
    apk add \
      autoconf automake build-base flex gettext libtool meson texinfo \
      7zip ca-certificates curl file gawk libarchive-tools py3-packaging python3 zstd

ENV XMAKE_ROOT=y
