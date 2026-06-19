import argparse
from packaging.version import Version
from pathlib import Path
import tempfile
from typing import NamedTuple, Optional

from module.profile import BranchProfile

class SourcePaths(NamedTuple):
  binutils: Path
  expat: Path
  gcc: Path
  gdb: Path
  gmp: Path
  iconv: Path
  isl: Path
  make: Path
  mcfgthread: Path
  meson: Path
  mingw: Path
  mpc: Path
  mpfr: Path
  nowide: Path
  pdcurses: Path
  pkgconf: Path
  python: Path
  setuptools: Path
  xmake: Path
  zlib_net: Path
  zstd: Path

class InTreeSourcePaths(NamedTuple):
  iconv: Path
  intl: Path
  sync: Path
  thunk: Path
  wrapper: Path

class LayerPathsAAA(NamedTuple):
  prefix: Path

  gmp: Path
  isl: Path
  mpc: Path
  mpfr: Path
  zlib: Path
  zstd: Path

  meson: Path
  python: Path
  setuptools: Path
  wrapper: Path
  xmake: Path

class LayerPathsAAB(NamedTuple):
  prefix: Path

  binutils: Path
  bootstrap: Path
  crt_base: Path
  crt_host: Path
  crt_target: Path
  crt_shared: Path
  gcc: Path
  gcc_lib: Path
  gcc_lib_shared: Path
  headers: Path
  mcfgthread: Path
  mcfgthread_shared: Path
  thunk_host: Path
  thunk_target: Path
  utf8: Path
  winpthreads: Path
  winpthreads_shared: Path
  zlib: Path
  zstd: Path

  expat: Path
  gmp: Path
  iconv: Path
  intl: Path
  isl: Path
  mpc: Path
  mpfr: Path
  pdcurses: Path
  python: Path

class LayerPathsABB(NamedTuple):
  prefix: Path

  test_driver: Path

  binutils: Path
  crt0: Path
  crt: Path
  crt_shared: Path
  gcc: Path
  gcc_lib: Path
  gcc_lib_shared: Path
  gdb: Path
  headers: Path
  make: Path
  mcfgthread: Path
  mcfgthread_shared: Path
  nowide: Path
  nowide_shared: Path
  pkgconf: Path
  thunk: Path
  winpthreads: Path
  winpthreads_shared: Path
  xmake: Path

