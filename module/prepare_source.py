from hashlib import sha256
import logging
from packaging.version import Version
from pathlib import Path
from shutil import copyfile
import subprocess
from urllib.request import urlopen

from module.checksum import CHECKSUMS
from module.path import ProjectPaths
from module.profile import BranchVersions, ProfileInfo

def _validate_and_download(path: Path, url: str):
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
    with urlopen(url) as response:
      body = response.read()
      if checksum != sha256(body).hexdigest():
        message = 'Download fail: checksum mismatch for %s' % path.name
        logging.critical(message)
        raise Exception(message)
      with open(path, "wb") as f:
        f.write(body)

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

def _patch_done(path: Path):
  mark = path / '.patched'
  mark.touch()

def _binutils(ver: str, info: ProfileInfo, paths: ProjectPaths):
  url = f'https://ftpmirror.gnu.org/gnu/binutils/{paths.binutils_arx.name}'
  _validate_and_download(paths.binutils_arx, url)
  if _check_and_extract(paths.binutils, paths.binutils_arx):
    # Fix path corruption
    if Version(ver) >= Version('2.43'):
      _patch(paths.binutils, paths.patch / 'binutils' / '2.43-fix-path-corruption.patch')
    else:
      _patch(paths.binutils, paths.patch / 'binutils' / '2.42-fix-path-corruption.patch')

    # Ignore long path
    if info.host_winnt <= 0x03FF:
      _patch(paths.binutils, paths.patch / 'binutils' / 'ignore-long-path.patch')

    _patch_done(paths.binutils)

def _gcc(ver: str, info: ProfileInfo, paths: ProjectPaths):
  if Version(ver) >= Version('15'):
    url = f'https://gcc.gnu.org/pub/gcc/snapshots/{ver}/{paths.gcc_arx.name}'
  else:
    url = f'https://ftpmirror.gnu.org/gnu/gcc/gcc-{ver}/{paths.gcc_arx.name}'
  _validate_and_download(paths.gcc_arx, url)
  if _check_and_extract(paths.gcc, paths.gcc_arx):
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

    if info.host_winnt >= 0x0600:
      # Fix console code page
      _patch(paths.gcc, paths.patch / 'gcc' / 'fix-console-cp.patch')
    else:
      # Remove UTF-8 manifest
      manifest = paths.gcc / 'gcc' / 'config' / 'i386' / 'winnt-utf8.manifest'
      with manifest.open('wb') as f:
        f.write(b'\n')

    # Disable `_aligned_malloc`
    if info.target_winnt <= 0x0500:
      _patch(paths.gcc, paths.patch / 'gcc' / 'disable-aligned-malloc.patch')

    # Fix libbacktrace (NT 5.0)
    if info.host_winnt <= 0x0500 and Version(ver) >= Version('15'):
      _patch(paths.gcc, paths.patch / 'gcc' / '15-nt50-fix-backtrace.patch')

    # Fix libbacktrace (NT 4.0)
    if info.host_winnt <= 0x0400 and Version(ver) >= Version('15'):
      _patch(paths.gcc, paths.patch / 'gcc' / '15-nt40-fix-backtrace.patch')

    # `<filesystem>` fallbacks
    if info.target_winnt <= 0x0400:
      if Version(ver) >= Version('15'):
        _patch(paths.gcc, paths.patch / 'gcc' / '15-filesystem-fallback.patch')
      else:
        _patch(paths.gcc, paths.patch / 'gcc' / '14-filesystem-fallback.patch')

    # `<print>` fallbacks to ANSI API
    if info.target_winnt <= 0x03FF and Version(ver) >= Version('14'):
      _patch(paths.gcc, paths.patch / 'gcc' / '14-print-ansi-fallback.patch')

    # `<fstream>` fallbacks to ANSI API
    if info.target_winnt <= 0x03FF:
      _patch(paths.gcc, paths.patch / 'gcc' / 'fstream-ansi-fallback.patch')

    _patch_done(paths.gcc)

def _gdb(ver: str, info: ProfileInfo, paths: ProjectPaths):
  url = f'https://ftpmirror.gnu.org/gnu/gdb/{paths.gdb_arx.name}'
  _validate_and_download(paths.gdb_arx, url)
  if _check_and_extract(paths.gdb, paths.gdb_arx):
    # Fix thread
    if info.host_winnt <= 0x0600:
      _patch(paths.gdb, paths.patch / 'gdb' / 'fix-thread.patch')

    if info.host_winnt <= 0x0500:
      copyfile(paths.patch / 'gdb' / 'win32-thunk.h', paths.gdb / 'gdb' / 'win32-thunk.h')
      # Kernel32 thunk
      _patch(paths.gdb, paths.patch / 'gdb' / 'kernel32-thunk.patch')

      # IPv6 thunk
      _patch(paths.gdb, paths.patch / 'gdb' / 'ipv6-thunk.patch')

    # Fix VC6 CRT compatibility
    if info.host_winnt <= 0x0400:
      _patch(paths.gdb, paths.patch / 'gdb' / 'vc6-crt-compat.patch')

    _patch_done(paths.gdb)

def _gettext(ver: str, info: ProfileInfo, paths: ProjectPaths):
  url = f'https://ftpmirror.gnu.org/gnu/gettext/{paths.gettext_arx.name}'
  _validate_and_download(paths.gettext_arx, url)
  if _check_and_extract(paths.gettext, paths.gettext_arx):
    # Kernel32 thunk
    if info.host_winnt <= 0x03FF:
      copyfile(paths.patch / 'gettext' / 'win32-thunk.h', paths.gettext / 'gettext-runtime' / 'intl' / 'gnulib-lib' / 'win32-thunk.h')
      _patch(paths.gettext, paths.patch / 'gettext' / 'kernel32-thunk.patch')

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
    # CRT: Fix missing function
    if info.target_winnt <= 0x0502:
      _patch(paths.mingw, paths.patch / 'crt' / 'fix-missing-function.patch')

    # CRT: Allow skip space check in `ftruncate64`
    if info.host_winnt <= 0x0400:
      _patch(paths.mingw, paths.patch / 'crt' / 'ftruncate64-allow-skip-space-check.patch')

    # CRT: Use ANSI API
    if info.target_winnt <= 0x03FF:
      _patch(paths.mingw, paths.patch / 'crt' / 'use-ansi-api.patch')

    # winpthreads: Disable VEH
    if info.target_winnt <= 0x0500:
      _patch(paths.mingw, paths.patch / 'winpthreads' / 'disable-veh.patch')

    # winpthreads: Fix thread
    if info.target_winnt <= 0x03FF:
      _patch(paths.mingw, paths.patch / 'winpthreads' / 'fix-thread.patch')

    # winpthreads: Kernel32 thunk
    if info.target_winnt <= 0x03FF:
      copyfile(paths.patch / 'winpthreads' / 'win32-thunk.h', paths.mingw / 'mingw-w64-libraries' / 'winpthreads' / 'src' / 'win32-thunk.h')
      _patch(paths.mingw, paths.patch / 'winpthreads' / 'kernel32-thunk.patch')

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

def download_and_patch(ver: BranchVersions, paths: ProjectPaths, info: ProfileInfo):
  _binutils(ver.binutils, info, paths)
  _gcc(ver.gcc, info, paths)
  _gdb(ver.gdb, info, paths)
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
