from concurrent.futures import Future, ThreadPoolExecutor
from dataclasses import dataclass
import json
import logging
from pathlib import Path
import shutil
import subprocess
from tempfile import TemporaryDirectory
from typing import Dict, List, NamedTuple, Optional, Set, Tuple, TypedDict

from module.profile import BranchProfile
from module.path import ProjectPaths
from module.util import ensure

DLLTOOL_ARCH_MAP = {
  '32': 'i386',
  '64': 'i386:x86-64',
  'arm64': 'arm64',
}

StaticObjects = Dict[str, List[str]]

class _SidecarExportBase(TypedDict):
  name: str

class SidecarExport(_SidecarExportBase, total=False):
  target: str
  ordinal: int
  constant: bool
  data: bool
  private: bool

class SidecarDllExports(TypedDict):
  exports: List[SidecarExport]

class Sidecar(TypedDict):
  imports: Dict[str, SidecarDllExports]
  static_objects: StaticObjects

class ExportEntry(NamedTuple):
  name: str
  target: Optional[str] = None
  ordinal: Optional[int] = None
  constant: bool = False
  data: bool = False
  private: bool = False

class ImportSymbols(NamedTuple):
  normal: Set[ExportEntry]
  alias: Set[ExportEntry]

DynamicSymbols = Dict[str, ImportSymbols]
ThunkMap = Dict[str, Dict[str, str]]

@dataclass
class ImportOverlay:
  static: Path
  alias: Optional[Path] = None

def read_sidecar(lib_path: Path) -> Optional[Sidecar]:
  sidecar_path = Path(str(lib_path) + '.import-info.json')
  if not sidecar_path.is_file():
    return None
  with open(sidecar_path) as f:
    return json.load(f)

def _def_line(entry: ExportEntry) -> str:
  base = entry.name
  if entry.ordinal is not None:
    base = f'{base} @{entry.ordinal}'
  qualifiers: List[str] = []
  if entry.constant:
    qualifiers.append('CONSTANT')
  if entry.data:
    qualifiers.append('DATA')
  if entry.private:
    qualifiers.append('PRIVATE')
  if qualifiers:
    base = base + ' ' + ' '.join(qualifiers)
  if entry.target is not None:
    base = f'{base} == {entry.target}'
  return base

def sidecar_to_dynamic(sidecar: Sidecar, overlay_symbols: Set[str]) -> DynamicSymbols:
  dynamic: DynamicSymbols = {}

  for dll_name, dll_data in sidecar['imports'].items():
    normal: Set[ExportEntry] = set()
    alias: Set[ExportEntry] = set()

    for entry in dll_data['exports']:
      if entry['name'] in overlay_symbols:
        continue
      e = ExportEntry(entry['name'], entry.get('target'), entry.get('ordinal'), entry.get('constant', False), entry.get('data', False), entry.get('private', False))
      if e.target is not None:
        alias.add(e)
      else:
        normal.add(e)

    if normal or alias:
      dynamic[dll_name] = ImportSymbols(normal, alias)

  return dynamic

def extract_overlay_symbols(overlay_path: Path, ver: BranchProfile) -> Tuple[Set[str], Set[str]]:
  result = subprocess.run(
    ['llvm-nm', '-g', str(overlay_path)],
    capture_output = True,
    text = True,
    check = True,
  )

  overlay_symbols: Set[str] = set()
  force_overrides: Set[str] = set()

  for line in result.stdout.splitlines():
    parts = line.split()
    if len(parts) < 3:
      continue
    sym_name = parts[2]

    if sym_name.startswith('__imp_'):
      name = sym_name[6:]
      if ver.arch == '32' and name and name[0] == '_':
        name = name[1:]
      overlay_symbols.add(name)
    elif sym_name.startswith('__force_override_mingw_emu__imp_'):
      name = sym_name[len('__force_override_mingw_emu__imp_'):]
      if ver.arch == '32' and name and name[0] == '_':
        name = name[1:]
      force_overrides.add(name)

  return overlay_symbols, force_overrides

def check_static_objects(
  imp0: Path,
  static_objects: StaticObjects,
  overlay_symbols: Set[str],
  overlay_force_override: Set[str],
  ver: BranchProfile,
) -> Set[str]:
  if not static_objects:
    return set()

  def _strip_imp(raw_sym: str) -> str:
    name = raw_sym[6:]
    if ver.arch == '32' and name and name[0] == '_':
      name = name[1:]
    return name

  result: Set[str] = set()
  for obj_name, syms in static_objects.items():
    blocked = False
    for raw_sym in syms:
      if raw_sym.startswith('__force_override_mingw_emu__imp_'):
        continue
      name = _strip_imp(raw_sym)
      if name in overlay_symbols and name not in overlay_force_override:
        raise Exception(f'Unexpected override of {name} in {obj_name} ({imp0})')
      if name in overlay_force_override:
        blocked = True
    if not blocked:
      result.add(obj_name)
  return result

def read_alias_sidecar_dynamic(sidecar: Sidecar) -> Tuple[DynamicSymbols, Set[str]]:
  dynamic: DynamicSymbols = {}
  revert_symbols: Set[str] = set()

  for dll_name, dll_data in sidecar['imports'].items():
    normal: Set[ExportEntry] = set()
    alias: Set[ExportEntry] = set()

    for entry in dll_data['exports']:
      e = ExportEntry(entry['name'], entry.get('target'), entry.get('ordinal'), entry.get('constant', False), entry.get('data', False), entry.get('private', False))
      if e.target is not None:
        alias.add(e)
        if e.name.startswith('__ms_'):
          revert_symbols.add(e.name[5:])
      else:
        normal.add(e)

    if normal or alias:
      dynamic[dll_name] = ImportSymbols(normal, alias)

  return dynamic, revert_symbols

