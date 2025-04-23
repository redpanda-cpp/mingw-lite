from hashlib import sha256
import logging
import os
from packaging.version import Version
from pathlib import Path
import shutil
import subprocess
from urllib.error import URLError
from urllib.request import urlopen

from module.fetch import validate_and_download, check_and_extract
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

def _patch_done(path: Path):
  mark = path / '.patched'
  mark.touch()

def _binutils(ver: BranchProfile, paths: ProjectPaths):
  url = f'https://ftpmirror.gnu.org/gnu/binutils/{paths.binutils_arx.name}'
  validate_and_download(paths.binutils_arx, url)
  if check_and_extract(paths.binutils, paths.binutils_arx):
    v = Version(ver.binutils)

    # Fix path corruption
    if v >= Version('2.43'):
      _patch(paths.binutils, paths.patch / 'binutils' / 'fix-path-corruption_2.43.patch')
    else:
      _patch(paths.binutils, paths.patch / 'binutils' / 'fix-path-corruption_2.41.patch')

    _patch_done(paths.binutils)

def _expat(ver: BranchProfile, paths: ProjectPaths):
  v = Version(ver.expat)
  tag = f'R_{v.major}_{v.minor}_{v.micro}'
  url = f'https://github.com/libexpat/libexpat/releases/download/{tag}/{paths.expat_arx.name}'
  validate_and_download(paths.expat_arx, url)
  check_and_extract(paths.expat, paths.expat_arx)
  _patch_done(paths.expat)

def _gcc(ver: BranchProfile, paths: ProjectPaths):
  v = Version(ver.gcc)
  if v.major >= 15:
    url = f'https://gcc.gnu.org/pub/gcc/snapshots/{ver.gcc}/{paths.gcc_arx.name}'
  else:
    url = f'https://ftpmirror.gnu.org/gnu/gcc/gcc-{ver.gcc}/{paths.gcc_arx.name}'
  validate_and_download(paths.gcc_arx, url)
  if check_and_extract(paths.gcc, paths.gcc_arx):
    # Fix make variable
    # - gcc 12 use `override CFLAGS +=` to handle PGO build, which breaks workaround for ucrt `access`
    if v.major >= 14:
      _patch(paths.gcc, paths.patch / 'gcc' / 'fix-make-variable_14.patch')
    else:
      _patch(paths.gcc, paths.patch / 'gcc' / 'fix-make-variable_12.patch')

    # Fix VT sequence
    _patch(paths.gcc, paths.patch / 'gcc' / 'fix-vt-seq.patch')

    # Fix locale directory
    _patch(paths.gcc, paths.patch / 'gcc' / 'fix-localedir.patch')

    # Fix libcpp setlocale
    # libcpp defines `setlocale` if `HAVE_SETLOCALE` not defined, but its configure.ac does not check `setlocale` at all
    _patch(paths.gcc, paths.patch / 'gcc' / 'fix-libcpp-setlocale.patch')

    # Parser-friendly diagnostics
    po_dir = paths.gcc / 'gcc' / 'po'
    po_files = list(po_dir.glob('*.po'))
    res = subprocess.run([
      'sed',
      '-iE',
      '/^msgid "(error|warning): "/,+1 d',
      *po_files
    ])
    if res.returncode != 0:
      message = 'Patch fail: applying gcc parser-friendly diagnostics'
      logging.critical(message)
      raise Exception(message)

    if ver.min_os.major >= 6:
      # Fix console code page
      _patch(paths.gcc, paths.patch / 'gcc' / 'fix-console-cp.patch')
    else:
      pass  # disable UTF-8 manifest later

    _patch_done(paths.gcc)

