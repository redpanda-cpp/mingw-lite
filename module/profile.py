import argparse
from typing import Dict, Optional

class BranchVersions:
  gcc: str
  rev: str

  binutils: str
  gdb: str
  gettext: Optional[str]
  gmp: str
  iconv: str
  make: str
  mcfgthread: Optional[str]
  mingw: str
  mpc: str
  mpfr: str

  def __init__(
    self,

    gcc: str,
    rev: str,

    binutils: str,
    gdb: str,
    gettext: Optional[str],
    gmp: str,
    iconv: str,
    make: str,
    mcfgthread: Optional[str],
    mingw: str,
    mpc: str,
    mpfr: str,
  ):
    self.gcc = gcc
    self.rev = rev

    self.binutils = binutils
    self.gdb = gdb
    self.gettext = gettext
    self.gmp = gmp
    self.iconv = iconv
    self.make = make
    self.mcfgthread = mcfgthread
    self.mingw = mingw
    self.mpc = mpc
    self.mpfr = mpfr

class ProfileInfo:
  arch: str
  target: str

  crt: str
  exception: str
  thread: str

  host_winnt: int
  target_winnt: int

  def __init__(
    self,

    arch: str,
    target: str,

    crt: str,
    exception: str,
    thread: str,

    host_winnt: int,
    target_winnt: int,
  ):
    self.arch = arch
    self.target = target

    self.crt = crt
    self.exception = exception
    self.thread = thread

    self.host_winnt = host_winnt
    self.target_winnt = target_winnt

class FullProfile:
  ver: BranchVersions
  info: ProfileInfo

  def __init__(
    self,
    ver: BranchVersions,
    info: ProfileInfo,
  ):
    self.ver = ver
    self.info = info

BRANCHES: Dict[str, BranchVersions] = {
  '14': BranchVersions(
    gcc = '14.2.0',
    rev = '1',

    binutils = '2.42',
    gdb = '15.1',
    gettext = '0.22.5',
    gmp = '6.3.0',
    iconv = '1.17',
    make = '4.4.1',
    mcfgthread = '1.8-ga.3',
    mingw = '12.0.0',
    mpc = '1.3.1',
    mpfr = '4.2.1',
  ),
  '15': BranchVersions(
    gcc = '15-20240804',
    rev = '0',

    binutils = '2.43',
    gdb = '15.1',
    gettext = '0.22.5',
    gmp = '6.3.0',
    iconv = '1.17',
    make = '4.4.1',
    mcfgthread = '1.8-ga.3',
    mingw = '12.0.0',
    mpc = '1.3.1',
    mpfr = '4.2.1',
  ),
}

PROFILES: Dict[str, ProfileInfo] = {
  '64-mcf': ProfileInfo(
    arch = '64',
    target = 'x86_64-w64-mingw32',

    crt = 'ucrt',
    exception = 'seh',
    thread = 'mcf',

    host_winnt = 0x0601,
    target_winnt = 0x0601,
  ),
  '64-ucrt': ProfileInfo(
    arch = '64',
    target = 'x86_64-w64-mingw32',

    crt = 'ucrt',
    exception = 'seh',
    thread = 'posix',

    host_winnt = 0x0601,
    target_winnt = 0x0502,
  ),
  '64-msvcrt': ProfileInfo(
    arch = '64',
    target = 'x86_64-w64-mingw32',

    crt = 'msvcrt',
    exception = 'seh',
    thread = 'posix',

    host_winnt = 0x0502,
    target_winnt = 0x0502,
  ),

  'arm64-mcf': ProfileInfo(
    arch = 'arm64',
    target = 'aarch64-w64-mingw32',

    crt = 'ucrt',
    exception = 'seh',
    thread = 'mcf',

    host_winnt = 0x0A00,
    target_winnt = 0x0A00,
  ),
  'arm64-ucrt': ProfileInfo(
    arch = 'arm64',
    target = 'aarch64-w64-mingw32',

    crt = 'ucrt',
    exception = 'seh',
    thread = 'posix',

    host_winnt = 0x0A00,
    target_winnt = 0x0A00,
  ),

  '32-mcf': ProfileInfo(
    arch = '32',
    target = 'i686-w64-mingw32',

    crt = 'ucrt',
    exception = 'dwarf',
    thread = 'mcf',

    host_winnt = 0x0601,
    target_winnt = 0x0601,
  ),
  '32-ucrt': ProfileInfo(
    arch = '32',
    target = 'i686-w64-mingw32',

    crt = 'ucrt',
    exception = 'dwarf',
    thread = 'posix',

    host_winnt = 0x0601,
    target_winnt = 0x0501,
  ),
  '32-msvcrt': ProfileInfo(
    arch = '32',
    target = 'i686-w64-mingw32',

    crt = 'msvcrt',
    exception = 'dwarf',
    thread = 'posix',

    host_winnt = 0x0500,
    target_winnt = 0x0400,
  ),
}

def get_full_profile(config: argparse.Namespace) -> FullProfile:
  return FullProfile(
    ver = BRANCHES[config.branch],
    info = PROFILES[config.profile],
  )
