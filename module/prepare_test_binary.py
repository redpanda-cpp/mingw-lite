import argparse
import logging
import os
from packaging.version import Version
from pathlib import Path
from shutil import copyfile
import subprocess

from module.fetch import validate_and_download, check_and_extract
from module.path import ProjectPaths
from module.profile import BranchVersions, ProfileInfo

def _gcc_mingw(ver: BranchVersions, paths: ProjectPaths):
  check_and_extract(paths.test_mingw, paths.mingw_pkg)
  (paths.test_mingw / '.patched').touch()

def _xmake(ver: BranchVersions, paths: ProjectPaths):
  url = f'https://github.com/xmake-io/xmake/releases/download/v{ver.xmake}/{paths.xmake_arx.name}'
  validate_and_download(paths.xmake_arx, url)
  check_and_extract(paths.xmake, paths.xmake_arx)
  paths.xmake_exe.chmod(0o755)
  (paths.xmake / '.patched').touch()

def prepare_test_binary(ver: BranchVersions, paths: ProjectPaths, config: argparse.Namespace):
  _gcc_mingw(ver, paths)

  _xmake(ver, paths)
