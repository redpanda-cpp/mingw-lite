from concurrent.futures import ThreadPoolExecutor
from enum import Enum
import io
import libarchive
import logging
import struct
from pathlib import Path
import shutil
import subprocess
from tempfile import TemporaryDirectory
import time
from typing import Dict, List, NamedTuple, Optional, Set, Tuple

from module.profile import BranchProfile
from module.path import ProjectPaths
from module.util import ensure

DLLTOOL_ARCH_MAP = {
  '32': 'i386',
  '64': 'i386:x86-64',
  'arm64': 'arm64',
}

PE_MACHINE_MAP = {
  '32': 0x014c,
  '64': 0x8664,
  'arm64': 0xAA64,
}

class ImportKind(Enum):
  CODE = 0
  DATA = 1
  CONST = 2

class NormalImportSymbol(NamedTuple):
  name: str
  kind: ImportKind

class AliasImportSymbol(NamedTuple):
  name: str
  kind: ImportKind
  target: str

class ImportSymbols(NamedTuple):
  normal: Set[NormalImportSymbol]
  alias: Set[AliasImportSymbol]

def undecorate_32(symbol: str) -> str:
  if symbol[0] == '_':
    symbol = symbol[1:]
  elif symbol[0] == '@' or symbol[0] == '?':
    pass
  else:
    raise Exception(f'Unexpected symbol name: {symbol}')
  return symbol