def _gdb(ver: BranchProfile, paths: ProjectPaths):
  url = f'https://ftpmirror.gnu.org/gnu/gdb/{paths.gdb_arx.name}'
  validate_and_download(paths.gdb_arx, url)
  if check_and_extract(paths.gdb, paths.gdb_arx):
    v = Version(ver.gdb)

    # Fix thread
    if v.major >= 16:
      _patch(paths.gdb, paths.patch / 'gdb' / 'fix-thread_16.patch')
    else:
      _patch(paths.gdb, paths.patch / 'gdb' / 'fix-thread_14.patch')

    # Fix iconv 'CP65001'
    _patch(paths.gdb, paths.patch / 'gdb' / 'fix-iconv-cp65001.patch')

    # Fix pythondir
    _patch(paths.gdb, paths.patch / 'gdb' / 'fix-pythondir.patch')

    _patch_done(paths.gdb)

def _gettext(ver: BranchProfile, paths: ProjectPaths):
  url = f'https://ftpmirror.gnu.org/gnu/gettext/{paths.gettext_arx.name}'
  validate_and_download(paths.gettext_arx, url)
  check_and_extract(paths.gettext, paths.gettext_arx)
  _patch_done(paths.gettext)

def _gmp(ver: BranchProfile, paths: ProjectPaths):
  url = f'https://ftpmirror.gnu.org/gnu/gmp/{paths.gmp_arx.name}'
  validate_and_download(paths.gmp_arx, url)
  check_and_extract(paths.gmp, paths.gmp_arx)
  _patch_done(paths.gmp)

def _iconv(ver: BranchProfile, paths: ProjectPaths):
  url = f'https://ftpmirror.gnu.org/gnu/libiconv/{paths.iconv_arx.name}'
  validate_and_download(paths.iconv_arx, url)
  check_and_extract(paths.iconv, paths.iconv_arx)
  _patch_done(paths.iconv)

def _make(ver: BranchProfile, paths: ProjectPaths):
  url = f'https://ftpmirror.gnu.org/gnu/make/{paths.make_arx.name}'
  validate_and_download(paths.make_arx, url)
  check_and_extract(paths.make, paths.make_arx)
  _patch_done(paths.make)

def _mcfgthread(ver: BranchProfile, paths: ProjectPaths):
  url = f'https://github.com/lhmouse/mcfgthread/archive/refs/tags/v{ver.mcfgthread}.tar.gz'
  validate_and_download(paths.mcfgthread_arx, url)
  check_and_extract(paths.mcfgthread, paths.mcfgthread_arx)
  _patch_done(paths.mcfgthread)

def _mingw_host(ver: BranchProfile, paths: ProjectPaths):
  url = f'https://downloads.sourceforge.net/project/mingw-w64/mingw-w64/mingw-w64-release/{paths.mingw_arx.name}'
  validate_and_download(paths.mingw_arx, url)
  if check_and_extract(paths.mingw_host, paths.mingw_arx):
    v = Version(ver.mingw)

    # CRT: Fix missing function
    if ver.min_os.major < 6:
      if v.major >= 12:
        _patch(paths.mingw_host, paths.patch / 'crt' / 'fix-missing-function_12.patch')
      else:
        _patch(paths.mingw_host, paths.patch / 'crt' / 'fix-missing-function_7.patch')

    # CRT: Add mingw thunks
    subprocess.run([
      './patch.py',
      paths.mingw_host,
      '-a', ver.arch,
      '--level', 'toolchain',
      '--nt-ver', str(ver.min_os),
    ], cwd = paths.root / 'thunk', check = True)

    _autoreconf(paths.mingw_host / 'mingw-w64-crt')
    _automake(paths.mingw_host / 'mingw-w64-crt')

    _patch_done(paths.mingw_host)

def _mingw_target(ver: BranchProfile, paths: ProjectPaths):
  url = f'https://downloads.sourceforge.net/project/mingw-w64/mingw-w64/mingw-w64-release/{paths.mingw_arx.name}'
  validate_and_download(paths.mingw_arx, url)
  if check_and_extract(paths.mingw_target, paths.mingw_arx):
    v = Version(ver.mingw)

    # CRT: Fix missing function
    if ver.min_os.major < 6:
      if v.major >= 12:
        _patch(paths.mingw_target, paths.patch / 'crt' / 'fix-missing-function_12.patch')
      else:
        _patch(paths.mingw_target, paths.patch / 'crt' / 'fix-missing-function_7.patch')

    # CRT: Add mingw thunks
    subprocess.run([
      './patch.py',
      paths.mingw_target,
      '-a', ver.arch,
      '--level', 'core',
      '--nt-ver', str(ver.min_os),
    ], cwd = paths.root / 'thunk', check = True)

    _autoreconf(paths.mingw_target / 'mingw-w64-crt')
    _automake(paths.mingw_target / 'mingw-w64-crt')

    _patch_done(paths.mingw_target)

