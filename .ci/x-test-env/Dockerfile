FROM docker.io/library/archlinux:base

RUN pacman -Syu --noconfirm && \
    pacman -S --noconfirm \
      musl python python-packaging wine

# avoid wine failures (possibly race condition)
#   wine: created the configuration directory '/root/.wine'
#   wine: chdir to /tmp/wine-wr755y/server-801-6f0401 : No such file or directory
RUN wineboot --init
