#!/usr/bin/python3

import argparse
import json
from typing import List, TypedDict

class AltBuildOsRelTestCross(TypedDict):
  osrel: str
  test_cross: bool

parser = argparse.ArgumentParser()
parser.add_argument('--ref-type')
parser.add_argument('--ref-name')
args = parser.parse_args()

all_branch = [
  'next', 'current',
  '16', '15', '14', '13',
]

common_profile = [
  '64-mcf', '64-win32', '64-ucrt', '64-msvcrt',
  '32-mcf', '32-win32', '32-ucrt', '32-msvcrt',

  # profile variants for micro architectures
  '64_v2-mcf', '64_v2-win32', '64_v2-ucrt', '64_v2-msvcrt',
]
all_old_profile = [
  '64-ucrt_ws2003', '64-msvcrt_ws2003',
  '32-ucrt_winxp',  '32-msvcrt_win2000',
  '32_686-msvcrt_win98',
  '32_486-msvcrt_win98',
  '32_386-msvcrt_win95',
]
release_old_profile = [
  '32-msvcrt_win2000',
  '32_686-msvcrt_win98',
  '32_486-msvcrt_win98',
]
beyond_profile = [
  # u8crt
  '64-u8crt', '32-u8crt',
]

exclude_profile_branch = [
  *( # native TLS removal
    {'profile': '32_386-msvcrt_win95', 'branch': b}
    for b in ['next', 'current', '16']
  ),
  *( # native TLS
    {'profile': p, 'branch': b}
    # 32-msvcrt_win2000 was initially added because gcc was mislead by Microsoft docs
    # and its UTF-8 manifest was not compatible with Windows XP.
    # Windows 2000 is the first NT version that guarantees SSE support,
    # and is so close to Windows XP, so we support it without difficulty.

    # 32-msvcrt_win2000 was deprecated since we fixed the manifest conformance.
    # Windows XP was the oldest NT version supported by upstream at that time,
    # so we set it as the new baseline (more specifically, thunk-free).
    # 32-msvcrt_win2000 was planned to be removed in branch 16.

    # as GCC 16 introduces native TLS, and some newer APIs are called by standard library,
    # the baseline is raised to Windows Vista. Windows XP is still supported for its usage.
    # for the same reason we initially supported Windows 2000, 32-msvcrt_win2000 is de-deprecated.

    # it's a coincidence that 32-msvcrt_win2000 by any branch is not excluded.
    for p in [
      '64-ucrt_ws2003', '64-msvcrt_ws2003',
      '32-ucrt_winxp',
    ]
    for b in ['15', '14', '13']
  ),
  *( # u8crt
    {'profile': p, 'branch': b}
    for p in ['64-u8crt', '32-u8crt']
    for b in ['16', '15', '14', '13']
  ),
]

alt_branch = ['current']
alt_profile = ['64-mcf', '64-ucrt']
alt_osrel = [
  'archlinux',
  'debian13', 'debian12', 'debian11',
  'ubuntu2604', 'ubuntu2404', 'ubuntu2204',
]
alt_osrel_test_cross: List[AltBuildOsRelTestCross] = [
  {'osrel': 'archlinux', 'test_cross': False},
  {'osrel': 'debian13', 'test_cross': True},
  {'osrel': 'debian12', 'test_cross': True},
  {'osrel': 'debian11', 'test_cross': True},
  {'osrel': 'ubuntu2604', 'test_cross': True},
  {'osrel': 'ubuntu2404', 'test_cross': True},
  {'osrel': 'ubuntu2204', 'test_cross': True},
]

