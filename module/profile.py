import argparse
from packaging.version import Version
from typing import Dict, Optional
class BranchVersions:
  gcc: str
  rev: str

  binutils: str
  expat: str
  gdb: str
  gettext: Optional[str]
  gmp: str
  iconv: str
  make: str
  mcfgthread: str
  mingw: str
  mpc: str
  mpfr: str
  python: str
  python_z: str
  xmake: str

  def __init__(
    self,

    gcc: str,
    rev: str,

    binutils: str,
    expat: str,
    gdb: str,
    gettext: Optional[str],
    gmp: str,
    iconv: str,
    make: str,
    mcfgthread: str,
    mingw: str,
    mpc: str,
    mpfr: str,
    python: str,
    python_z: str,
    xmake: str,
  ):
    self.gcc = gcc
    self.rev = rev

    self.binutils = binutils
    self.expat = expat
    self.gdb = gdb
    self.gettext = gettext
    self.gmp = gmp
    self.iconv = iconv
    self.make = make
    self.mcfgthread = mcfgthread
    self.mingw = mingw
    self.mpc = mpc
    self.mpfr = mpfr
    self.python = python
    self.python_z = python_z
    self.xmake = xmake

class ProfileInfo:
  arch: str
  target: str

  default_crt: str
  exception: str
  thread: str

  win32_winnt: int
  min_os: Version

  def __init__(
    self,

    arch: str,
    target: str,

    default_crt: str,
    exception: str,
    thread: str,

    win32_winnt: int,
    min_os: str,
  ):
    self.arch = arch
    self.target = target

    self.default_crt = default_crt
    self.exception = exception
    self.thread = thread

    self.win32_winnt = win32_winnt
    self.min_os = Version(min_os)

class BranchProfile(BranchVersions):
  arch: str
  target: str

  default_crt: str
  exception: str
  thread: str

  win32_winnt: int
  min_os: Version
  min_winnt: int

  def __init__(
    self,
    ver: BranchVersions,
    info: ProfileInfo,
  ):
    BranchVersions.__init__(self, **ver.__dict__)

    self.arch = info.arch
    self.target = info.target

    self.default_crt = info.default_crt
    self.exception = info.exception
    self.thread = info.thread

    self.win32_winnt = info.win32_winnt
    self.min_os = info.min_os

    if info.min_os.major < 4:
      self.min_winnt = 0x0400
    else:
      self.min_winnt = info.min_os.major * 0x100 + info.min_os.minor

BRANCHES: Dict[str, BranchVersions] = {
  '16': BranchVersions(
    gcc = '15-20250420',
    rev = '0',

    binutils = '2.44',
    expat = '2.7.1',
    gdb = '16.3',
    gettext = '0.24',
    gmp = '6.3.0',
    iconv = '1.18',
    make = '4.4.1',
    mcfgthread = '2.0-ga.1',
    mingw = '12.0.0',
    mpc = '1.3.1',
    mpfr = '4.2.2',
    python = '3.13.3',
    python_z = '1.3.1',
    xmake = '2.9.9',
  ),
  '15': BranchVersions(
    gcc = '15.1.0-RC-20250418',
    rev = '0',

    binutils = '2.44',
    expat = '2.7.1',
    gdb = '16.3',
    gettext = '0.24',
    gmp = '6.3.0',
    iconv = '1.18',
    make = '4.4.1',
    mcfgthread = '2.0-ga.1',
    mingw = '12.0.0',
    mpc = '1.3.1',
    mpfr = '4.2.2',
    python = '3.13.3',
    python_z = '1.3.1',
    xmake = '2.9.9',
  ),
  '14': BranchVersions(
    gcc = '14.2.0',
    rev = '5',

    # freeze: 2025-01-01
    binutils = '2.43.1',
    expat = '2.6.4',
    gdb = '15.2',
    gettext = '0.23.1',
    gmp = '6.3.0',
    iconv = '1.18',
    make = '4.4.1',
    mcfgthread = '1.9-ga.2',
    mingw = '12.0.0',
    mpc = '1.3.1',
    mpfr = '4.2.2',
    python = '3.13.3',
    python_z = '1.3.1',
    xmake = '2.9.9',
  ),
  '13': BranchVersions(
    gcc = '13.3.0',
    rev = '0',

    # freeze: 2024-01-01
    binutils = '2.41',
    expat = '2.5.0',
    gdb = '14.2',
    gettext = None,
    gmp = '6.3.0',
    iconv = '1.17',
    make = '4.4.1',
    # postponed until 1.8-ga.3 (2024-08-01)
    # - bootstrapping from scratch made easy
    # - gcc runtime library exception added
    mcfgthread = '1.8-ga.4',
    mingw = '11.0.1',
    mpc = '1.3.1',
    mpfr = '4.2.2',
    python = '3.12.10',
    python_z = '1.3.1',
    xmake = '2.9.9',
  ),
}

