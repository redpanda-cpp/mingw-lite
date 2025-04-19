from packaging.version import Version

THUNK_LIST_TOOLCHAIN = {
  '4.0': {},
  '5.0': {},
  '5.1': {},
  '5.2': {},
  '6.0': {},
  '6.1': {
    'kernel32': [
      'GetActiveProcessorCount',
      'GetActiveProcessorGroupCount',
      'GetMaximumProcessorCount',
      'GetMaximumProcessorGroupCount',
      'SetWaitableTimerEx',
    ],
  },
  '6.2': {
    'api-ms-win-core-path-l1-1-0': [
      'PathAllocCanonicalize',
      'PathAllocCombine',
      'PathCchAddBackslash',
      'PathCchAddBackslashEx',
      'PathCchAddExtension',
      'PathCchAppend',
      'PathCchAppendEx',
      'PathCchCanonicalize',
      'PathCchCanonicalizeEx',
      'PathCchCombine',
      'PathCchCombineEx',
      'PathCchFindExtension',
      'PathCchIsRoot',
      'PathCchRemoveBackslash',
      'PathCchRemoveBackslashEx',
      'PathCchRemoveExtension',
      'PathCchRemoveFileSpec',
      'PathCchRenameExtension',
      'PathCchSkipRoot',
      'PathCchStripPrefix',
      'PathCchStripToRoot',
      'PathIsUNCEx',
    ],
    'kernel32': [
      'CopyFile2',
      'GetSystemTimePreciseAsFileTime',
    ],
  },
}
