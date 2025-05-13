#!/usr/bin/python3

import argparse
import os
from packaging.version import Version
from pathlib import Path
import re
import shutil

from module.thunk_list_core import THUNK_LIST_CORE
from module.thunk_list_toolchain import THUNK_LIST_TOOLCHAIN

ARCH_DEFAULT_VERSION_MAP = {
  '32': '4.0',
  '64': '5.2',
  'arm64': '10.0.19041',
}

ARCH_LIB_DIR_MAP = {
  '32': 'lib32',
  '64': 'lib64',
  'arm64': 'libarm64',
}

ARCH_CPPFLAGS_MAP = {
  '32': 'CPPFLAGS32',
  '64': 'CPPFLAGS64',
  'arm64': 'CPPFLAGSARM64',
}

ARCH_DT_DEF_COMMAND_MAP = {
  '32': 'DTDEF32',
  '64': 'DTDEF64',
  'arm64': 'DTDEFARM64',
}

def parse_args():
  parser = argparse.ArgumentParser(
    description = 'Patch mingw-w64 source code'
  )
  parser.add_argument(
    'source',
    type = Path,
    help = 'Path to mingw-w64 source code'
  )
  parser.add_argument(
    '-a', '--arch',
    type = str,
    choices = ARCH_DEFAULT_VERSION_MAP.keys(),
    required = True,
    help = 'Target architecture'
  )
  parser.add_argument(
    '--level',
    type = str,
    choices = ['core', 'toolchain'],
    required = True,
    help = 'Thunk level'
  )
  parser.add_argument(
    '--nt-ver',
    type = Version,
    help = 'Target Windows NT version'
  )
  args = parser.parse_args()
  if not args.nt_ver:
    args.nt_ver = Version(ARCH_DEFAULT_VERSION_MAP[args.arch])
  return args

def resolve_modules_and_add_thunk_sources(args, modules, thunk_list):
  mingw_mac_path = args.source / 'mingw-w64-headers' / 'crt' / '_mingw_mac.h'
  with open(mingw_mac_path, 'r') as f:
    v_mingw_major_pattern = re.compile(r'#define\s+__MINGW64_VERSION_MAJOR\s+(\d+)')
    v_mingw_major_str = v_mingw_major_pattern.search(f.read()).group(1)
    v_mingw_major = int(v_mingw_major_str)

  thunk_dir = args.source / 'mingw-w64-crt'/ 'thunk'
  thunk_dir.mkdir(parents = True, exist_ok = True)

  shutil.copytree('include/thunk', thunk_dir, dirs_exist_ok = True)

  for (ver_str, mod_list) in thunk_list.items():
    v = Version(ver_str)
    if v <= args.nt_ver:
      continue
    for (mod_name, func_list) in mod_list.items():
      if mod_name not in modules:
        modules[mod_name] = []
      for func_name in func_list:
        if type(func_name) == tuple:
          func_name, cond = func_name
          if not cond(v_mingw_major):
            continue
        shutil.copy(f'src/{ver_str}/{mod_name}/{func_name}.cc', thunk_dir)
        modules[mod_name].append(func_name)

def locate_mod_def_file(possible_paths):
  for path in possible_paths:
    if path.exists():
      return path
  raise FileNotFoundError('No suitable .def file found')

