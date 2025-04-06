from hashlib import sha256
import logging
import os
from packaging.version import Version
from pathlib import Path
import shutil
import subprocess
from urllib.error import URLError
from urllib.request import urlopen

from module.checksum import CHECKSUMS
from module.path import ProjectPaths
from module.profile import BranchVersions, ProfileInfo

def _validate_and_download(path: Path, url: str):
  MAX_RETRY = 3
  checksum = CHECKSUMS[path.name]
  if path.exists():
    with open(path, 'rb') as f:
      body = f.read()
      if checksum != sha256(body).hexdigest():
        message = 'Validate fail: %s exists but checksum mismatch' % path.name
        logging.critical(message)
        logging.info('Please delete %s and try again' % path.name)
        raise Exception(message)
  else:
    logging.info('Downloading %s' % path.name)
    retry_count = 0
    while True:
      retry_count += 1
      try:
        response = urlopen(url)
        body = response.read()
        if checksum != sha256(body).hexdigest():
          message = 'Download fail: checksum mismatch for %s' % path.name
          logging.critical(message)
          raise Exception(message)
        with open(path, "wb") as f:
          f.write(body)
          return
      except URLError as e:
        message = 'Download fail: %s (retry %d/3)' % (e.reason, retry_count)
        if retry_count < MAX_RETRY:
          logging.warning(message)
          logging.warning('Retrying...')
        else:
          logging.critical(message)
          raise e

def _check_and_extract(path: Path, arx: Path):
  # check if already extracted
  if path.exists():
    mark = path / '.patched'
    if mark.exists():
      return False
    else:
      message = 'Extract fail: %s exists but not marked as fully patched' % path.name
      logging.critical(message)
      logging.info('Please delete %s and try again' % path.name)
      raise Exception(message)

  # extract
  res = subprocess.run([
    'bsdtar',
    '-xf',
    arx,
    '--no-same-owner',
  ], cwd = path.parent)
  if res.returncode != 0:
    message = 'Extract fail: bsdtar returned %d extracting %s' % (res.returncode, arx.name)
    logging.critical(message)
    raise Exception(message)

  return True

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

def _binutils(ver: str, info: ProfileInfo, paths: ProjectPaths):
  url = f'https://ftpmirror.gnu.org/gnu/binutils/{paths.binutils_arx.name}'
  _validate_and_download(paths.binutils_arx, url)
  if _check_and_extract(paths.binutils, paths.binutils_arx):
    v = Version(ver)

    # Fix path corruption
    if v >= Version('2.43'):
      _patch(paths.binutils, paths.patch / 'binutils' / 'fix-path-corruption_2.43.patch')
    else:
      _patch(paths.binutils, paths.patch / 'binutils' / 'fix-path-corruption_2.41.patch')

    _patch_done(paths.binutils)

def _gcc(ver: str, info: ProfileInfo, paths: ProjectPaths):
  v = Version(ver)
  if v.major >= 15:
    url = f'https://gcc.gnu.org/pub/gcc/snapshots/{ver}/{paths.gcc_arx.name}'
  else:
    url = f'https://ftpmirror.gnu.org/gnu/gcc/gcc-{ver}/{paths.gcc_arx.name}'
  _validate_and_download(paths.gcc_arx, url)
  if _check_and_extract(paths.gcc, paths.gcc_arx):
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

    if info.target_winnt >= 0x0600:
      # Fix console code page
      _patch(paths.gcc, paths.patch / 'gcc' / 'fix-console-cp.patch')
    else:
      pass  # disable UTF-8 manifest later

    _patch_done(paths.gcc)

def _gdb(ver: BranchVersions, info: ProfileInfo, paths: ProjectPaths):
  url = f'https://ftpmirror.gnu.org/gnu/gdb/{paths.gdb_arx.name}'
  _validate_and_download(paths.gdb_arx, url)
  if _check_and_extract(paths.gdb, paths.gdb_arx):
    v = Version(ver.gdb)

    # Fix thread
    if info.target_winnt <= 0x0600:
      _patch(paths.gdb, paths.patch / 'gdb' / 'fix-thread.patch')

    # Fix pythondir
    if ver.python:
      _patch(paths.gdb, paths.patch / 'gdb' / 'fix-pythondir.patch')

    _patch_done(paths.gdb)

def _gettext(ver: str, info: ProfileInfo, paths: ProjectPaths):
  url = f'https://ftpmirror.gnu.org/gnu/gettext/{paths.gettext_arx.name}'
  _validate_and_download(paths.gettext_arx, url)
  _check_and_extract(paths.gettext, paths.gettext_arx)
  _patch_done(paths.gettext)

def _gmp(ver: str, info: ProfileInfo, paths: ProjectPaths):
  url = f'https://ftpmirror.gnu.org/gnu/gmp/{paths.gmp_arx.name}'
  _validate_and_download(paths.gmp_arx, url)
  _check_and_extract(paths.gmp, paths.gmp_arx)
  _patch_done(paths.gmp)

