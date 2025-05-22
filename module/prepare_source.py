from hashlib import sha256
import logging
import os
from packaging.version import Version
from pathlib import Path
import re
import shutil
import subprocess
from urllib.error import URLError
from urllib.request import urlopen

from module.fetch import validate_and_download, check_and_extract, patch_done
from module.path import ProjectPaths
from module.profile import BranchProfile

def _patch(path: Path, patch: Path):
  res = subprocess.run([
    'patch',
    '-Np1',
    '-i', patch,
  ], cwd = path)
  if res.returncode != 0:
    message = 'Patch fail: applying %s to %s' % (patch.name, path.name)
    logging.critical(message)
    raise Exception(message)

def _autoreconf(path: Path):
  res = subprocess.run([
    'autoreconf',
    '-fi',
  ], cwd = path)
  if res.returncode != 0:
    message = 'Autoreconf fail: %s' % path.name
    logging.critical(message)
    raise Exception(message)

def _automake(path: Path):
  res = subprocess.run([
    'automake',
  ], cwd = path)
  if res.returncode != 0:
    message = 'Automake fail: %s' % path.name
    logging.critical(message)
    raise Exception(message)

def _binutils(ver: BranchProfile, paths: ProjectPaths):
  url = f'https://ftpmirror.gnu.org/gnu/binutils/{paths.src_arx.binutils.name}'
  validate_and_download(paths.src_arx.binutils, url)
  if check_and_extract(paths.src_dir.binutils, paths.src_arx.binutils):
    v = Version(ver.binutils)

    # Fix path corruption
    if v >= Version('2.43'):
      _patch(paths.src_dir.binutils, paths.patch_dir / 'binutils' / 'fix-path-corruption_2.43.patch')
    else:
      _patch(paths.src_dir.binutils, paths.patch_dir / 'binutils' / 'fix-path-corruption_2.41.patch')

    patch_done(paths.src_dir.binutils)

def _expat(ver: BranchProfile, paths: ProjectPaths):
  v = Version(ver.expat)
  tag = f'R_{v.major}_{v.minor}_{v.micro}'
  url = f'https://github.com/libexpat/libexpat/releases/download/{tag}/{paths.src_arx.expat.name}'
  validate_and_download(paths.src_arx.expat, url)
  check_and_extract(paths.src_dir.expat, paths.src_arx.expat)
  patch_done(paths.src_dir.expat)

def _gcc(ver: BranchProfile, paths: ProjectPaths):
  v = Version(ver.gcc)

  is_snapshot = re.search(r'-\d{8}$', ver.gcc)
  if is_snapshot:
    url = f'https://gcc.gnu.org/pub/gcc/snapshots/{ver.gcc}/{paths.src_arx.gcc.name}'
  else:
    url = f'https://ftpmirror.gnu.org/gnu/gcc/gcc-{ver.gcc}/{paths.src_arx.gcc.name}'

  validate_and_download(paths.src_arx.gcc, url)
  if check_and_extract(paths.src_dir.gcc, paths.src_arx.gcc):
    # Fix make variable
    # - gcc 12 use `override CFLAGS +=` to handle PGO build, which breaks workaround for ucrt `access`
    if v.major >= 14:
      _patch(paths.src_dir.gcc, paths.patch_dir / 'gcc' / 'fix-make-variable_14.patch')
    else:
      _patch(paths.src_dir.gcc, paths.patch_dir / 'gcc' / 'fix-make-variable_12.patch')

    # Fix VT sequence
    _patch(paths.src_dir.gcc, paths.patch_dir / 'gcc' / 'fix-vt-seq.patch')

    # Fix UCRT pipe
    _patch(paths.src_dir.gcc, paths.patch_dir / 'gcc' / 'fix-ucrt-pipe.patch')

    # Fix libcpp setlocale
    # libcpp defines `setlocale` if `HAVE_SETLOCALE` not defined, but its configure.ac does not check `setlocale` at all
    _patch(paths.src_dir.gcc, paths.patch_dir / 'gcc' / 'fix-libcpp-setlocale.patch')

    # Parser-friendly diagnostics
    po_dir = paths.src_dir.gcc / 'gcc' / 'po'
    po_files = list(po_dir.glob('*.po'))
    res = subprocess.run([
      'sed',
      '-i', '-E',
      '/^msgid "(error|warning): "/,+1 d',
      *po_files
    ])
    if res.returncode != 0:
      message = 'Patch fail: applying gcc parser-friendly diagnostics'
      logging.critical(message)
      raise Exception(message)

    patch_done(paths.src_dir.gcc)

