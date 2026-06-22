#!/usr/bin/python3

import os
import sys

from module.platform import is_genuine_linux, is_wsl1

def direct_run():
  os.execv(sys.executable, [sys.executable, '_xtest_impl.py'] + sys.argv[1:])

def unshare_run():
  os.execvp('unshare', ['unshare', '--user', '--map-root-user', '--mount', sys.executable, '_xtest_impl.py'] + sys.argv[1:])

if is_genuine_linux():
  if os.getuid() == 0:
    direct_run()
  else:
    unshare_run()
elif is_wsl1():
  if os.getuid() != 0:
    raise RuntimeError('WSL1: mount as user is impossible. please run as root.')
  direct_run()
else:
  raise RuntimeError('Unsupported platform')
