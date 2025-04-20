import argparse
import logging
import os
from packaging.version import Version
from pathlib import Path
from shutil import copyfile
import subprocess

from module.fetch import validate_and_download, check_and_extract
from module.path import ProjectPaths
from module.profile import BranchProfile

def _gcc_mingw(ver: BranchProfile, paths: ProjectPaths):
  check_and_extract(paths.test_mingw, paths.mingw_pkg)
  (paths.test_mingw / 'bin' / 'xmake.exe').chmod(0o755)
  (paths.test_mingw / '.patched').touch()

def prepare_test_binary(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  _gcc_mingw(ver, paths)