sat_group = [
  {
    'name': '64-stable',
    'profile': json.dumps(['64-mcf', '64-win32', '64-ucrt', '64-msvcrt']),
    'pattern': '{mingw64-mcf-*,mingw64-win32-*,mingw64-ucrt-*,mingw64-msvcrt-*}',
    'dict': '512m',
  },
  {
    'name': '64_v2-stable',
    'profile': json.dumps(['64_v2-mcf', '64_v2-win32', '64_v2-ucrt', '64_v2-msvcrt']),
    'pattern': '{mingw64_v2-mcf-*,mingw64_v2-win32-*,mingw64_v2-ucrt-*,mingw64_v2-msvcrt-*}',
    'dict': '512m',
  },
  {
    'name': '32-stable',
    'profile': json.dumps(['32-mcf', '32-win32', '32-ucrt', '32-msvcrt']),
    'pattern': '{mingw32-mcf-*,mingw32-win32-*,mingw32-ucrt-*,mingw32-msvcrt-*}',
    'dict': '512m',
  },
  {
    'name': '64-unstable',
    'profile': json.dumps(['64-ucrt_ws2003', '64-msvcrt_ws2003']),
    'pattern': '{mingw64-ucrt_ws2003-*,mingw64-msvcrt_ws2003-*}',
    'dict': '512m',
  },
  {
    'name': '32-unstable',
    'profile': json.dumps(['32-ucrt_winxp', '32-msvcrt_win2000']),
    'pattern': '{mingw32-ucrt_winxp-*,mingw32-msvcrt_win2000-*}',
    'dict': '512m',
  },
  {
    'name': '32_686-unstable',
    'profile': json.dumps(['32_686-msvcrt_win98']),
    'pattern': 'mingw32_686-*',
    # may be extracted on 9x, be moderate
    # 32_386-msvcrt_win95, with all 6 branches, tested on 2026-04-24:
    #   d=192m: 184.9 MiB
    #   d=224m: 170.1 MiB
    #   d=256m: 169.8 MiB
    #   d=320m: 169.5 MiB
    'dict': '224m',
  },
  {
    'name': '32_486-unstable',
    'profile': json.dumps(['32_486-msvcrt_win98']),
    'pattern': 'mingw32_486-*',
    # may be extracted on 9x, be moderate
    'dict': '224m',
  },
  {
    'name': '32_386-unstable',
    'profile': json.dumps(['32_386-msvcrt_win95']),
    'pattern': 'mingw32_386-*',
    # may be extracted on 9x, be moderate
    # outdated, tested on 2026-04-24:
    #   d=192m: 184.9 MiB
    #   d=224m: 170.1 MiB
    #   d=256m: 169.8 MiB
    #   d=320m: 169.5 MiB
    'dict': '224m',
  },

  # beyond mingw lite
  {
    'name': 'u8crt',
    'profile': json.dumps(['64-u8crt', '32-u8crt']),
    'pattern': '{mingw64-u8crt-*,mingw32-u8crt-*}',
    'dict': '512m',
  },
]

sat_exclude_profile_branch = ':'.join([
  f'{x['profile']}-{x['branch']}'
  for x in exclude_profile_branch
])

if args.ref_type == 'tag':
  current_branch = args.ref_name.split('-')[0].split('.')[0]
  branch = [current_branch]
  profile = [
    p for p in (common_profile + release_old_profile)
    if not any(
      p == x['profile'] and current_branch == x['branch']
      for x in exclude_profile_branch)]
  exclude_profile_branch = []
  release = True
  prerelease = not current_branch.isdigit() or int(current_branch) >= 16
else:
  branch = all_branch
  profile = common_profile + all_old_profile + beyond_profile
  release = False
  prerelease = True

print(f'branch={json.dumps(branch)}')
print(f'profile={json.dumps(profile)}')
print(f'exclude_profile_branch={json.dumps(exclude_profile_branch)}')
print(f'alt_branch={json.dumps(alt_branch)}')
print(f'alt_profile={json.dumps(alt_profile)}')
print(f'alt_osrel={json.dumps(alt_osrel)}')
print(f'alt_osrel_test_cross={json.dumps(alt_osrel_test_cross)}')
print(f'release={json.dumps(release)}')
print(f'prerelease={json.dumps(prerelease)}')
print(f'sat_group={json.dumps(sat_group)}')
print(f'sat_exclude_profile_branch={sat_exclude_profile_branch}')
