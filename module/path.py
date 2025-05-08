import argparse
from packaging.version import Version
from pathlib import Path
import tempfile
from typing import NamedTuple, Optional

from module.profile import BranchProfile

class SourcePaths(NamedTuple):
  binutils: Path
  expat: Path
  gettext: Path
  gcc: Path
  gdb: Path
  gmp: Path
  iconv: Path
  make: Path
  mcfgthread: Path
  mingw_host: Path
  mingw_target: Path
  mpc: Path
  mpfr: Path
  pdcurses: Path
  python: Path
  xmake: Path
  z: Path

class LayerPathsAAA(NamedTuple):
  prefix: Path

  gmp: Path
  mpc: Path
  mpfr: Path
  z: Path

  python: Path
  xmake: Path

class LayerPathsAAB(NamedTuple):
  prefix: Path

  binutils: Path
  crt: Path
  gcc: Path
  headers: Path

  expat: Path
  gettext: Path
  gmp: Path
  iconv: Path
  mpc: Path
  mpfr: Path
  pdcurses: Path
  python: Path

class LayerPathsABB(NamedTuple):
  prefix: Path

  binutils: Path
  crt: Path
  gcc: Path
  gdb: Path
  headers: Path
  make: Path
  xmake: Path

  license: Path

class ProjectPaths:
  root_dir: Path

  assets_dir: Path
  dist_dir: Path
  patch_dir: Path

  mingw_pkg: Path
  cross_pkg: Path

  # build phase

  build_dir: Path
  build_host: Path
  build_target: Path
  pkg_dir: Path
  layer_dir: Path
  utf8_src_dir: Path

  src_dir: SourcePaths
  src_arx: SourcePaths

  layer_AAA: LayerPathsAAA
  layer_AAB: LayerPathsAAB
  layer_ABB: LayerPathsABB

  # test phase

  test_dir: Path
  test_src_dir: Path

  test_mingw_dir: Path

  # target test archive phase

  test_archive_dir: Path
  test_archive_mingw_dir: Path

  def __init__(
    self,
    config: argparse.Namespace,
    ver: BranchProfile,
  ):
    self.root_dir = Path.cwd()

    self.assets_dir = self.root_dir / 'assets'
    self.dist_dir = self.root_dir / 'dist'
    self.patch_dir = self.root_dir / 'patch'

    dir = f'mingw{config.profile}-{config.branch}'

    self.mingw_pkg = self.dist_dir / f'mingw{config.profile}-{ver.gcc}-r{ver.rev}.tar.zst'
    self.cross_pkg = self.dist_dir / f'x-mingw{config.profile}-{ver.gcc}-r{ver.rev}.tar.zst'

    # build phase

    self.build_dir = Path(f'/tmp/build/{dir}')
    self.build_host = self.build_dir / 'host'
    self.build_target = self.build_dir / 'target'
    self.pkg_dir = Path(f'/tmp/pkg/{dir}')
    self.layer_dir = Path(f'/tmp/layer/{dir}')
    self.utf8_src_dir = self.root_dir / 'support' / 'utf8'

    src_name = SourcePaths(
      binutils = f'binutils-{ver.binutils}',
      expat = f'expat-{ver.expat}',
      gcc = f'gcc-{ver.gcc}',
      gdb = f'gdb-{ver.gdb}',
      gettext = f'gettext-{ver.gettext}'
        if ver.gettext
        else None,
      gmp = f'gmp-{ver.gmp}',
      iconv = f'libiconv-{ver.iconv}',
      make = f'make-{ver.make}',
      mcfgthread = f'mcfgthread-{ver.mcfgthread}'
        if ver.thread == 'mcf'
        else None,
      mingw_host = f'mingw-w64-v{ver.mingw}',
      mingw_target = f'mingw-w64-v{ver.mingw}',
      mpc = f'mpc-{ver.mpc}',
      mpfr = f'mpfr-{ver.mpfr}',
      pdcurses = f'PDCurses-{ver.pdcurses}',
      python = f'Python-{ver.python}',
      xmake = f'xmake-{ver.xmake}',
      z = f'zlib-{ver.z}',
    )

    self.src_dir = SourcePaths(
      binutils = self.build_dir / src_name.binutils,
      expat = self.build_dir / src_name.expat,
      gcc = self.build_dir / src_name.gcc,
      gdb = self.build_dir / src_name.gdb,
      gettext = self.build_dir / src_name.gettext
        if ver.gettext
        else None,
      gmp = self.build_dir / src_name.gmp,
      iconv = self.build_dir / src_name.iconv,
      make = self.build_dir / src_name.make,
      mcfgthread = self.build_dir / src_name.mcfgthread
        if ver.thread == 'mcf'
        else None,
      mingw_host = self.build_host / src_name.mingw_host,
      mingw_target = self.build_target / src_name.mingw_target,
      mpc = self.build_dir / src_name.mpc,
      mpfr = self.build_dir / src_name.mpfr,
      pdcurses = self.build_dir / src_name.pdcurses,
      python = self.build_dir / src_name.python,
      xmake = self.build_dir / src_name.xmake,
      z = self.build_dir / src_name.z,
    )

    self.src_arx = SourcePaths(
      binutils = self.assets_dir / f'{src_name.binutils}.tar.zst'
        if Version(ver.binutils) >= Version('2.43')
        else self.assets_dir / f'{src_name.binutils}.tar.xz',
      expat = self.assets_dir / f'{src_name.expat}.tar.xz',
      gcc = self.assets_dir / f'{src_name.gcc}.tar.xz',
      gdb = self.assets_dir / f'{src_name.gdb}.tar.xz',
      gettext = self.assets_dir / f'{src_name.gettext}.tar.xz'
        if ver.gettext
        else None,
      gmp = self.assets_dir / f'{src_name.gmp}.tar.zst',
      iconv = self.assets_dir / f'{src_name.iconv}.tar.gz',
      make = self.assets_dir / f'{src_name.make}.tar.lz',
      mcfgthread = self.assets_dir / f'{src_name.mcfgthread}.tar.gz'
        if ver.thread == 'mcf'
        else None,
      mingw_host = self.assets_dir / f'{src_name.mingw_host}.tar.bz2',
      mingw_target = self.assets_dir / f'{src_name.mingw_target}.tar.bz2',
      mpc = self.assets_dir / f'{src_name.mpc}.tar.gz',
      mpfr = self.assets_dir / f'{src_name.mpfr}.tar.xz',
      pdcurses = self.assets_dir / f'{src_name.pdcurses}.tar.gz',
      python = self.assets_dir / f'{src_name.python}.tar.xz',
      xmake = self.assets_dir / f'{src_name.xmake}.tar.gz',
      z = self.assets_dir / f'{src_name.z}.tar.gz',
    )

    layer_AAA_prefix = self.layer_dir / 'AAA'
    self.layer_AAA = LayerPathsAAA(
      prefix = layer_AAA_prefix,

      gmp = layer_AAA_prefix / 'gmp',
      mpc = layer_AAA_prefix / 'mpc',
      mpfr = layer_AAA_prefix / 'mpfr',
      z = layer_AAA_prefix / 'z',

      python = layer_AAA_prefix / 'python',
      xmake = layer_AAA_prefix / 'xmake',
    )

    layer_AAB_prefix = self.layer_dir / 'AAB'
    self.layer_AAB = LayerPathsAAB(
      prefix = layer_AAB_prefix,

      binutils = layer_AAB_prefix / 'binutils',
      crt = layer_AAB_prefix / 'crt',
      gcc = layer_AAB_prefix / 'gcc',
      headers = layer_AAB_prefix / 'headers',

      expat = layer_AAB_prefix / 'expat',
      gettext = layer_AAB_prefix / 'gettext',
      gmp = layer_AAB_prefix / 'gmp',
      iconv = layer_AAB_prefix / 'iconv',
      mpc = layer_AAB_prefix / 'mpc',
      mpfr = layer_AAB_prefix / 'mpfr',
      pdcurses = layer_AAB_prefix / 'pdcurses',
      python = layer_AAB_prefix / 'python',
    )

    layer_ABB_prefix = self.layer_dir / 'ABB'
    self.layer_ABB = LayerPathsABB(
      prefix = layer_ABB_prefix,

      binutils = layer_ABB_prefix / 'binutils',
      crt = layer_ABB_prefix / 'crt',
      gcc = layer_ABB_prefix / 'gcc',
      gdb = layer_ABB_prefix / 'gdb',
      headers = layer_ABB_prefix / 'headers',
      make = layer_ABB_prefix / 'make',
      xmake = layer_ABB_prefix / 'xmake',

      license = layer_ABB_prefix / 'license',
    )

    # test phase

    self.test_dir = Path(f'{tempfile.gettempdir()}/{dir}')
    self.test_src_dir = self.root_dir / 'support' / 'test'

    self.test_mingw_dir = self.test_dir / dir

    # target test archive phase

    self.test_archive_dir = self.root_dir / 'pkg' / f'test-archive-{config.profile}' / str(config.branch)
    self.test_archive_mingw_dir = self.test_archive_dir / dir
