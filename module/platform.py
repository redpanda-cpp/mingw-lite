import os
import sys

def is_genuine_linux() -> bool:
  return sys.platform == 'linux' and not is_wsl1()

def is_linux() -> bool:
  return sys.platform == 'linux'

def is_win32() -> bool:
  return sys.platform == 'win32'

def is_wsl1() -> bool:
  # https://github.com/systemd/systemd/blob/v260.1/src/basic/virt.c#L656
  # https://github.com/Microsoft/WSL/issues/423#issuecomment-221627364
  # here we only detect WSL 1 (WSL 2 is considered “genuine Linux”)
  #   WSL 1: 4.4.0-19041-Microsoft
  #   WSL 2: 6.6.87.2-microsoft-standard-WSL2
  return is_linux() and os.uname().release.endswith('-Microsoft')

def win32_set_utf8_pipe():
  if is_win32():
    sys.stdout.reconfigure(encoding = 'UTF-8')
