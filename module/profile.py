import argparse
from dataclasses import dataclass
from packaging.version import Version
from typing import Callable, Dict, Optional

@dataclass
class BranchVersions:
  gcc: str
  rev: str

  abi_frozen: bool
  branch_opt_speed: bool
  iconv_win32: bool
  short_import: bool
  utf8_thunk: bool

  mcfgthread: str
  mingw: str
  nowide: str

  binutils: str
  expat: str
  gdb: str
  gmp: str
  iconv: str
  make: str
  mpc: str
  mpfr: str
  pdcurses: str
  pkgconf: str
  python: str
  z: str

  display_version: Optional[str] = None

  meson: str = '1.10.2'
  setuptools: str = '82.0.1'
  xmake: str = '3.0.8'

@dataclass
class ProfileInfo:
  arch: str
  fpmath: Optional[str]
  march: str
  target: str

  default_crt: str
  exception: str
  thread: str

  win32_winnt: int
  min_os: Version

  profile_opt_lto: bool
  profile_opt_speed: bool
  thunk_free: bool
  utf8_user_crt: bool

@dataclass
class BranchProfile(BranchVersions, ProfileInfo):
  @property
  def min_winnt(self) -> int:
    if self.min_os.major < 4:
      return 0x0400
    else:
      return self.min_os.major * 0x100 + self.min_os.minor

  @property
  def opt_speed(self) -> bool:
    return self.branch_opt_speed or self.profile_opt_speed

BRANCHES: Dict[str, BranchVersions] = {
  'next': BranchVersions(
    gcc = '17-20260503',
    rev = '0',
    display_version = 'next-17-20260503',

    abi_frozen = False,
    branch_opt_speed = True,
    iconv_win32 = True,
    short_import = True,
    utf8_thunk = True,

    mcfgthread = '2.3-ga.1',
    mingw = '14.0.0',
    nowide = '11.3.1',

    binutils = '2.46.0',
    expat = '2.7.5',
    gdb = '17.1',
    gmp = '6.3.0',
    iconv = '1.19',
    make = '4.4.1',
    mpc = '1.3.1',
    mpfr = '4.2.2',
    pdcurses = '3.9',
    pkgconf = '2.5.1',
    python = '3.14.4',
    z = '1.3.2',
  ),
  'current': BranchVersions(
    gcc = '16-20260502',
    rev = '0',
    display_version = 'current-16-20260502',

    abi_frozen = False,
    branch_opt_speed = True,
    iconv_win32 = True,
    short_import = True,
    utf8_thunk = True,

    mcfgthread = '2.3-ga.1',
    mingw = '14.0.0',
    nowide = '11.3.1',

    binutils = '2.46.0',
    expat = '2.7.5',
    gdb = '17.1',
    gmp = '6.3.0',
    iconv = '1.19',
    make = '4.4.1',
    mpc = '1.4.1',
    mpfr = '4.2.2',
    pdcurses = '3.9',
    pkgconf = '2.5.1',
    python = '3.14.4',
    z = '1.3.2',
  ),
  '16': BranchVersions(
    gcc = '16.1.0',
    rev = '0.1',

    abi_frozen = False,
    branch_opt_speed = True,
    iconv_win32 = True,
    short_import = True,
    utf8_thunk = True,

    mcfgthread = '2.3-ga.1',
    mingw = '14.0.0',
    nowide = '11.3.1',

    binutils = '2.46.0',
    expat = '2.7.5',
    gdb = '17.1',
    gmp = '6.3.0',
    iconv = '1.19',
    make = '4.4.1',
    mpc = '1.4.1',
    mpfr = '4.2.2',
    pdcurses = '3.9',
    pkgconf = '2.5.1',
    python = '3.14.4',
    z = '1.3.2',
  ),
  '15': BranchVersions(
    gcc = '15.2.0',
    rev = '8.1',

    abi_frozen = True,
    branch_opt_speed = False,
    iconv_win32 = False,
    short_import = False,
    utf8_thunk = False,

    # ABI critical: 2025-08-08
    mcfgthread = '2.1-ga.1',
    mingw = '13.0.0',
    nowide = '11.3.0',  # 11.3.1 is not ABI compatible

    # freeze: 2025-12-21
    binutils = '2.45.1',
    expat = '2.7.5',
    gdb = '17.1',
    gmp = '6.3.0',
    iconv = '1.18',
    make = '4.4.1',
    mpc = '1.3.1',
    mpfr = '4.2.2',
    pdcurses = '3.9',
    pkgconf = '2.5.1',
    python = '3.14.4',
    z = '1.3.2',
  ),
  '14': BranchVersions(
    gcc = '14.3.0',
    rev = '7.1',

    abi_frozen = True,
    branch_opt_speed = False,
    iconv_win32 = False,
    short_import = False,
    utf8_thunk = False,

    # ABI critical: 2024-08-01
    mcfgthread = '1.8-ga.4',
    mingw = '12.0.0',
    nowide = '11.3.0',  # 11.3.1 is not ABI compatible

    # freeze: 2024-12-21
    binutils = '2.43.1',
    expat = '2.6.4',
    gdb = '15.2',
    gmp = '6.3.0',
    iconv = '1.18',
    make = '4.4.1',
    mpc = '1.3.1',
    mpfr = '4.2.2',
    pdcurses = '3.9',
    pkgconf = '2.3.0',
    python = '3.13.13',
    z = '1.3.2',
  ),
  '13': BranchVersions(
    gcc = '13.4.0',
    rev = '7.1',

    abi_frozen = True,
    branch_opt_speed = False,
    iconv_win32 = False,
    short_import = False,
    utf8_thunk = False,

    # trace back: 2023-12-22
    binutils = '2.41',
    expat = '2.5.0',
    gdb = '14.2',
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
    nowide = '11.3.0',  # 11.3.1 is not ABI compatible
    pdcurses = '3.9',
    pkgconf = '2.1.1',
    python = '3.12.13',
    z = '1.3.2',
  ),
}