def _gdb(ver: BranchProfile, paths: ProjectPaths):
  url = f'https://ftpmirror.gnu.org/gnu/gdb/{paths.src_arx.gdb.name}'
  validate_and_download(paths.src_arx.gdb, url)
  if check_and_extract(paths.src_dir.gdb, paths.src_arx.gdb):
    v = Version(ver.gdb)

    # Fix thread
    if v.major >= 16:
      _patch(paths.src_dir.gdb, paths.patch_dir / 'gdb' / 'fix-thread_16.patch')
    else:
      _patch(paths.src_dir.gdb, paths.patch_dir / 'gdb' / 'fix-thread_14.patch')

    # Fix iconv 'CP65001'
    _patch(paths.src_dir.gdb, paths.patch_dir / 'gdb' / 'fix-iconv-cp65001.patch')

    # Fix pythondir
    _patch(paths.src_dir.gdb, paths.patch_dir / 'gdb' / 'fix-pythondir.patch')

    patch_done(paths.src_dir.gdb)

def _gmp(ver: BranchProfile, paths: ProjectPaths):
  url = f'https://ftpmirror.gnu.org/gnu/gmp/{paths.src_arx.gmp.name}'
  validate_and_download(paths.src_arx.gmp, url)
  check_and_extract(paths.src_dir.gmp, paths.src_arx.gmp)
  patch_done(paths.src_dir.gmp)

def _iconv(ver: BranchProfile, paths: ProjectPaths):
  url = f'https://ftpmirror.gnu.org/gnu/libiconv/{paths.src_arx.iconv.name}'
  validate_and_download(paths.src_arx.iconv, url)
  check_and_extract(paths.src_dir.iconv, paths.src_arx.iconv)
  patch_done(paths.src_dir.iconv)

def _intl(ver: BranchProfile, paths: ProjectPaths):
  shutil.copytree(paths.in_tree_src_tree.intl, paths.in_tree_src_dir.intl, dirs_exist_ok = True)

def _make(ver: BranchProfile, paths: ProjectPaths):
  url = f'https://ftpmirror.gnu.org/gnu/make/{paths.src_arx.make.name}'
  validate_and_download(paths.src_arx.make, url)
  check_and_extract(paths.src_dir.make, paths.src_arx.make)
  patch_done(paths.src_dir.make)

def _mcfgthread(ver: BranchProfile, paths: ProjectPaths):
  url = f'https://github.com/lhmouse/mcfgthread/archive/refs/tags/v{ver.mcfgthread}.tar.gz'
  validate_and_download(paths.src_arx.mcfgthread, url)
  check_and_extract(paths.src_dir.mcfgthread, paths.src_arx.mcfgthread)
  patch_done(paths.src_dir.mcfgthread)

def _mingw_host(ver: BranchProfile, paths: ProjectPaths):
  url = f'https://downloads.sourceforge.net/project/mingw-w64/mingw-w64/mingw-w64-release/{paths.src_arx.mingw_host.name}'
  validate_and_download(paths.src_arx.mingw_host, url)
  if check_and_extract(paths.src_dir.mingw_host, paths.src_arx.mingw_host):
    v = Version(ver.mingw)

    # CRT: Fix missing function
    if ver.min_os.major < 6:
      if v.major >= 12:
        _patch(paths.src_dir.mingw_host, paths.patch_dir / 'crt' / 'fix-missing-function_12.patch')
      else:
        _patch(paths.src_dir.mingw_host, paths.patch_dir / 'crt' / 'fix-missing-function_7.patch')

    # CRT: Add mingw thunks
    subprocess.run([
      './patch.py',
      paths.src_dir.mingw_host,
      '-a', ver.arch,
      '--level', 'toolchain',
      '--nt-ver', str(ver.min_os),
    ], cwd = paths.root_dir / 'thunk', check = True)

    _autoreconf(paths.src_dir.mingw_host / 'mingw-w64-crt')
    _automake(paths.src_dir.mingw_host / 'mingw-w64-crt')

    patch_done(paths.src_dir.mingw_host)

def _mingw_target(ver: BranchProfile, paths: ProjectPaths):
  url = f'https://downloads.sourceforge.net/project/mingw-w64/mingw-w64/mingw-w64-release/{paths.src_arx.mingw_target.name}'
  validate_and_download(paths.src_arx.mingw_target, url)
  if check_and_extract(paths.src_dir.mingw_target, paths.src_arx.mingw_target):
    v = Version(ver.mingw)

    # CRT: Fix missing function
    if ver.min_os.major < 6:
      if v.major >= 12:
        _patch(paths.src_dir.mingw_target, paths.patch_dir / 'crt' / 'fix-missing-function_12.patch')
      else:
        _patch(paths.src_dir.mingw_target, paths.patch_dir / 'crt' / 'fix-missing-function_7.patch')

    # CRT: Add mingw thunks
    subprocess.run([
      './patch.py',
      paths.src_dir.mingw_target,
      '-a', ver.arch,
      '--level', 'core',
      '--nt-ver', str(ver.min_os),
    ], cwd = paths.root_dir / 'thunk', check = True)

    _autoreconf(paths.src_dir.mingw_target / 'mingw-w64-crt')
    _automake(paths.src_dir.mingw_target / 'mingw-w64-crt')

    patch_done(paths.src_dir.mingw_target)