def _mpc(ver: BranchProfile, paths: ProjectPaths):
  url = f'https://ftpmirror.gnu.org/gnu/mpc/{paths.mpc_arx.name}'
  validate_and_download(paths.mpc_arx, url)
  check_and_extract(paths.mpc, paths.mpc_arx)
  _patch_done(paths.mpc)

def _mpfr(ver: BranchProfile, paths: ProjectPaths):
  url = f'https://ftpmirror.gnu.org/gnu/mpfr/{paths.mpfr_arx.name}'
  validate_and_download(paths.mpfr_arx, url)
  check_and_extract(paths.mpfr, paths.mpfr_arx)
  _patch_done(paths.mpfr)

def _python(ver: BranchProfile, paths: ProjectPaths):
  url = f'https://www.python.org/ftp/python/{ver.python}/{paths.python_arx.name}'
  z_url = f'https://zlib.net/fossils/{paths.python_z_arx.name}'
  validate_and_download(paths.python_arx, url)
  validate_and_download(paths.python_z_arx, z_url)
  if check_and_extract(paths.python, paths.python_arx):
    ver = Version(ver.python)

    # Disable xxlimited shared library if `--disable-test-modules`
    if ver >= Version('3.12') and ver < Version('3.13'):
      _patch(paths.python, paths.patch / 'python' / 'disable-shared-xxlimited_3.12.patch')

    # Alternative build system
    check_and_extract(paths.python_z, paths.python_z_arx)
    os.symlink(paths.python_z, paths.python / 'zlib', target_is_directory = True)
    if ver >= Version('3.13'):
      shutil.copy(paths.patch / 'python' / 'xmake_3.13.lua', paths.python / 'xmake.lua')
      _patch(paths.python, paths.patch / 'python' / 'fix-mingw-build_3.13.patch')
    else:
      shutil.copy(paths.patch / 'python' / 'xmake_3.12.lua', paths.python / 'xmake.lua')
      _patch(paths.python, paths.patch / 'python' / 'fix-mingw-build_3.12.patch')
    shutil.copy(paths.patch / 'python' / 'python-config.sh', paths.python / 'python-config.sh')

    _patch_done(paths.python)

def _xmake(ver: BranchProfile, paths: ProjectPaths):
  url = f'https://github.com/xmake-io/xmake/releases/download/v{ver.xmake}/{paths.xmake_arx.name}'
  validate_and_download(paths.xmake_arx, url)
  if check_and_extract(paths.xmake, paths.xmake_arx):
    # disable werror
    xmake_lua = paths.xmake / 'core' / 'xmake.lua'
    with open(xmake_lua, 'r') as f:
      xmake_lua_content = f.readlines()
    with open(xmake_lua, 'w') as f:
      for line in xmake_lua_content:
        if line.startswith('set_warnings'):
          f.write('set_warnings("all")\n')
        else:
          f.write(line)

    # Fix module mapper path
    _patch(paths.xmake, paths.patch / 'xmake' / 'fix-module-mapper-path.patch')

    _patch_done(paths.xmake)

def prepare_source(ver: BranchProfile, paths: ProjectPaths):
  _binutils(ver, paths)
  _expat(ver, paths)
  _gcc(ver, paths)
  _gdb(ver, paths)
  if ver.gettext:
    _gettext(ver, paths)
  _gmp(ver, paths)
  _iconv(ver, paths)
  _make(ver, paths)
  if ver.thread == 'mcf':
    _mcfgthread(ver, paths)
  _mingw_host(ver, paths)
  _mingw_target(ver, paths)
  _mpc(ver, paths)
  _mpfr(ver, paths)
  _python(ver, paths)
  _xmake(ver, paths)
