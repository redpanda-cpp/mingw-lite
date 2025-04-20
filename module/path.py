import argparse
from packaging.version import Version
from pathlib import Path
from typing import Optional

from module.profile import BranchProfile

class ProjectPaths:
  root: Path

  assets: Path
  dist: Path
  patch: Path

  mingw_prefix: Path
  x_prefix: Path

  mingw_pkg: Path
  x_pkg: Path

  # build phase

  build: Path
  build_host: Path
  build_target: Path
  dep: Path
  x_dep: Path

  binutils: Path
  gettext: Optional[Path]
  gcc: Path
  gdb: Path
  gmp: Path
  iconv: Path
  make: Path
  mcfgthread: Optional[Path]
  mingw_host: Path
  mingw_target: Path
  mpc: Path
  mpfr: Path
  python: Path
  python_z: Path
  xmake: Path

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
  python_arx: Path
  python_z_arx: Path
  xmake_arx: Path

  # test phase

  test: Path
  test_src: Path

  test_mingw: Path

  def __init__(
    self,
    config: argparse.Namespace,
    ver: BranchProfile,
  ):
    self.root = Path.cwd()

    self.assets = self.root / 'assets'
    self.dist = self.root / 'dist'
    self.patch = self.root / 'patch'

    dir = f'mingw{config.profile}-{config.branch}'

    self.mingw_prefix = Path(f'/opt/{dir}')
    self.x_prefix = Path(f'/opt/x-{dir}')

    self.mingw_pkg = self.dist / f'mingw{config.profile}-{ver.gcc}-r{ver.rev}.tar.zst'
    self.x_pkg = self.dist / f'x-mingw{config.profile}-{ver.gcc}-r{ver.rev}.tar.zst'

    # build phase

    self.build = Path(f'/tmp/build/{dir}')
    self.build_host = self.build / 'host'
    self.build_target = self.build / 'target'
    self.dep = self.build / 'deps'
    self.x_dep = self.build / 'x-deps'

    binutils = f'binutils-{ver.binutils}'
    self.binutils = self.build / binutils
    if Version(ver.binutils) >= Version('2.43'):
      self.binutils_arx = self.assets / f'{binutils}.tar.zst'
    else:
      self.binutils_arx = self.assets / f'{binutils}.tar.xz'

    gcc = f'gcc-{ver.gcc}'
    self.gcc = self.build / gcc
    self.gcc_arx = self.assets / f'{gcc}.tar.xz'

    gdb = f'gdb-{ver.gdb}'
    self.gdb = self.build / gdb
    self.gdb_arx = self.assets / f'{gdb}.tar.xz'

    if ver.gettext:
      gettext = f'gettext-{ver.gettext}'
      self.gettext = self.build / gettext
      self.gettext_arx = self.assets / f'{gettext}.tar.xz'
    else:
      self.gettext = None
      self.gettext_arx = None

    gmp = f'gmp-{ver.gmp}'
    self.gmp = self.build / gmp
    self.gmp_arx = self.assets / f'{gmp}.tar.zst'

    iconv = f'libiconv-{ver.iconv}'
    self.iconv = self.build / iconv
    self.iconv_arx = self.assets / f'{iconv}.tar.gz'

    make = f'make-{ver.make}'
    self.make = self.build / make
    self.make_arx = self.assets / f'{make}.tar.lz'

    if ver.mcfgthread:
      mcfgthread = f'mcfgthread-{ver.mcfgthread}'
      self.mcfgthread = self.build / mcfgthread
      self.mcfgthread_arx = self.assets / f'{mcfgthread}.tar.gz'
    else:
      self.mcfgthread = None
      self.mcfgthread_arx = None

    mingw = f'mingw-w64-v{ver.mingw}'
    self.mingw_host = self.build_host / mingw
    self.mingw_target = self.build_target / mingw
    self.mingw_arx = self.assets / f'{mingw}.tar.bz2'

    mpc = f'mpc-{ver.mpc}'
    self.mpc = self.build / mpc
    self.mpc_arx = self.assets / f'{mpc}.tar.gz'

    mpfr = f'mpfr-{ver.mpfr}'
    self.mpfr = self.build / mpfr
    self.mpfr_arx = self.assets / f'{mpfr}.tar.xz'

    python = f'Python-{ver.python}'
    python_z = f'zlib-{ver.python_z}'
    self.python = self.build / python
    self.python_arx = self.assets / f'{python}.tar.xz'
    self.python_z = self.python / python_z
    self.python_z_arx = self.assets / f'{python_z}.tar.gz'

    self.xmake = self.build / f'xmake-{ver.xmake}'
    self.xmake_arx = self.assets / f'xmake-v{ver.xmake}.tar.gz'

    # test phase

    self.test = Path(f'/tmp/test/{dir}')
    self.test_src = self.root / 'support' / 'test'

    self.test_mingw = self.test / dir
