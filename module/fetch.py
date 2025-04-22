from hashlib import sha256
import logging
from pathlib import Path
import subprocess
import time
from urllib.error import URLError
from urllib.request import urlopen

from module.checksum import CHECKSUMS

def validate_and_download(path: Path, url: str):
  MAX_RETRY = 5
  checksum = CHECKSUMS[path.name]
  if path.exists():
    with open(path, 'rb') as f:
      body = f.read()
      if checksum != sha256(body).hexdigest():
        message = 'Validate fail: %s exists but checksum mismatch' % path.name
        logging.critical(message)
        logging.info('Please delete %s and try again' % path.name)
        raise Exception(message)
  else:
    logging.info('Downloading %s' % path.name)
    retry_count = 0
    while True:
      retry_count += 1
      try:
        response = urlopen(url)
        body = response.read()
        if checksum != sha256(body).hexdigest():
          message = 'Download fail: checksum mismatch for %s' % path.name
          logging.critical(message)
          raise Exception(message)
        with open(path, "wb") as f:
          f.write(body)
          return
      except URLError as e:
        message = f'Download fail: {e.reason} (retry {retry_count}/{MAX_RETRY})'
        if retry_count < MAX_RETRY:
          logging.warning(message)
          wait_time = 2 ** retry_count
          logging.warning(f'Retry in {wait_time} seconds...')
          time.sleep(wait_time)
        else:
          logging.critical(message)
          raise e

def check_and_extract(path: Path, arx: Path):
  # check if already extracted
  if path.exists():
    mark = path / '.patched'
    if mark.exists():
      return False
    else:
      message = 'Extract fail: %s exists but not marked as fully patched' % path.name
      logging.critical(message)
      logging.info('Please delete %s and try again' % path.name)
      raise Exception(message)

  # extract
  res = subprocess.run([
    'bsdtar',
    '-xf',
    arx,
    '--no-same-owner',
  ], cwd = path.parent)
  if res.returncode != 0:
    message = 'Extract fail: bsdtar returned %d extracting %s' % (res.returncode, arx.name)
    logging.critical(message)
    raise Exception(message)

  return True