_MINGW_ARCH_2_TRIPLET_MAP: Dict[str, str] = {
  '64': 'x86_64-w64-mingw32',
  'arm64': 'aarch64-w64-mingw32',
  # required by xmake, here we use i686 for -march=i486 and -march=i386
  '32': 'i686-w64-mingw32',
}

_ARCH_VARIANT_2_MARCH_MAP: Dict[str, str] = {
  '64_v2': 'x86-64-v2',
  '64': 'x86-64',
  'arm64': 'armv8-a',
  '32': 'pentium4',
  '32_686': 'i686',
  '32_486': 'i486',
  '32_386': 'i386',
}

_ARCH_VARIANT_2_FPMATH_MAP: Dict[str, Optional[str]] = {
  '64_v2': None,
  '64': None,
  'arm64': None,
  '32': 'sse',
  '32_686': None,
  '32_486': None,
  '32_386': None,
}

def _create_profile(
  arch: str,
  crt: str,
  thread: str,
  min_os: str,

  opt_lto: bool = False,
  opt_speed: bool = False,
  stable: bool = True,
  u8crt: bool = False,
) -> ProfileInfo:
  mingw_arch = arch.split('_')[0]
  exception = 'dwarf' if mingw_arch == '32' else 'seh'
  triplet = _MINGW_ARCH_2_TRIPLET_MAP[mingw_arch]
  march = _ARCH_VARIANT_2_MARCH_MAP[arch]
  fpmath = _ARCH_VARIANT_2_FPMATH_MAP[arch]

  return ProfileInfo(
    arch = mingw_arch,
    fpmath = fpmath,
    march = march,
    target = triplet,

    default_crt = crt,
    exception = exception,
    thread = thread,

    win32_winnt = 0x0A00,
    min_os = Version(min_os),

    profile_opt_lto = opt_lto,
    profile_opt_speed = opt_speed,
    thunk_free = stable,
    utf8_user_crt = u8crt,
  )