def patch_automake_template(args, modules):
  # load automake template
  AM_ADDITION_START = '# <<< mingw-thunk addition\n'
  AM_ADDITION_END = '# >>> mingw-thunk addition\n'
  am_file = args.source / 'mingw-w64-crt' / 'Makefile.am'
  with open(am_file, 'r') as f:
    am_content = f.readlines()
  addition_start = -1
  addition_end = -1
  for i, line in enumerate(am_content):
    if line == AM_ADDITION_START:
      addition_start = i
    if line == AM_ADDITION_END:
      addition_end = i
  if addition_start >= 0:
    am_content = am_content[:addition_start] + am_content[addition_end+1:]

  # patch automake cflags
  cflags_line = -1
  cxxflags_line = -1
  cflags_pattern = re.compile(r'^AM_CFLAGS\s*=')
  cxxflags_pattern = re.compile(r'^AM_CXXFLAGS\s*=')
  for i, line in enumerate(am_content):
    if cflags_pattern.match(line):
      cflags_line = i
    if cxxflags_pattern.match(line):
      cxxflags_line = i
  if cflags_line < 0 or cxxflags_line < 0:
    raise RuntimeError('Cannot find cflags or cxxflags in Makefile.am')
  cflags = cflags_pattern.sub('', am_content[cflags_line]).strip().split()
  cxxflags = ['-nostdinc++', '-fno-exceptions', '-fno-threadsafe-statics']
  for flag in cflags:
    if flag.startswith('-std='):
      cxxflags.append('-std=gnu++17')
    elif '_C_ONLY_' in flag:
      cxxflags.append(flag.replace('_C_ONLY_', '_CXX_ONLY_'))
    else:
      cxxflags.append(flag)
  am_content[cxxflags_line] = 'AM_CXXFLAGS=' + ' '.join(cxxflags) + '\n'

  # process modules
  am_addition = []
  lib_dir = ARCH_LIB_DIR_MAP[args.arch]
  arch_def_dir = args.source / 'mingw-w64-crt' / lib_dir
  common_def_dir = args.source / 'mingw-w64-crt' / 'lib-common'
  for mod_name, func_list in modules.items():
    # sanitize 'api-ms-win-...'
    sanitized_mod_name = mod_name.replace('-', '_')

    # special handling 'libmsvcrt-os.a':
    # the def file is 'msvcrt.def' (our on-disk name),
    # and the automake template specifies 'libmsvcrt_extra.a'.
    if mod_name == 'msvcrt':
      sanitized_mod_name = 'msvcrt_extra'

    # find module def file
    mod_def_file_paths = [
      arch_def_dir / (mod_name + '.def.in'),
      arch_def_dir / (mod_name + '.def'),
      common_def_dir / (mod_name + '.def.in'),
      common_def_dir / (mod_name + '.def'),
    ]
    mod_def_file = locate_mod_def_file(mod_def_file_paths)

    # patch module def file
    with open(mod_def_file, 'r') as f:
      mod_def_content = f.readlines()
    for func_name in func_list:
      if args.arch == '32':
        pattern = re.compile(rf'^{func_name}(@\d*)?$')
      else:
        pattern = re.compile(rf'^{func_name}$')
      for i, line in enumerate(mod_def_content):
        if pattern.match(line):
          mod_def_content[i] = '; ' + line
    with open(mod_def_file, 'w') as f:
      f.writelines(mod_def_content)

    # add thunk files to makefile rules
    am_src = list(map(lambda func: f'thunk/{func}.cc', func_list))
    am_lib_name = f'{lib_dir}_lib{sanitized_mod_name}_a'

    # protect msvcrt from cygwin w32api
    if mod_name == 'msvcrt':
      am_addition.append(f'if !W32API\n')

    # do we have a lib<mod_name>.a rule already?
    # then append to existing one or create new one
    am_lib_sources_pattern = re.compile(fr'^{am_lib_name}_SOURCES\s*=')
    am_lib_sources_line = -1
    for i, line in enumerate(am_content):
      if am_lib_sources_pattern.match(line):
        am_lib_sources_line = i
    if am_lib_sources_line >= 0:
      am_addition.append(f'{am_lib_name}_SOURCES += ' + ' '.join(am_src) + '\n')
    else:
      cppflags = ARCH_CPPFLAGS_MAP[args.arch]
      dt_def_command = ARCH_DT_DEF_COMMAND_MAP[args.arch]
      am_addition.append(f'{lib_dir}_LIBRARIES += {lib_dir}/lib{mod_name}.a\n')
      am_addition.append(f'{am_lib_name}_SOURCES = {" ".join(am_src)}\n')
      am_addition.append(f'{am_lib_name}_CPPFLAGS = $({cppflags}) $(sysincludes)\n')
      rel_def_path = mod_def_file.relative_to(args.source / 'mingw-w64-crt')
      am_addition.append(f'{am_lib_name}_AR = $({dt_def_command}) $(top_srcdir)/{rel_def_path} && $(AR) $(ARFLAGS)\n')

    # protect msvcrt from cygwin w32api
    if mod_name == 'msvcrt':
      am_addition.append(f'endif\n')

  with open(am_file, 'w') as f:
    f.writelines(am_content + [AM_ADDITION_START, f'if {lib_dir.upper()}\n'] + am_addition + ['endif\n', AM_ADDITION_END])

def main():
  args = parse_args()
  modules = {}
  resolve_modules_and_add_thunk_sources(args, modules, THUNK_LIST_CORE)
  if args.level == 'toolchain':
    resolve_modules_and_add_thunk_sources(args, modules, THUNK_LIST_TOOLCHAIN)
  patch_automake_template(args, modules)

if __name__ == '__main__':
  main()
