import argparse
import logging
import os
from packaging.version import Version
from shutil import copyfile
import subprocess

from module.debug import shell_here
from module.path import ProjectPaths
from module.profile import BranchProfile
from module.util import cflags_A, configure, ensure, make_custom, make_default, make_destdir_install, overlayfs_ro

def _gmp(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  build_dir = paths.src_dir.gmp / 'build-AAA'
  ensure(build_dir)
  configure('gmp', build_dir, [
    f'--prefix=/usr/local',
    f'--host={config.build}',
    f'--build={config.build}',
    '--disable-assembly',
    '--enable-static',
    '--disable-shared',
    *cflags_A(),
  ])
  make_default('gmp', build_dir, config.jobs)
  make_destdir_install('gmp', build_dir, paths.layer_AAA.gmp)

def _mpfr(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  with overlayfs_ro('/usr/local', [
    paths.layer_AAA.gmp / 'usr/local',
  ]):
    build_dir = paths.src_dir.mpfr / 'build-AAA'
    ensure(build_dir)
    configure('mpfr', build_dir, [
      f'--prefix=/usr/local',
      f'--host={config.build}',
      f'--build={config.build}',
      '--enable-static',
      '--disable-shared',
      *cflags_A(),
    ])
    make_default('mpfr', build_dir, config.jobs)
    make_destdir_install('mpfr', build_dir, paths.layer_AAA.mpfr)

def _mpc(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  with overlayfs_ro('/usr/local', [
    paths.layer_AAA.gmp / 'usr/local',
    paths.layer_AAA.mpfr / 'usr/local',
  ]):
    build_dir = paths.src_dir.mpc / 'build-AAA'
    ensure(build_dir)
    configure('mpc', build_dir, [
      f'--prefix=/usr/local',
      f'--host={config.build}',
      f'--build={config.build}',
      '--enable-static',
      '--disable-shared',
      *cflags_A(),
    ])
    make_default('mpc', build_dir, config.jobs)
    make_destdir_install('mpc', build_dir, paths.layer_AAA.mpc)

def _z(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  build_dir = paths.src_dir.z / 'build-AAA'
  ensure(build_dir)
  configure('zlib for python', build_dir, [
    '--prefix=/usr/local',
    '--static',
  ])
  make_default('zlib for python', build_dir, config.jobs)
  make_destdir_install('zlib for python', build_dir, paths.layer_AAA.z)

def build_AAA_library(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  _gmp(ver.gmp, paths, config)

  _mpfr(ver.mpfr, paths, config)

  _mpc(ver.mpc, paths, config)

  _z(ver, paths, config)

def _python(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  with overlayfs_ro('/usr/local', [
    paths.layer_AAA.z / 'usr/local',
  ]):
    build_dir = paths.src_dir.python / 'build-AAA'
    ensure(build_dir)
    configure('python', build_dir, [
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
    make_default('python', build_dir, config.jobs)
    make_destdir_install('python', build_dir, paths.layer_AAA.python)

def build_AAA_tool(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  _python(ver, paths, config)
