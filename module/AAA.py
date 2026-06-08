import argparse
import logging
import os
from packaging.version import Version
import shutil
import subprocess

from module.debug import shell_here
from module.path import ProjectPaths
from module.profile import BranchProfile
from module.util import cflags_A, configure, ensure, make_custom, make_default, make_destdir_install, overlayfs_ro
from module.util import cmake_build, cmake_config, cmake_flags_A, cmake_install

def _gmp(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  v = Version(ver.gmp)
  v_gcc = Version(config.build_gcc_version)
  build_dir = paths.src_dir.gmp / 'build-AAA'
  ensure(build_dir)

  c_extra = []

  # GCC 15 defaults to C23, in which `foo()` means `foo(void)` instead of `foo(...)`.
  if v_gcc.major >= 15 and v < Version('6.4.0'):
    c_extra.append('-std=gnu11')

  configure(build_dir, [
    f'--prefix=/usr/local',
    f'--host={config.build}',
    f'--build={config.build}',
    '--disable-assembly',
    '--enable-static',
    '--disable-shared',
    *cflags_A(
      c_extra = c_extra,
    ),
  ])
  make_default(build_dir, config.jobs)
  make_destdir_install(build_dir, paths.layer_AAA.gmp)

def _mpfr(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  with overlayfs_ro('/usr/local', [
    paths.layer_AAA.gmp / 'usr/local',
  ]):
    build_dir = paths.src_dir.mpfr / 'build-AAA'
    ensure(build_dir)
    configure(build_dir, [
      f'--prefix=/usr/local',
      f'--host={config.build}',
      f'--build={config.build}',
      '--enable-static',
      '--disable-shared',
      *cflags_A(),
    ])
    make_default(build_dir, config.jobs)
    make_destdir_install(build_dir, paths.layer_AAA.mpfr)

def _mpc(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  with overlayfs_ro('/usr/local', [
    paths.layer_AAA.gmp / 'usr/local',
    paths.layer_AAA.mpfr / 'usr/local',
  ]):
    build_dir = paths.src_dir.mpc / 'build-AAA'
    ensure(build_dir)
    configure(build_dir, [
      f'--prefix=/usr/local',
      f'--host={config.build}',
      f'--build={config.build}',
      '--enable-static',
      '--disable-shared',
      *cflags_A(),
    ])
    make_default(build_dir, config.jobs)
    make_destdir_install(build_dir, paths.layer_AAA.mpc)

def _zlib_net(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  build_dir = 'build-AAA'
  cmake_config(
    paths.src_dir.zlib_net,
    extra_args = [
      '-DCMAKE_INSTALL_PREFIX=/usr/local',
      '-DZLIB_BUILD_TESTING=OFF',
      '-DZLIB_BUILD_SHARED=OFF',
      *cmake_flags_A(),
    ],
    build_dir = build_dir,
  )
  cmake_build(
    paths.src_dir.zlib_net,
    jobs = config.jobs,
    build_dir = build_dir,
  )
  cmake_install(
    paths.src_dir.zlib_net,
    destdir = paths.layer_AAA.zlib,
    build_dir = build_dir,
  )

def build_AAA_library(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  _gmp(ver, paths, config)
  _mpfr(ver, paths, config)
  _mpc(ver, paths, config)
  _zlib_net(ver, paths, config)

def _python(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  with overlayfs_ro('/usr/local', [
    paths.layer_AAA.zlib / 'usr/local',
  ]):
    build_dir = paths.src_dir.python / 'build-AAA'
    ensure(build_dir)
    configure(build_dir, [
      f'--prefix=/usr/local',
      # static
      '--disable-shared',
      'MODULE_BUILDTYPE=static',
      # features
      '--disable-test-modules',
      # packages
      '--without-static-libpython',
      *cflags_A(),
    ])
    make_default(build_dir, config.jobs)
    make_destdir_install(build_dir, paths.layer_AAA.python)

def _setuptools(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  with overlayfs_ro('/usr/local', [
    paths.layer_AAA.zlib / 'usr/local',
    paths.layer_AAA.python / 'usr/local',
  ]):
    subprocess.run([
      'python3',
      'setup.py',
      'install',
      f'--root={paths.layer_AAA.setuptools}',
      '--prefix=/usr/local',
    ], cwd = paths.src_dir.setuptools, check = True)

def _meson(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  with overlayfs_ro('/usr/local', [
    paths.layer_AAA.zlib / 'usr/local',
    paths.layer_AAA.python / 'usr/local',
    paths.layer_AAA.setuptools / 'usr/local',
  ]):
    subprocess.run([
      'python3',
      'setup.py',
      'install',
      f'--root={paths.layer_AAA.meson}',
      '--prefix=/usr/local',
    ], cwd = paths.src_dir.meson, check = True)

def _wrapper(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  bin_dir = paths.layer_AAA.wrapper / 'usr/local/bin'
  ensure(bin_dir)

  shutil.copy(paths.in_tree_src_tree.wrapper / 'ar-wrapper', bin_dir / 'ar-wrapper')
  shutil.copy(paths.in_tree_src_tree.wrapper / 'dlltool-wrapper', bin_dir / 'dlltool-wrapper')

def _xmake(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  subprocess.run([
    './configure',
    f'--prefix=/usr/local',
  ], cwd = paths.src_dir.xmake, check = True)
  make_default(paths.src_dir.xmake, config.jobs)
  make_destdir_install(paths.src_dir.xmake, paths.layer_AAA.xmake)

def build_AAA_tool(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  _python(ver, paths, config)
  _setuptools(ver, paths, config)
  _meson(ver, paths, config)
  _wrapper(ver, paths, config)
  _xmake(ver, paths, config)
