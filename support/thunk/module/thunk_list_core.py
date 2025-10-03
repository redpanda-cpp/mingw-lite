THUNK_LIST_CORE = {
  '5.0': {
    'kernel32': [
      'CreateHardLinkW',
      'FindFirstVolumeW',
      'FindNextVolumeW',
      'FindVolumeClose',
      'GetFileSizeEx',
    ],
    'msvcrt': [
      ('_fstat64', lambda v_major: v_major < 13),
      ('_wstat64', lambda v_major: v_major < 13),
    ]
  },
  '5.1': {
    'kernel32': [
      # winpthreads: no longer required since 12
      ('AddVectoredExceptionHandler', lambda v_major: v_major < 12),
      ('RemoveVectoredExceptionHandler', lambda v_major: v_major < 12),
    ],
    'msvcrt': [
      ('_aligned_free', lambda v_major: v_major < 13),
      ('_aligned_malloc', lambda v_major: v_major < 13),
    ],
  },
  '5.2': {},
  '6.0': {},
}
