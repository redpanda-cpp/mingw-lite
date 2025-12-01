THUNK_LIST_CORE_WIN32 = {
  '3.9999+4.10': {
    'kernel32': [
      'GetFileAttributesExA',
      'IsDebuggerPresent',
      'SetProcessAffinityMask',
    ],
  },
  '4.0': {
    'kernel32': [
      'CreateFileW',
      'DeleteFileW',
      'FindFirstFileW',
      'FindNextFileW',
      'GetCurrentDirectoryW',
      'GetFileAttributesExW',
      'GetFileAttributesW',
      'GetFullPathNameW',
      'GetHandleInformation',
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

THUNK_LIST_CORE_MSVCRT = {
  '4.0': [
    '_beginthreadex',
    '_wchdir',
    '_wchmod',
    '_wfindfirst',
    '_wfindfirst32',
    '_wfindfirst32i64',
    '_wfindfirsti64',
    '_wfindnext',
    '_wfindnext32',
    '_wfindnext32i64',
    '_wfindnexti64',
    '_wfopen',
    '_wfullpath',
    '_wgetcwd',
    '_wmkdir',
    '_wopen',
    '_wstat32',
    ('_wstat32i64', lambda v_major : v_major >= 12),
    '_wstati64',
    '_wutime',
    '_wutime32',
  ],
  '5.0': [
    ('_fstat64', lambda v_major: v_major < 13),
    ('_wstat64', lambda v_major: v_major < 13),
  ],
  '5.1': [
    ('_aligned_free', lambda v_major: v_major < 13),
    ('_aligned_malloc', lambda v_major: v_major < 13),
  ],
}