def process_single_import_library(
  ver: BranchProfile,
  imp0: Path,
  imp: Path,
):
  dynamic: Dict[str, ImportSymbols] = {}
  static = set()

  with libarchive.file_reader(str(imp0)) as archive:
    for entry in archive:
      data = b''
      for block in entry.get_blocks():
        data += block

      if entry.name == '/' or entry.name == '//':
        # import metadata
        pass
      elif entry.name.endswith(('.dll', '.DLL')):
        # import object
        if b'__NULL_IMPORT_DESCRIPTOR\x00' in data or b'_NULL_THUNK_DATA\x00' in data:
          # import auxiliary data
          pass
        elif data[:4] == b'\x00\x00\xff\xff':
          # normal import symbol

          # check types at offset 18
          #   2 bits: type
          #   3 bits: name type
          kind = ImportKind(data[18] & 0b11)

          strings = data[20:].split(b'\x00')
          assert(len(strings) == 3 and len(strings[2]) == 0)
          sym_name = strings[0].decode()
          if ver.arch == '32':
            sym_name = undecorate_32(sym_name)
          dll = strings[1].decode().lower()
          assert(dll == entry.name.lower())

          if dll not in dynamic:
            dynamic[dll] = ImportSymbols(set(), set())
          dynamic[dll].normal.add(NormalImportSymbol(sym_name, kind))
        else:
          # check for weak alias
          file_header = struct.unpack('<HHIIIHH', data[:20])
          if file_header[0] != PE_MACHINE_MAP[ver.arch]:
            raise Exception(f'PE machine type mismatch: {file_header[0]:x} != {PE_MACHINE_MAP[ver.arch]:x}')

          sym_table_off = file_header[3]
          sym_cnt = file_header[4]

          str_table_off = sym_table_off + 18 * sym_cnt
          str_table = data[str_table_off:]
          str_table_size = struct.unpack('<I', str_table[:4])[0]
          str_table = str_table[:str_table_size]

          externals: Dict[int, str] = {}
          weak_externals: Dict[str, int] = {}

          for i in range(sym_cnt):
            sym_off = sym_table_off + 18 * i
            sym_entry = data[sym_off:sym_off + 18]
            sym_fields = struct.unpack('<QIHHBB', sym_entry)
            if sym_fields[0] & 0xffff_ffff == 0:
              # symbol name in string table
              sym_name_off = sym_fields[0] >> 32
              sym_name = str_table[sym_name_off:].split(b'\x00', 1)[0].decode()
            else:
              sym_name = sym_entry[:8].split(b'\x00', 1)[0].decode()
            storage_class = sym_fields[4]
            if storage_class == 2:
              # external symbol
              if sym_name.startswith('__imp_'):
                sym_name = sym_name[6:]
              if ver.arch == '32':
                sym_name = undecorate_32(sym_name)
              externals[i] = sym_name
            elif storage_class == 105:
              # weak alias
              if sym_name.startswith('__imp_'):
                sym_name = sym_name[6:]
              if ver.arch == '32':
                sym_name = undecorate_32(sym_name)
              n_aux = sym_fields[5]
              assert(n_aux == 1)
              index = struct.unpack('<I', data[sym_off + 18:sym_off + 22])[0]
              weak_externals[sym_name] = index
              i += 1

          dll = entry.name.lower()
          for n, i in weak_externals.items():
            if dll not in dynamic:
              dynamic[dll] = ImportSymbols(set(), set())
            dynamic[dll].alias.add(AliasImportSymbol(n, ImportKind.CODE, externals[i]))
      else:
        # static object
        static.add(entry.name)

  with TemporaryDirectory() as tmpdir:
    mri_content = [f'create {imp}\n']

    for dll_name, symbols in sorted(dynamic.items()):
      if ver.short_import:
        if len(symbols.normal) > 0:
          mri_content.append(f'addlib {dll_name}.normal.a\n')

          def_content = ['EXPORTS\n']
          for sym_name in symbols.normal:
            if sym_name.kind == ImportKind.CODE:
              def_content.append(f'{sym_name.name}\n')
            elif sym_name.kind == ImportKind.DATA:
              def_content.append(f'{sym_name.name} DATA\n')
            elif sym_name.kind == ImportKind.CONST:
              def_content.append(f'{sym_name.name} CONST\n')
            else:
              raise Exception(f'Unknown import symbol kind: {sym_name.kind}')
          with open(f'{tmpdir}/{dll_name}.normal.def', 'w') as f:
            f.write(''.join(def_content))
          subprocess.run(
            [
              'llvm-dlltool',
              '-m', DLLTOOL_ARCH_MAP[ver.arch],
              '-D', dll_name,
              '-k',
              '-d', f'{dll_name}.normal.def',
              '-l', f'{dll_name}.normal.a',
            ],
            cwd = tmpdir,
            check = True,
          )
        if len(symbols.alias) > 0:
          mri_content.append(f'addlib {dll_name}.alias.a\n')

          def_content = ['EXPORTS\n']
          for sym_name in symbols.alias:
            def_content.append(f'{sym_name.name} == {sym_name.target}\n')
          with open(f'{tmpdir}/{dll_name}.alias.def', 'w') as f:
            f.write(''.join(def_content))
          subprocess.run(
            [
              f'{ver.target}-dlltool',
              '-D', dll_name,
              '-k',
              '-d', f'{dll_name}.alias.def',
              '-l', f'{dll_name}.alias.a',
            ],
            cwd = tmpdir,
            check = True,
          )
      else:
        mri_content.append(f'addlib {dll_name}.a\n')

        def_content = ['EXPORTS\n']
        for sym_name in symbols.normal:
          if sym_name.kind == ImportKind.CODE:
            def_content.append(f'{sym_name.name}\n')
          elif sym_name.kind == ImportKind.DATA:
            def_content.append(f'{sym_name.name} DATA\n')
          elif sym_name.kind == ImportKind.CONST:
            def_content.append(f'{sym_name.name} CONST\n')
          else:
            raise Exception(f'Unknown import symbol kind: {sym_name.kind}')
        for sym_name in symbols.alias:
          def_content.append(f'{sym_name.name} == {sym_name.target}\n')
        with open(f'{tmpdir}/{dll_name}.def', 'w') as f:
          f.write(''.join(def_content))
        subprocess.run(
          [
            f'{ver.target}-dlltool',
            '-D', dll_name,
            '-k',
            '-d', f'{dll_name}.def',
            '-l', f'{dll_name}.a',
          ],
          cwd = tmpdir,
          check = True,
        )

    if len(static) > 0:
      subprocess.run(
        ['llvm-ar', 'x', imp0, *static],
        cwd = tmpdir,
        check = True,
      )
      for obj in static:
        mri_content.append(f'addmod {obj}\n')

    mri_content.append('save\n')
    mri_content.append('end\n')
    with open(f'{tmpdir}/mri', 'w') as f:
      f.write(''.join(mri_content))
    with open(f'{tmpdir}/mri') as f:
      subprocess.run(
        ['llvm-ar', '-M'],
        cwd = tmpdir,
        check = True,
        stdin = f,
      )

def postprocess_crt_import_libraries(
    ver: BranchProfile,
    crt0_lib_dir: Path,
    crt_lib_dir: Path,
    jobs: int,
):
    ensure(crt_lib_dir)

    with ThreadPoolExecutor(max_workers = jobs) as executor:
        futures = []

        for imp0 in crt0_lib_dir.glob('*'):
            assert(imp0.is_file())
            name = imp0.name
            if name.startswith('lib') and name.endswith('.a'):
                imp = crt_lib_dir / name
                futures.append(executor.submit(
                  process_single_import_library,
                  ver, imp0, imp,
                ))
            else:
                shutil.copy2(imp0, crt_lib_dir / name)

        for future in futures:
            future.result()
