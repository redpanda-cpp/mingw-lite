#pragma once

#include <thunk/stl/algorithm.h>
#include <thunk/stl/iterator.h>

#include <errno.h>
#include <stdlib.h>

#include <windows.h>

namespace mingw_thunk::internal
{
  struct doserrmap_t
  {
    unsigned long dos;
    int c;
  };

  // ReactOS 0.4.15
  inline doserrmap_t doserrmap[] = {
      {ERROR_FILE_NOT_FOUND, ENOENT},
      {ERROR_PATH_NOT_FOUND, ENOENT},
      {ERROR_TOO_MANY_OPEN_FILES, EMFILE},
      {ERROR_ACCESS_DENIED, EACCES},
      {ERROR_INVALID_HANDLE, EBADF},
      {ERROR_ARENA_TRASHED, ENOMEM},
      {ERROR_NOT_ENOUGH_MEMORY, ENOMEM},
      {ERROR_INVALID_BLOCK, ENOMEM},
      {ERROR_BAD_ENVIRONMENT, E2BIG},
      {ERROR_BAD_FORMAT, ENOEXEC},
      {ERROR_INVALID_DRIVE, ENOENT},
      {ERROR_CURRENT_DIRECTORY, EACCES},
      {ERROR_NOT_SAME_DEVICE, EXDEV},
      {ERROR_NO_MORE_FILES, ENOENT},
      {ERROR_WRITE_PROTECT, EACCES},
      {ERROR_BAD_UNIT, EACCES},
      {ERROR_NOT_READY, EACCES},
      {ERROR_BAD_COMMAND, EACCES},
      {ERROR_CRC, EACCES},
      {ERROR_BAD_LENGTH, EACCES},
      {ERROR_SEEK, EACCES},
      {ERROR_NOT_DOS_DISK, EACCES},
      {ERROR_SECTOR_NOT_FOUND, EACCES},
      {ERROR_OUT_OF_PAPER, EACCES},
      {ERROR_WRITE_FAULT, EACCES},
      {ERROR_READ_FAULT, EACCES},
      {ERROR_GEN_FAILURE, EACCES},
      {ERROR_SHARING_VIOLATION, EACCES},
      {ERROR_LOCK_VIOLATION, EACCES},
      {ERROR_WRONG_DISK, EACCES},
      {ERROR_SHARING_BUFFER_EXCEEDED, EACCES},
      {ERROR_BAD_NETPATH, ENOENT},
      {ERROR_NETWORK_ACCESS_DENIED, EACCES},
      {ERROR_BAD_NET_NAME, ENOENT},
      {ERROR_FILE_EXISTS, EEXIST},
      {ERROR_CANNOT_MAKE, EACCES},
      {ERROR_FAIL_I24, EACCES},
      {ERROR_NO_PROC_SLOTS, EAGAIN},
      {ERROR_DRIVE_LOCKED, EACCES},
      {ERROR_BROKEN_PIPE, EPIPE},
      {ERROR_DISK_FULL, ENOSPC},
      {ERROR_INVALID_TARGET_HANDLE, EBADF},
      {ERROR_WAIT_NO_CHILDREN, ECHILD},
      {ERROR_CHILD_NOT_COMPLETE, ECHILD},
      {ERROR_DIRECT_ACCESS_HANDLE, EBADF},
      {ERROR_SEEK_ON_DEVICE, EACCES},
      {ERROR_DIR_NOT_EMPTY, ENOTEMPTY},
      {ERROR_NOT_LOCKED, EACCES},
      {ERROR_BAD_PATHNAME, ENOENT},
      {ERROR_MAX_THRDS_REACHED, EAGAIN},
      {ERROR_LOCK_FAILED, EACCES},
      {ERROR_ALREADY_EXISTS, EEXIST},
      {ERROR_INVALID_STARTING_CODESEG, ENOEXEC},
      {ERROR_INVALID_STACKSEG, ENOEXEC},
      {ERROR_INVALID_MODULETYPE, ENOEXEC},
      {ERROR_INVALID_EXE_SIGNATURE, ENOEXEC},
      {ERROR_EXE_MARKED_INVALID, ENOEXEC},
      {ERROR_BAD_EXE_FORMAT, ENOEXEC},
      {ERROR_ITERATED_DATA_EXCEEDS_64k, ENOEXEC},
      {ERROR_INVALID_MINALLOCSIZE, ENOEXEC},
      {ERROR_DYNLINK_FROM_INVALID_RING, ENOEXEC},
      {ERROR_IOPL_NOT_ENABLED, ENOEXEC},
      {ERROR_INVALID_SEGDPL, ENOEXEC},
      {ERROR_AUTODATASEG_EXCEEDS_64k, ENOEXEC},
      {ERROR_RING2SEG_MUST_BE_MOVABLE, ENOEXEC},
      {ERROR_RELOC_CHAIN_XEEDS_SEGLIM, ENOEXEC},
      {ERROR_INFLOOP_IN_RELOC_CHAIN, ENOEXEC},
      {ERROR_FILENAME_EXCED_RANGE, ENOENT},
      {ERROR_NESTING_NOT_ALLOWED, EAGAIN},
      {ERROR_NOT_ENOUGH_QUOTA, ENOMEM},
  };

  inline int dosmaperr(unsigned long oserror)
  {
    int err = EINVAL;
    if (auto it = internal::lower_bound(
            internal::cbegin(doserrmap),
            internal::cend(doserrmap),
            doserrmap_t{oserror, 0},
            [](doserrmap_t a, doserrmap_t b) { return a.dos < b.dos; });
        it != internal::end(doserrmap) && it->dos == oserror) {
      err = it->c;
    }
    _set_errno(err);
    return err;
  }

  inline int dosmaperr()
  {
    return dosmaperr(GetLastError());
  }
} // namespace mingw_thunk::internal