def _iconv(ver: str, info: ProfileInfo, paths: ProjectPaths):
  url = f'https://ftpmirror.gnu.org/gnu/libiconv/{paths.iconv_arx.name}'
  _validate_and_download(paths.iconv_arx, url)
  _check_and_extract(paths.iconv, paths.iconv_arx)
  _patch_done(paths.iconv)

def _make(ver: str, info: ProfileInfo, paths: ProjectPaths):
  url = f'https://ftpmirror.gnu.org/gnu/make/{paths.make_arx.name}'
  _validate_and_download(paths.make_arx, url)
  _check_and_extract(paths.make, paths.make_arx)
  _patch_done(paths.make)

def _mcfgthread(ver: str, info: ProfileInfo, paths: ProjectPaths):
  url = f'https://github.com/lhmouse/mcfgthread/archive/refs/tags/v{ver}.tar.gz'
  _validate_and_download(paths.mcfgthread_arx, url)
  _check_and_extract(paths.mcfgthread, paths.mcfgthread_arx)
  _patch_done(paths.mcfgthread)

def _mingw(ver: str, info: ProfileInfo, paths: ProjectPaths):
  url = f'https://downloads.sourceforge.net/project/mingw-w64/mingw-w64/mingw-w64-release/{paths.mingw_arx.name}'
  _validate_and_download(paths.mingw_arx, url)
  if _check_and_extract(paths.mingw, paths.mingw_arx):
    v = Version(ver)

    # CRT: Fix missing function
    if info.target_winnt <= 0x0502:
      if v.major >= 12:
        _patch(paths.mingw, paths.patch / 'crt' / 'fix-missing-function_12.patch')
      else:
        _patch(paths.mingw, paths.patch / 'crt' / 'fix-missing-function_7.patch')

    # CRT: Add mingw thunks
    nt_ver = f'{info.target_winnt >> 8}.{info.target_winnt & 0xFF}'
    subprocess.run([
      './patch.py',
      paths.mingw,
      '-a', info.arch,
      '--nt-ver', nt_ver,
    ], cwd = paths.root / 'thunk', check = True)

    _autoreconf(paths.mingw / 'mingw-w64-crt')
    _automake(paths.mingw / 'mingw-w64-crt')

    _patch_done(paths.mingw)

def _mpc(ver: str, info: ProfileInfo, paths: ProjectPaths):
  url = f'https://ftpmirror.gnu.org/gnu/mpc/{paths.mpc_arx.name}'
  _validate_and_download(paths.mpc_arx, url)
  _check_and_extract(paths.mpc, paths.mpc_arx)
  _patch_done(paths.mpc)

def _mpfr(ver: str, info: ProfileInfo, paths: ProjectPaths):
  url = f'https://ftpmirror.gnu.org/gnu/mpfr/{paths.mpfr_arx.name}'
  _validate_and_download(paths.mpfr_arx, url)
  _check_and_extract(paths.mpfr, paths.mpfr_arx)
  _patch_done(paths.mpfr)

def _python(ver: BranchVersions, info: ProfileInfo, paths: ProjectPaths):
  url = f'https://www.python.org/ftp/python/{ver.python}/{paths.python_arx.name}'
  z_url = f'https://zlib.net/fossils/{paths.python_z_arx.name}'
  _validate_and_download(paths.python_arx, url)
  _validate_and_download(paths.python_z_arx, z_url)
  if _check_and_extract(paths.python, paths.python_arx):
    ver = Version(ver.python)

    # Disable xxlimited shared library if `--disable-test-modules`
    if ver >= Version('3.12') and ver < Version('3.13'):
      _patch(paths.python, paths.patch / 'python' / 'disable-shared-xxlimited_3.12.patch')

    # Alternative build system
    _check_and_extract(paths.python_z, paths.python_z_arx)
    os.symlink(paths.python_z, paths.python / 'zlib', target_is_directory = True)
    if ver >= Version('3.13'):
      shutil.copy(paths.patch / 'python' / 'xmake_3.13.lua', paths.python / 'xmake.lua')
      _patch(paths.python, paths.patch / 'python' / 'fix-mingw-build_3.13.patch')
    else:
      shutil.copy(paths.patch / 'python' / 'xmake_3.12.lua', paths.python / 'xmake.lua')
      _patch(paths.python, paths.patch / 'python' / 'fix-mingw-build_3.12.patch')
    shutil.copy(paths.patch / 'python' / 'python-config.sh', paths.python / 'python-config.sh')

    _patch_done(paths.python)

def download_and_patch(ver: BranchVersions, paths: ProjectPaths, info: ProfileInfo):
  _binutils(ver.binutils, info, paths)
  _gcc(ver.gcc, info, paths)
  _gdb(ver, info, paths)
  if ver.gettext:
    _gettext(ver.gettext, info, paths)
  _gmp(ver.gmp, info, paths)
  _iconv(ver.iconv, info, paths)
  _make(ver.make, info, paths)
  if ver.mcfgthread:
    _mcfgthread(ver.mcfgthread, info, paths)
  _mingw(ver.mingw, info, paths)
  _mpc(ver.mpc, info, paths)
  _mpfr(ver.mpfr, info, paths)
  if ver.python and info.target_winnt >= 0x0601:
    _python(ver, info, paths)