PROFILES: Dict[str, ProfileInfo] = {
  '64-mcf':    _create_profile('64', 'ucrt',   'mcf',   '6.1'),
  '64-win32':  _create_profile('64', 'ucrt',   'win32', '6.0'),
  '64-ucrt':   _create_profile('64', 'ucrt',   'posix', '5.2'),
  '64-msvcrt': _create_profile('64', 'msvcrt', 'posix', '5.2'),

  'arm64-mcf':   _create_profile('arm64', 'ucrt', 'mcf',   '10.0.16299', opt_speed = True),
  'arm64-win32': _create_profile('arm64', 'ucrt', 'win32', '10.0.16299', opt_speed = True),
  'arm64-ucrt':  _create_profile('arm64', 'ucrt', 'posix', '10.0.16299', opt_speed = True),

  '32-mcf':    _create_profile('32', 'ucrt',   'mcf',   '6.1'),
  '32-win32':  _create_profile('32', 'ucrt',   'win32', '6.0'),
  '32-ucrt':   _create_profile('32', 'ucrt',   'posix', '5.1'),
  '32-msvcrt': _create_profile('32', 'msvcrt', 'posix', '5.1'),

  ############################################
  # profile variants for micro architectures #
  ############################################

  '64_v2-mcf':    _create_profile('64_v2', 'ucrt',   'mcf',   '6.1', opt_lto = True, opt_speed = True),
  '64_v2-win32':  _create_profile('64_v2', 'ucrt',   'win32', '6.0', opt_lto = True, opt_speed = True),
  '64_v2-ucrt':   _create_profile('64_v2', 'ucrt',   'posix', '5.2', opt_lto = True, opt_speed = True),
  '64_v2-msvcrt': _create_profile('64_v2', 'msvcrt', 'posix', '5.2', opt_lto = True, opt_speed = True),

  #################################################
  # profile variants for earlier Windows versions #
  #################################################

  '32-msvcrt_win2000': None, # branch-dependent

  '32_686-msvcrt_winnt40': _create_profile('32_686', 'msvcrt', 'posix', '4.0',         stable = False),
  '32_686-msvcrt_win98':   _create_profile('32_686', 'msvcrt', 'posix', '3.9999+4.10', stable = False),
  '32_486-msvcrt_win98':   _create_profile('32_486', 'msvcrt', 'posix', '3.9999+4.10', stable = False),
  '32_386-msvcrt_win95':   _create_profile('32_386', 'msvcrt', 'posix', '3.9999+4.00', stable = False),

  #####################
  # beyond MinGW Lite #
  #####################

  '64-u8crt': _create_profile('64', 'ucrt', 'posix', '5.2', opt_speed = True, stable = False, u8crt = True),
  '32-u8crt': _create_profile('32', 'ucrt', 'posix', '5.1', opt_speed = True, stable = False, u8crt = True),
}

def _profile_32_msvcrt_win2000(branch: str):
  v = Version(branch)
  if v.major >= 16:
    raise NotImplementedError()
  return _create_profile('32', 'msvcrt', 'posix', '5.0', stable = False)

BRANCH_DEPENDENT_PROFILES: Dict[str, Callable[[str], ProfileInfo]] = {
  '32-msvcrt_win2000': _profile_32_msvcrt_win2000,
}

def resolve_profile(config: argparse.Namespace) -> BranchProfile:
  ver = BRANCHES[config.branch]
  if config.profile in BRANCH_DEPENDENT_PROFILES:
    info = BRANCH_DEPENDENT_PROFILES[config.profile](config.branch)
  else:
    info = PROFILES[config.profile]
  return BranchProfile(
    **ver.__dict__,
    **info.__dict__,
  )