PROFILES: Dict[str, ProfileInfo] = {
  '64-mcf': ProfileInfo(
    arch = '64',
    target = 'x86_64-w64-mingw32',

    default_crt = 'ucrt',
    exception = 'seh',
    thread = 'mcf',

    win32_winnt = 0x0A00,
    min_os = '6.1',
  ),
  '64-win32': ProfileInfo(
    arch = '64',
    target = 'x86_64-w64-mingw32',

    default_crt = 'ucrt',
    exception = 'seh',
    thread = 'win32',

    win32_winnt = 0x0A00,
    min_os = '6.0',
  ),
  '64-ucrt': ProfileInfo(
    arch = '64',
    target = 'x86_64-w64-mingw32',

    default_crt = 'ucrt',
    exception = 'seh',
    thread = 'posix',

    win32_winnt = 0x0A00,
    min_os = '6.0',
  ),
  '64-msvcrt': ProfileInfo(
    arch = '64',
    target = 'x86_64-w64-mingw32',

    default_crt = 'msvcrt',
    exception = 'seh',
    thread = 'posix',

    win32_winnt = 0x0A00,
    min_os = '6.0',
  ),

  'arm64-mcf': ProfileInfo(
    arch = 'arm64',
    target = 'aarch64-w64-mingw32',

    default_crt = 'ucrt',
    exception = 'seh',
    thread = 'mcf',

    win32_winnt = 0x0A00,
    min_os = '10.0.16299',
  ),
  'arm64-win32': ProfileInfo(
    arch = 'arm64',
    target = 'aarch64-w64-mingw32',

    default_crt = 'ucrt',
    exception = 'seh',
    thread = 'win32',

    win32_winnt = 0x0A00,
    min_os = '10.0.16299',
  ),
  'arm64-ucrt': ProfileInfo(
    arch = 'arm64',
    target = 'aarch64-w64-mingw32',

    default_crt = 'ucrt',
    exception = 'seh',
    thread = 'posix',

    win32_winnt = 0x0A00,
    min_os = '10.0.16299',
  ),

  '32-mcf': ProfileInfo(
    arch = '32',
    target = 'i686-w64-mingw32',

    default_crt = 'ucrt',
    exception = 'dwarf',
    thread = 'mcf',

    win32_winnt = 0x0A00,
    min_os = '6.1',
  ),
  '32-win32': ProfileInfo(
    arch = '32',
    target = 'i686-w64-mingw32',

    default_crt = 'ucrt',
    exception = 'dwarf',
    thread = 'win32',

    win32_winnt = 0x0A00,
    min_os = '6.0',
  ),
  '32-ucrt': ProfileInfo(
    arch = '32',
    target = 'i686-w64-mingw32',

    default_crt = 'ucrt',
    exception = 'dwarf',
    thread = 'posix',

    win32_winnt = 0x0A00,
    min_os = '6.0',
  ),
  '32-msvcrt': ProfileInfo(
    arch = '32',
    target = 'i686-w64-mingw32',

    default_crt = 'msvcrt',
    exception = 'dwarf',
    thread = 'posix',

    win32_winnt = 0x0A00,
    min_os = '6.0',
  ),
}

def resolve_profile(config: argparse.Namespace) -> BranchProfile:
  return BranchProfile(
    ver = BRANCHES[config.branch],
    info = PROFILES[config.profile],
  )