def process_single_import_library(
  ver: BranchProfile,
  overlay: Optional[ImportOverlay],
  imp0: Path,
  imp: Path,
  assert_thunk_free: bool,
  assert_thunk_revertible: bool,
) -> Dict[str, str]:
  overlay_symbols: Set[str] = set()
  overlay_force_override: Set[str] = set()
  revert_symbols: Set[str] = set()
  dynamic: DynamicSymbols = {}
  result_names: Set[str] = set()

  if overlay:
    overlay_symbols, overlay_force_override = extract_overlay_symbols(overlay.static, ver)

    if overlay.alias:
      alias_sidecar = read_sidecar(overlay.alias)
      if not alias_sidecar:
        raise Exception(f'Missing .import-info.json sidecar for overlay alias library: {overlay.alias}')
      alias_dynamic, alias_revert = read_alias_sidecar_dynamic(alias_sidecar)
      revert_symbols = alias_revert
      for dll_name, symbols in alias_dynamic.items():
        if dll_name not in dynamic:
          dynamic[dll_name] = ImportSymbols(set(), set())
        dynamic[dll_name].normal.update(symbols.normal)
        dynamic[dll_name].alias.update(symbols.alias)

    if assert_thunk_free:
      if overlay_symbols:
        raise AssertionError(f'ABI stability broken: {overlay_symbols}')

    if assert_thunk_revertible:
      non_revertible = overlay_symbols - revert_symbols
      if non_revertible:
        raise AssertionError(f'Non-revertible thunks: {non_revertible}')

    if not overlay_symbols:
      overlay = None
      dynamic = {}

  base_sidecar: Optional[Sidecar] = read_sidecar(imp0)
  if not base_sidecar:
    raise Exception(f'Missing .import-info.json sidecar for input library: {imp0}')
  base_dynamic: DynamicSymbols = sidecar_to_dynamic(base_sidecar, overlay_symbols)
  for dll_name, symbols in base_dynamic.items():
    if dll_name not in dynamic:
      dynamic[dll_name] = ImportSymbols(set(), set())
    dynamic[dll_name].normal.update(symbols.normal)
    dynamic[dll_name].alias.update(symbols.alias)
  static_objects: StaticObjects = base_sidecar['static_objects']

  if overlay and static_objects and overlay_symbols:
    result_names = check_static_objects(imp0, static_objects, overlay_symbols, overlay_force_override, ver)
  else:
    if static_objects:
      result_names = set(static_objects.keys())

  with TemporaryDirectory() as tmpdir:
    mri_content: List[str] = [f'create {imp}\n']

    for dll_name, symbols in sorted(dynamic.items()):
      if ver.short_import:
        if len(symbols.normal) > 0:
          mri_content.append(f'addlib {dll_name}.normal.a\n')

          def_content: List[str] = ['EXPORTS\n']
          for entry in symbols.normal:
            def_content.append(f'{_def_line(entry)}\n')
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

          def_content: List[str] = ['EXPORTS\n']
          for entry in symbols.alias:
            def_content.append(f'{_def_line(entry)}\n')
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

        def_content: List[str] = ['EXPORTS\n']
        for entry in symbols.normal:
          def_content.append(f'{_def_line(entry)}\n')
        for entry in symbols.alias:
          def_content.append(f'{_def_line(entry)}\n')
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

    if overlay:
      mri_content.append(f'addlib {overlay.static}\n')

    if result_names:
      subprocess.run(
        ['llvm-ar', 'x', imp0, *result_names],
        cwd = tmpdir,
        check = True,
      )
      for obj in sorted(result_names):
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

  return {name: f'__ms_{name}' for name in sorted(overlay_symbols)}

def postprocess_crt_import_libraries(
  ver: BranchProfile,
  thunk_lib_dir: Path,
  crt0_lib_dir: Path,
  crt_lib_dir: Path,
  assert_thunk_free: bool,
  assert_thunk_revertible: bool,
  jobs: int,
) -> ThunkMap:
  ensure(crt_lib_dir)

  thunk_map: ThunkMap = {}

  with ThreadPoolExecutor(max_workers = jobs) as executor:
    futures: List[Future[Dict[str, str]]] = []
    lib_names: List[str] = []

    for imp0 in crt0_lib_dir.glob('*'):
      assert(imp0.is_file())
      file_name = imp0.name

      if file_name.startswith('lib') and file_name.endswith('.a'):
        if file_name == 'libmsvcrt.a':
          continue
        overlay = None
        lib_name = file_name[3:-2]

        overlay_file_name = f'liboverlay-{lib_name}.a'
        if (thunk_lib_dir / overlay_file_name).exists():
          overlay = ImportOverlay(static = thunk_lib_dir / overlay_file_name)
          alias_file_name = f'libalias-short-{lib_name}.a'
          if (thunk_lib_dir / alias_file_name).exists():
            overlay.alias = thunk_lib_dir / alias_file_name

        imp = crt_lib_dir / file_name
        futures.append(executor.submit(
          process_single_import_library,
          ver, overlay, imp0, imp, assert_thunk_free, assert_thunk_revertible,
        ))
        lib_names.append(lib_name)
      else:
        shutil.copy2(imp0, crt_lib_dir / file_name)

    for lib_name, future in zip(lib_names, futures):
      result = future.result()
      if result:
        thunk_map[lib_name] = result

  return thunk_map

def generate_thunk_revert_map(thunk_map: ThunkMap, output_path: Path):
  if not thunk_map:
    return
  ensure(output_path.parent)
  with open(output_path, 'w') as f:
    json.dump(thunk_map, f, indent=2, sort_keys=True)
    f.write('\n')
