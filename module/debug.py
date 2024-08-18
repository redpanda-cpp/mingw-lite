from pathlib import Path
import subprocess
from typing import Optional

def shell_here(path: Optional[Path] = None):
  if path is None:
    path = Path.cwd()
  if Path('/bin/bash').exists():
    subprocess.run('/bin/bash', cwd = path)
  else:
    subprocess.run('/bin/sh', cwd = path)
