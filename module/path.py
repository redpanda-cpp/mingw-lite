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
  z: Path

class InTreeSourcePaths(NamedTuple):
  atomic_bootstrap: Path
  gcc_lib_bootstrap: Path
  iconv: Path
  intl: Path
  sync: Path
  thunk: Path

class LayerPathsAAA(NamedTuple):
  prefix: Path

  gmp: Path
  mpc: Path
  mpfr: Path
  z: Path

  meson: Path
  python: Path
  setuptools: Path
  xmake: Path

class LayerPathsAAB(NamedTuple):
  prefix: Path

  atomic_bootstrap: Path
  binutils: Path
  crt_base: Path
  crt_host: Path
  crt_target: Path
  crt_shared: Path
  gcc: Path
  gcc_lib_bootstrap: Path
  gcc_lib: Path
  gcc_lib_shared: Path
  headers: Path
  mcfgthread: Path
  mcfgthread_shared: Path
  thunk_host: Path
  thunk_target: Path
  utf8: Path
  winpthreads_bootstrap: Path
  winpthreads: Path
  winpthreads_shared: Path

  expat: Path
  gmp: Path
  iconv: Path
  intl: Path
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

    self.utf8_src_dir = self.root_dir / 'support/utf8'
    self.build_dir = Path(f'/tmp/build/{abi_name}')
    self.layer_dir = Path(f'/tmp/layer/{abi_name}')
    self.pkg_dir = Path(f'/tmp/pkg/{abi_name}')

    src_name = SourcePaths(
      binutils = f'binutils-{ver.binutils}',
      expat = f'expat-{ver.expat}',
      gcc = f'gcc-{ver.gcc}',
      gdb = f'gdb-{ver.gdb}',
      gmp = f'gmp-{ver.gmp}',
      iconv = f'libiconv-{ver.iconv}',
      make = f'make-{ver.make}',
      mcfgthread = f'mcfgthread-{ver.mcfgthread}',
      meson = f'meson-{ver.meson}',
      mingw = f'mingw-w64-v{ver.mingw}',
      mpc = f'mpc-{ver.mpc}',
      mpfr = f'mpfr-{ver.mpfr}',
      nowide = f'nowide_standalone_v{ver.nowide}',
      pdcurses = f'PDCurses-{ver.pdcurses}',
      pkgconf = f'pkgconf-pkgconf-{ver.pkgconf}',
      python = f'Python-{ver.python}',
      setuptools = f'setuptools-{ver.setuptools}',
      xmake = f'xmake-{ver.xmake}',
      z = f'zlib-{ver.z}',
    )

    self.src_dir = SourcePaths(
      binutils = self.build_dir / src_name.binutils,
      expat = self.build_dir / src_name.expat,
      gcc = self.build_dir / src_name.gcc,
      gdb = self.build_dir / src_name.gdb,
      gmp = self.build_dir / src_name.gmp,
      iconv = self.build_dir / src_name.iconv,
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
      z = self.build_dir / src_name.z,
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
      z = self.assets_dir / f'{src_name.z}.tar.gz',
    )

    self.in_tree_src_dir = InTreeSourcePaths(
      atomic_bootstrap = self.build_dir / 'atomic-bootstrap',
      gcc_lib_bootstrap = self.build_dir / 'gcc-lib-bootstrap',
      iconv = self.build_dir / 'iconv',
      intl = self.build_dir / 'intl',
      sync = self.build_dir / 'sync',
      thunk = self.build_dir / 'thunk',
    )

    self.in_tree_src_tree = InTreeSourcePaths(
      atomic_bootstrap = self.root_dir / 'support/atomic-bootstrap',
      gcc_lib_bootstrap = self.root_dir / 'support/gcc-lib-bootstrap',
      iconv = self.root_dir / 'support/iconv',
      intl = self.root_dir / 'support/intl',
      sync = self.root_dir / 'support/sync',
      thunk = self.root_dir / 'support/thunk',
    )

    layer_AAA_prefix = self.layer_dir / 'AAA'
    self.layer_AAA = LayerPathsAAA(
      prefix = layer_AAA_prefix,

      gmp = layer_AAA_prefix / 'gmp',
      mpc = layer_AAA_prefix / 'mpc',
      mpfr = layer_AAA_prefix / 'mpfr',
      z = layer_AAA_prefix / 'z',

      meson = layer_AAA_prefix / 'meson',
      python = layer_AAA_prefix / 'python',
      setuptools = layer_AAA_prefix / 'setuptools',
      xmake = layer_AAA_prefix / 'xmake',
    )

    layer_AAB_prefix = self.layer_dir / 'AAB'
    self.layer_AAB = LayerPathsAAB(
      prefix = layer_AAB_prefix,

      atomic_bootstrap = layer_AAB_prefix / 'atomic-bootstrap',
      binutils = layer_AAB_prefix / 'binutils',
      crt_base = layer_AAB_prefix / 'crt-base',
      crt_host = layer_AAB_prefix / 'crt-host',
      crt_target = layer_AAB_prefix / 'crt-target',
      crt_shared = layer_AAB_prefix / 'crt-shared',
      gcc = layer_AAB_prefix / 'gcc',
      gcc_lib_bootstrap = layer_AAB_prefix / 'gcc-lib-bootstrap',
      gcc_lib = layer_AAB_prefix / 'gcc-lib',
      gcc_lib_shared = layer_AAB_prefix / 'gcc-lib-shared',
      mcfgthread = layer_AAB_prefix / 'mcfgthread',
      mcfgthread_shared = layer_AAB_prefix / 'mcfgthread-shared',
      headers = layer_AAB_prefix / 'headers',
      thunk_host = layer_AAB_prefix / 'thunk-host',
      thunk_target = layer_AAB_prefix / 'thunk-target',
      utf8 = layer_AAB_prefix / 'utf8',
      winpthreads_bootstrap = layer_AAB_prefix / 'winpthreads-bootstrap',
      winpthreads = layer_AAB_prefix / 'winpthreads',
      winpthreads_shared = layer_AAB_prefix / 'winpthreads-shared',

      expat = layer_AAB_prefix / 'expat',
      gmp = layer_AAB_prefix / 'gmp',
      iconv = layer_AAB_prefix / 'iconv',
      intl = layer_AAB_prefix / 'intl',
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
