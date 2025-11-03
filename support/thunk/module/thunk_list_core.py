THUNK_LIST_CORE = {
  '3.9999+4.10': {
    'kernel32': [
      'IsDebuggerPresent',
      'SetProcessAffinityMask',
    ],
  },
  '4.0': {
    'kernel32': [
      'CreateFileW',
      'DeleteFileW',
      'EnterCriticalSection',
      'FindFirstFileW',
      'FindNextFileW',
      'GetCurrentDirectoryW',
      'GetFileAttributesExW',
      'GetFileAttributesW',
      'GetFullPathNameW',
      'GetHandleInformation',
      'LeaveCriticalSection',
      'MoveFileExA',
      'MoveFileExW',
      'RemoveDirectoryW',
      'TryEnterCriticalSection',
      'WriteConsoleW',
    ],
  },
  '5.0': {
    'kernel32': [
      'CreateHardLinkW',
      'FindFirstVolumeW',
      'FindNextVolumeW',
      'FindVolumeClose',
      'GetFileSizeEx',
    ],
  },
  '5.1': {
    'kernel32': [
      # winpthreads: no longer required since 12
      # but still required by python (see thunk_list_toolchain)
      ('AddVectoredExceptionHandler', lambda v_major: v_major < 12),
      ('RemoveVectoredExceptionHandler', lambda v_major: v_major < 12),
    ],
  },
  '5.2': {},
  '6.0': {},
}
