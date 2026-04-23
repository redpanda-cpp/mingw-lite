#!/usr/bin/python3

import argparse
import json

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
  '32-msvcrt_win2000',
  '32_686-msvcrt_winnt40',
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
  *( # deprecated
    {'profile': '32-msvcrt_win2000', 'branch': b}
    for b in ['next', 'current', '16']
  ),
  *( # u8thunk path test
    {'profile': '32_686-msvcrt_winnt40', 'branch': b}
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
    'name': '32-unstable',
    'profile': json.dumps(['32-msvcrt_win2000']),
    'pattern': 'mingw32-msvcrt_win2000-*',
    'dict': '128m',
  },
  {
    'name': '32_686-unstable',
    'profile': json.dumps(['32_686-msvcrt_winnt40', '32_686-msvcrt_win98']),
    'pattern': 'mingw32_686-*',
    'dict': '128m',
  },
  {
    'name': '32_486-unstable',
    'profile': json.dumps(['32_486-msvcrt_win98']),
    'pattern': 'mingw32_486-*',
    'dict': '128m',
  },
  {
    'name': '32_386-unstable',
    'profile': json.dumps(['32_386-msvcrt_win95']),
    'pattern': 'mingw32_386-*',
    'dict': '128m',
  },

  # beyond mingw lite
  {
    'name': 'u8crt',
    'profile': json.dumps(['64-u8crt', '32-u8crt']),
    'pattern': '{mingw64-u8crt-*,mingw32-u8crt-*}',
    'dict': '128m',
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
      p == x['profile'] and branch == x['branch']
      for x in exclude_profile_branch)]
  execlude_profile_branch = []
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
print(f'release={json.dumps(release)}')
print(f'prerelease={json.dumps(prerelease)}')
print(f'sat_group={json.dumps(sat_group)}')
print(f'sat_exclude_profile_branch={sat_exclude_profile_branch}')