class ProjectPaths:
  root_dir: Path
  abi_name: str
  shared_dir: str

  assets_dir: Path
  dist_dir: Path
  patch_dir: Path

  cmake_cross_file: Path
  meson_cross_file: Path

  test_driver_pkg: Path
  mingw_pkg: Path
  xmake_pkg: Path
  cross_pkg: Path

  # build phase

  bootstrap_src_dir: Path
  utf8_src_dir: Path
  build_dir: Path
  layer_dir: Path
  pkg_dir: Path

  src_dir: SourcePaths
  src_arx: SourcePaths

  in_tree_src_dir: InTreeSourcePaths
  in_tree_src_tree: InTreeSourcePaths

  layer_AAA: LayerPathsAAA
  layer_AAB: LayerPathsAAB
  layer_ABB: LayerPathsABB

  # test phase

  test_dir: Path
  test_src_dir: Path

  test_mingw_dir: Path

  # target test archive phase

  sat_dir: Path
  sat_mingw_dir: Path

  def __init__(
    self,
    config: argparse.Namespace,
    ver: BranchProfile,
  ):
    self.root_dir = Path.cwd()
    if config.abi_name:
      abi_name = config.abi_name
      if config.pkg_prefix:
        pkg_id = f'{config.pkg_prefix}+ml-{ver.gcc}-r{ver.rev}'
      else:
        pkg_id = f'{config.abi_name}+ml-{ver.gcc}-r{ver.rev}'
    else:
      abi_name = f'mingw{config.profile}-{config.branch}'
      pkg_id = f'mingw{config.profile}-{ver.display_version or ver.gcc}-r{ver.rev}'
    self.abi_name = abi_name

    if ver.abi_frozen and ver.thunk_free:
      shared_dir = 'lib/shared'
    else:
      shared_dir = 'lib/shared-unstable'
    self.shared_dir = shared_dir

    self.assets_dir = self.root_dir / 'assets'
    self.dist_dir = self.root_dir / 'dist'
    self.patch_dir = self.root_dir / 'patch'

    self.cmake_cross_file = self.root_dir / f'support/cmake/{ver.target}.cmake'
    self.meson_cross_file = self.root_dir / f'support/meson/{ver.target}.txt'

    self.test_driver_pkg = self.dist_dir / f'test-{pkg_id}.tar.zst'
    self.mingw_pkg = self.dist_dir / f'{pkg_id}.tar.zst'
    self.xmake_pkg = self.dist_dir / f'xmake-{pkg_id}.tar.zst'
    self.cross_pkg = self.dist_dir / f'x-{pkg_id}.tar.zst'

    # build phase

    self.bootstrap_src_dir = self.root_dir / 'support/bootstrap'
    self.utf8_src_dir = self.root_dir / 'support/utf8'
    self.build_dir = Path(f'/tmp/build/{abi_name}')
    self.layer_dir = Path(f'/tmp/layer/{abi_name}')
    self.pkg_dir = Path(f'/tmp/pkg/{abi_name}')

    src_name = SourcePaths(
      binutils = Path(f'binutils-{ver.binutils}'),
      expat = Path(f'expat-{ver.expat}'),
      gcc = Path(f'gcc-{ver.gcc}'),
      gdb = Path(f'gdb-{ver.gdb}'),
      gmp = Path(f'gmp-{ver.gmp}'),
      iconv = Path(f'libiconv-{ver.iconv}'),
      isl = Path(f'isl-{ver.isl}'),
      make = Path(f'make-{ver.make}'),
      mcfgthread = Path(f'mcfgthread-{ver.mcfgthread}'),
      meson = Path(f'meson-{ver.meson}'),
      mingw = Path(f'mingw-w64-v{ver.mingw}'),
      mpc = Path(f'mpc-{ver.mpc}'),
      mpfr = Path(f'mpfr-{ver.mpfr}'),
      nowide = Path(f'nowide_standalone_v{ver.nowide}'),
      pdcurses = Path(f'PDCurses-{ver.pdcurses}'),
      pkgconf = Path(f'pkgconf-pkgconf-{ver.pkgconf}'),
      python = Path(f'Python-{ver.python}'),
      setuptools = Path(f'setuptools-{ver.setuptools}'),
      xmake = Path(f'xmake-{ver.xmake}'),
      zlib_net = Path(f'zlib-{ver.zlib_net}'),
      zstd = Path(f'zstd-{ver.zstd}'),
    )

    self.src_dir = SourcePaths(
      binutils = self.build_dir / src_name.binutils,
      expat = self.build_dir / src_name.expat,
      gcc = self.build_dir / src_name.gcc,
      gdb = self.build_dir / src_name.gdb,
      gmp = self.build_dir / src_name.gmp,
      iconv = self.build_dir / src_name.iconv,
      isl = self.build_dir / src_name.isl,
      make = self.build_dir / src_name.make,
      mcfgthread = self.build_dir / src_name.mcfgthread,
      meson = self.build_dir / src_name.meson,
      mingw = self.build_dir / src_name.mingw,
      mpc = self.build_dir / src_name.mpc,
      mpfr = self.build_dir / src_name.mpfr,
      nowide = self.build_dir / src_name.nowide,
      pdcurses = self.build_dir / src_name.pdcurses,
      pkgconf = self.build_dir / src_name.pkgconf,
      python = self.build_dir / src_name.python,
      setuptools = self.build_dir / src_name.setuptools,
      xmake = self.build_dir / src_name.xmake,
      zlib_net = self.build_dir / src_name.zlib_net,
      zstd = self.build_dir / src_name.zstd,
    )

    self.src_arx = SourcePaths(
      binutils = self.assets_dir / f'{src_name.binutils}.tar.zst'
        if Version(ver.binutils) >= Version('2.43')
        else self.assets_dir / f'{src_name.binutils}.tar.xz',
      expat = self.assets_dir / f'{src_name.expat}.tar.xz',
      gcc = self.assets_dir / f'{src_name.gcc}.tar.xz',
      gdb = self.assets_dir / f'{src_name.gdb}.tar.xz',
      gmp = self.assets_dir / f'{src_name.gmp}.tar.zst',
      iconv = self.assets_dir / f'{src_name.iconv}.tar.gz',
      isl = self.assets_dir / f'{src_name.isl}.tar.xz',
      make = self.assets_dir / f'{src_name.make}.tar.lz',
      mcfgthread = self.assets_dir / f'{src_name.mcfgthread}.tar.gz',
      meson = self.assets_dir / f'{src_name.meson}.tar.gz',
      mingw = self.assets_dir / f'{src_name.mingw}.tar.bz2',
      mpc = self.assets_dir / f'{src_name.mpc}.tar.xz'
        if Version(ver.mpc) >= Version('1.4.0')
        else self.assets_dir / f'{src_name.mpc}.tar.gz',
      mpfr = self.assets_dir / f'{src_name.mpfr}.tar.xz',
      nowide = self.assets_dir / f'{src_name.nowide}.tar.gz',
      pdcurses = self.assets_dir / f'{src_name.pdcurses}.tar.gz',
      python = self.assets_dir / f'{src_name.python}.tar.xz',
      pkgconf = self.assets_dir / f'{src_name.pkgconf}.tar.gz',
      setuptools = self.assets_dir / f'{src_name.setuptools}.tar.gz',
      xmake = self.assets_dir / f'{src_name.xmake}.tar.gz',
      zlib_net = self.assets_dir / f'{src_name.zlib_net}.tar.gz',
      zstd = self.assets_dir / f'{src_name.zstd}.tar.zst',
    )

    self.in_tree_src_dir = InTreeSourcePaths(
      iconv = self.build_dir / 'iconv',
      intl = self.build_dir / 'intl',
      sync = self.build_dir / 'sync',
      thunk = self.build_dir / 'thunk',
      wrapper = self.build_dir / 'wrapper',
    )

    self.in_tree_src_tree = InTreeSourcePaths(
      iconv = self.root_dir / 'support/iconv',
      intl = self.root_dir / 'support/intl',
      sync = self.root_dir / 'support/sync',
      thunk = self.root_dir / 'support/thunk',
      wrapper = self.root_dir / 'support/wrapper',
    )

    layer_AAA_prefix = self.layer_dir / 'AAA'
    self.layer_AAA = LayerPathsAAA(
      prefix = layer_AAA_prefix,

      gmp = layer_AAA_prefix / 'gmp',
      isl = layer_AAA_prefix / 'isl',
      mpc = layer_AAA_prefix / 'mpc',
      mpfr = layer_AAA_prefix / 'mpfr',
      zlib = layer_AAA_prefix / 'zlib',
      zstd = layer_AAA_prefix / 'zstd',

      meson = layer_AAA_prefix / 'meson',
      python = layer_AAA_prefix / 'python',
      setuptools = layer_AAA_prefix / 'setuptools',
      wrapper = layer_AAA_prefix / 'wrapper',
      xmake = layer_AAA_prefix / 'xmake',
    )

    layer_AAB_prefix = self.layer_dir / 'AAB'
    self.layer_AAB = LayerPathsAAB(
      prefix = layer_AAB_prefix,

      binutils = layer_AAB_prefix / 'binutils',
      bootstrap = layer_AAB_prefix / 'bootstrap',
      crt_base = layer_AAB_prefix / 'crt-base',
      crt_host = layer_AAB_prefix / 'crt-host',
      crt_target = layer_AAB_prefix / 'crt-target',
      crt_shared = layer_AAB_prefix / 'crt-shared',
      gcc = layer_AAB_prefix / 'gcc',
      gcc_lib = layer_AAB_prefix / 'gcc-lib',
      gcc_lib_shared = layer_AAB_prefix / 'gcc-lib-shared',
      mcfgthread = layer_AAB_prefix / 'mcfgthread',
      mcfgthread_shared = layer_AAB_prefix / 'mcfgthread-shared',
      headers = layer_AAB_prefix / 'headers',
      thunk_host = layer_AAB_prefix / 'thunk-host',
      thunk_target = layer_AAB_prefix / 'thunk-target',
      utf8 = layer_AAB_prefix / 'utf8',
      winpthreads = layer_AAB_prefix / 'winpthreads',
      winpthreads_shared = layer_AAB_prefix / 'winpthreads-shared',
      zlib = layer_AAB_prefix / 'zlib',
      zstd = layer_AAB_prefix / 'zstd',

      expat = layer_AAB_prefix / 'expat',
      gmp = layer_AAB_prefix / 'gmp',
      iconv = layer_AAB_prefix / 'iconv',
      intl = layer_AAB_prefix / 'intl',
      isl = layer_AAB_prefix / 'isl',
      mpc = layer_AAB_prefix / 'mpc',
      mpfr = layer_AAB_prefix / 'mpfr',
      pdcurses = layer_AAB_prefix / 'pdcurses',
      python = layer_AAB_prefix / 'python',
    )

    layer_ABB_prefix = self.layer_dir / 'ABB'
    self.layer_ABB = LayerPathsABB(
      prefix = layer_ABB_prefix,

      test_driver = layer_ABB_prefix / 'test-driver',

      binutils = layer_ABB_prefix / 'binutils',
      crt0 = layer_ABB_prefix / 'crt0',
      crt = layer_ABB_prefix / 'crt',
      crt_shared = layer_ABB_prefix / 'crt' / shared_dir,
      gcc = layer_ABB_prefix / 'gcc',
      gcc_lib = layer_ABB_prefix / 'gcc-lib',
      gcc_lib_shared = layer_ABB_prefix / 'gcc-lib' / shared_dir,
      gdb = layer_ABB_prefix / 'gdb',
      headers = layer_ABB_prefix / 'headers',
      make = layer_ABB_prefix / 'make',
      mcfgthread = layer_ABB_prefix / 'mcfgthread',
      mcfgthread_shared = layer_ABB_prefix / 'mcfgthread' / shared_dir,
      nowide = layer_ABB_prefix / 'nowide',
      nowide_shared = layer_ABB_prefix / 'nowide' / shared_dir,
      pkgconf = layer_ABB_prefix / 'pkgconf',
      thunk = layer_ABB_prefix / 'thunk',
      winpthreads = layer_ABB_prefix / 'winpthreads',
      winpthreads_shared = layer_ABB_prefix / 'winpthreads' / shared_dir,
      xmake = layer_ABB_prefix / 'xmake',
    )

    # test phase

    self.test_dir = Path(f'{tempfile.gettempdir()}/{abi_name}-नमस्ते')
    self.test_src_dir = self.root_dir / 'support' / 'test'

    self.test_mingw_dir = self.test_dir / abi_name

    # target semi-automated testing archive phase

    if ver.utf8_thunk and ver.min_os >= Version('4.0'):
      self.sat_dir = self.root_dir / 'pkg' / f'sat{config.profile}-{config.branch}-नमस्ते'
    else:
      self.sat_dir = self.root_dir / 'pkg' / f'sat{config.profile}-{config.branch}-‘hello’'
    self.sat_mingw_dir = self.sat_dir / abi_name