def _mpc(ver: BranchProfile, paths: ProjectPaths):
  url = f'https://ftpmirror.gnu.org/gnu/mpc/{paths.src_arx.mpc.name}'
  validate_and_download(paths.src_arx.mpc, url)
  check_and_extract(paths.src_dir.mpc, paths.src_arx.mpc)
  patch_done(paths.src_dir.mpc)

def _mpfr(ver: BranchProfile, paths: ProjectPaths):
  url = f'https://ftpmirror.gnu.org/gnu/mpfr/{paths.src_arx.mpfr.name}'
  validate_and_download(paths.src_arx.mpfr, url)
  check_and_extract(paths.src_dir.mpfr, paths.src_arx.mpfr)
  patch_done(paths.src_dir.mpfr)

def _pdcurses(ver: BranchProfile, paths: ProjectPaths):
  url = f'https://github.com/wmcbrine/PDCurses/archive/refs/tags/{ver.pdcurses}.tar.gz'
  validate_and_download(paths.src_arx.pdcurses, url)
  check_and_extract(paths.src_dir.pdcurses, paths.src_arx.pdcurses)
  patch_done(paths.src_dir.pdcurses)

def _python(ver: BranchProfile, paths: ProjectPaths):
  url = f'https://www.python.org/ftp/python/{ver.python}/{paths.src_arx.python.name}'
  validate_and_download(paths.src_arx.python, url)
  if check_and_extract(paths.src_dir.python, paths.src_arx.python):
    ver = Version(ver.python)

    # Disable xxlimited shared library if `--disable-test-modules`
    if ver >= Version('3.12') and ver < Version('3.13'):
      _patch(paths.src_dir.python, paths.patch_dir / 'python' / 'disable-shared-xxlimited_3.12.patch')

    # Fix thread touch last error
    # https://github.com/python/cpython/pull/104531
    if ver >= Version('3.12') and ver < Version('3.13'):
      _patch(paths.src_dir.python, paths.patch_dir / 'python' / 'fix-thread-touch-last-error_3.12.patch')

    # Alternative build system
    os.symlink(paths.src_dir.z, paths.src_dir.python / 'zlib', target_is_directory = True)
    if ver >= Version('3.13'):
      shutil.copy(paths.patch_dir / 'python' / 'xmake_3.13.lua', paths.src_dir.python / 'xmake.lua')
      _patch(paths.src_dir.python, paths.patch_dir / 'python' / 'fix-mingw-build_3.13.patch')
    else:
      shutil.copy(paths.patch_dir / 'python' / 'xmake_3.12.lua', paths.src_dir.python / 'xmake.lua')
      _patch(paths.src_dir.python, paths.patch_dir / 'python' / 'fix-mingw-build_3.12.patch')
    shutil.copy(paths.patch_dir / 'python' / 'python-config.sh', paths.src_dir.python / 'python-config.sh')

    patch_done(paths.src_dir.python)

def _xmake(ver: BranchProfile, paths: ProjectPaths):
  release_name = paths.src_arx.xmake.name.replace('xmake-', 'xmake-v')
  url = f'https://github.com/xmake-io/xmake/releases/download/v{ver.xmake}/{release_name}'
  validate_and_download(paths.src_arx.xmake, url)
  if check_and_extract(paths.src_dir.xmake, paths.src_arx.xmake):
    # disable werror
    xmake_lua = paths.src_dir.xmake / 'core' / 'xmake.lua'
    with open(xmake_lua, 'r') as f:
      xmake_lua_content = f.readlines()
    with open(xmake_lua, 'w') as f:
      for line in xmake_lua_content:
        if line.startswith('set_warnings'):
          f.write('set_warnings("all")\n')
        else:
          f.write(line)

    # Fix module mapper path
    _patch(paths.src_dir.xmake, paths.patch_dir / 'xmake' / 'fix-module-mapper-path.patch')

    patch_done(paths.src_dir.xmake)

def _z(ver: BranchProfile, paths: ProjectPaths):
  url = f'https://zlib.net/fossils/{paths.src_arx.z.name}'
  validate_and_download(paths.src_arx.z, url)
  check_and_extract(paths.src_dir.z, paths.src_arx.z)
  patch_done(paths.src_dir.z)

def prepare_source(ver: BranchProfile, paths: ProjectPaths):
  _binutils(ver, paths)
  _expat(ver, paths)
  _gcc(ver, paths)
  _gdb(ver, paths)
  _gmp(ver, paths)
  _iconv(ver, paths)
  _intl(ver, paths)
  _make(ver, paths)
  if ver.thread == 'mcf':
    _mcfgthread(ver, paths)
  _mingw_host(ver, paths)
  _mingw_target(ver, paths)
  _mpc(ver, paths)
  _mpfr(ver, paths)
  _pdcurses(ver, paths)
  _python(ver, paths)
  _xmake(ver, paths)
  _z(ver, paths)
