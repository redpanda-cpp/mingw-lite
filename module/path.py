import argparse
from packaging.version import Version
from pathlib import Path
from typing import Optional

from module.profile import BranchVersions

class ProjectPaths:
  root: Path

  assets: Path
  dist: Path
  patch: Path

  build: Path
  dep: Path
  x_dep: Path

  prefix: Path
  x_prefix: Path

  arx: Path
  x_arx: Path

  binutils: Path
  gettext: Optional[Path]
  gcc: Path
  gdb: Path
  gmp: Path
  iconv: Path
  make: Path
  mcfgthread: Optional[Path]
  mingw: Path
  mpc: Path
  mpfr: Path

  binutils_arx: Path
  gettext_arx: Optional[Path]
  gcc_arx: Path
  gdb_arx: Path
  gmp_arx: Path
  iconv_arx: Path
  make_arx: Path
  mcfgthread_arx: Optional[Path]
  mingw_arx: Path
  mpc_arx: Path
  mpfr_arx: Path

  def __init__(
    self,
    config: argparse.Namespace,
    ver: BranchVersions,
  ):
    self.root = Path.cwd()

    self.assets = self.root / 'assets'
    self.dist = self.root / 'dist'
    self.patch = self.root / 'patch'

    dir = f'mingw{config.profile}-{config.branch}'

    self.build = Path(f'/tmp/build/{dir}')
    self.dep = self.build / 'deps'
    self.x_dep = self.build / 'x-deps'

    self.prefix = Path(f'/opt/{dir}')
    self.x_prefix = Path(f'/opt/x-{dir}')

    self.arx = self.dist / f'mingw{config.profile}-{ver.gcc}-r{ver.rev}.7z'
    self.x_arx = self.dist / f'x-mingw{config.profile}-{ver.gcc}-r{ver.rev}.tar.zst'

    binutils = f'binutils-{ver.binutils}'
    self.binutils = self.build / binutils
    if Version(ver.binutils) >= Version('2.43'):
      self.binutils_arx = self.assets / f'{binutils}.tar.zst'
    elif Version(ver.binutils) >= Version('2.28.1'):
      self.binutils_arx = self.assets / f'{binutils}.tar.xz'
    else:
      self.binutils_arx = self.assets / f'{binutils}.tar.bz2'

    if ver.gettext:
      gettext = f'gettext-{ver.gettext}'
      self.gettext = self.build / gettext
      self.gettext_arx = self.assets / f'{gettext}.tar.xz'
    else:
      self.gettext = None
      self.gettext_arx = None

    gcc = f'gcc-{ver.gcc}'
    self.gcc = self.build / gcc
    if Version(ver.gcc).major >= 5:
      self.gcc_arx = self.assets / f'{gcc}.tar.xz'
    else:
      self.gcc_arx = self.assets / f'{gcc}.tar.bz2'

    gdb = f'gdb-{ver.gdb}'
    self.gdb = self.build / gdb
    if Version(ver.gdb) >= Version('7.8'):
      self.gdb_arx = self.assets / f'{gdb}.tar.xz'
    else:
      self.gdb_arx = self.assets / f'{gdb}.tar.bz2'

    gmp = f'gmp-{ver.gmp}'
    self.gmp = self.build / gmp
    if Version(ver.gmp) >= Version('6.2.0'):
      self.gmp_arx = self.assets / f'{gmp}.tar.zst'
    else:
      self.gmp_arx = self.assets / f'{gmp}.tar.xz'

    iconv = f'libiconv-{ver.iconv}'
    self.iconv = self.build / iconv
    self.iconv_arx = self.assets / f'{iconv}.tar.gz'

    make = f'make-{ver.make}'
    self.make = self.build / make
    if Version(ver.make) >= Version('4.3'):
      self.make_arx = self.assets / f'{make}.tar.lz'
    else:
      self.make_arx = self.assets / f'{make}.tar.bz2'

    if ver.mcfgthread:
      mcfgthread = f'mcfgthread-{ver.mcfgthread}'
      self.mcfgthread = self.build / mcfgthread
      self.mcfgthread_arx = self.assets / f'{mcfgthread}.tar.gz'
    else:
      self.mcfgthread = None
      self.mcfgthread_arx = None

    mingw = f'mingw-w64-v{ver.mingw}'
    self.mingw = self.build / mingw
    if Version(ver.mingw).major >= 3:
      self.mingw_arx = self.assets / f'{mingw}.tar.bz2'
    else:
      self.mingw_arx = self.assets / f'{mingw}.tar.gz'

    mpc = f'mpc-{ver.mpc}'
    self.mpc = self.build / mpc
    self.mpc_arx = self.assets / f'{mpc}.tar.gz'

    mpfr = f'mpfr-{ver.mpfr}'
    self.mpfr = self.build / mpfr
    self.mpfr_arx = self.assets / f'{mpfr}.tar.xz'
