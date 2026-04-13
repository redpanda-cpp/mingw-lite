#!/usr/bin/python3

import os
import sys

if os.getuid() == 0:
  os.execv(sys.executable, [sys.executable, '_xtest_impl.py'] + sys.argv[1:])
else:
  os.execvp('unshare', ['unshare', '--user', '--map-root-user', '--mount', sys.executable, '_xtest_impl.py'] + sys.argv[1:])
