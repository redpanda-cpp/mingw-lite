target('alias-short-kernel32')
  set_kind('static')

  if is_arch('i386', 'i686') then
    on_build(build_short_import_library({
      'def/lib32/kernel32.def',
      'def/lib32/kernel32_ntdll.def'}))
  else
    on_build(build_short_import_library({
      'def/kernel32.def',
      'def/kernel32_ntdll.def'}))
  end

target('overlay-kernel32')
  enable_thunk_options()

  if profile_toolchain_or_utf8() then
    if ntddi_version() < ntddi_win98() then
      add_files(
        'kernel32/3.9999+4.10/CancelIo.cc',
        'kernel32/3.9999+4.10/IsDebuggerPresent.cc',
        'kernel32/3.9999+4.10/SetProcessAffinityMask.cc',
        'kernel32/3.9999+4.10/SetWaitableTimer.cc',
        'kernel32/3.9999+4.10/SwitchToThread.cc')
    end
    if ntddi_version() < ntddi_win4() then
      add_files(
        'kernel32/4.0/CreateThread.cc',
        'kernel32/4.0/GetHandleInformation.cc',
        'kernel32/4.0/LockFileEx.cc',
        'kernel32/4.0/SetHandleInformation.cc',
        'kernel32/4.0/TryEnterCriticalSection.cc',
        'kernel32/4.0/UnlockFileEx.cc')
      add_files(
        'kernel32/w/CopyFileExW.cc',
        'kernel32/w/CopyFileW.cc',
        'kernel32/w/CreateDirectoryW.cc',
        'kernel32/w/CreateEventW.cc',
        'kernel32/w/CreateFileW.cc',
        'kernel32/w/CreateProcessW.cc',
        'kernel32/w/CreateSemaphoreW.cc',
        'kernel32/w/CreateWaitableTimerW.cc',
        'kernel32/w/DeleteFileW.cc',
        'kernel32/w/FindFirstFileW.cc',
        'kernel32/w/FindNextFileW.cc',
        'kernel32/w/GetCurrentDirectoryW.cc',
        'kernel32/w/GetEnvironmentVariableW.cc',
        'kernel32/w/GetFileAttributesExW.cc',
        'kernel32/w/GetFileAttributesW.cc',
        'kernel32/w/GetFullPathNameW.cc',
        'kernel32/w/GetModuleFileNameW.cc',
        'kernel32/w/GetTempPathW.cc',
        'kernel32/w/MoveFileExW.cc',
        'kernel32/w/ReadDirectoryChangesW.cc',
        'kernel32/w/RemoveDirectoryW.cc',
        'kernel32/w/SetCurrentDirectoryW.cc',
        'kernel32/w/SetEnvironmentVariableW.cc',
        'kernel32/w/WriteConsoleW.cc')
    end
    if ntddi_version() < ntddi_win2k() then
      add_files(
        'kernel32/5.0/CreateHardLinkW.cc',
        'kernel32/5.0/CreateToolhelp32Snapshot.cc',
        'kernel32/5.0/FindFirstVolumeW.cc',
        'kernel32/5.0/FindNextVolumeW.cc',
        'kernel32/5.0/FindVolumeClose.cc',
        'kernel32/5.0/GetCPInfoExW.cc',
        'kernel32/5.0/GetFileSizeEx.cc',
        'kernel32/5.0/GetLongPathNameW.cc',
        'kernel32/5.0/GetVolumePathNameW.cc',
        'kernel32/5.0/GlobalMemoryStatusEx.cc',
        'kernel32/5.0/Module32First.cc',
        'kernel32/5.0/Module32Next.cc',
        'kernel32/5.0/OpenThread.cc',
        'kernel32/5.0/SetFilePointerEx.cc')
    end
    if ntddi_version() < ntddi_winxp() then
      add_files(
        'kernel32/5.1/AddVectoredExceptionHandler.cc',
        'kernel32/5.1/GetConsoleProcessList.cc',
        'kernel32/5.1/GetModuleHandleExW.cc',
        'kernel32/5.1/GetNumaHighestNodeNumber.cc',
        'kernel32/5.1/GetNumaNodeProcessorMask.cc',
        'kernel32/5.1/GetSystemTimes.cc',
        'kernel32/5.1/GetSystemWow64DirectoryW.cc',
        'kernel32/5.1/GetVolumePathNamesForVolumeNameW.cc',
        'kernel32/5.1/LCMapStringW.cc',
        'kernel32/5.1/RemoveVectoredExceptionHandler.cc')
    end
    if ntddi_version() < ntddi_ws03() then
      add_files(
        'kernel32/5.2/FlsAlloc.cc',
        'kernel32/5.2/FlsFree.cc',
        'kernel32/5.2/FlsGetValue.cc',
        'kernel32/5.2/FlsSetValue.cc',
        'kernel32/5.2/GetCurrentProcessorNumber.cc',
        'kernel32/5.2/GetLargePageMinimum.cc',
        'kernel32/5.2/NeedCurrentDirectoryForExePathW.cc',
        'kernel32/5.2/SetThreadStackGuarantee.cc')
    end
    if ntddi_version() < ntddi_vista() then
      add_files(
        'kernel32/6.0/CancelIoEx.cc',
        'kernel32/6.0/CompareStringOrdinal.cc',
        'kernel32/6.0/CreateSymbolicLinkW.cc',
        'kernel32/6.0/CreateWaitableTimerExW.cc',
        'kernel32/6.0/DeleteProcThreadAttributeList.cc',
        'kernel32/6.0/GetFileInformationByHandleEx.cc',
        'kernel32/6.0/GetFinalPathNameByHandleW.cc',
        'kernel32/6.0/GetTickCount64.cc',
        'kernel32/6.0/InitializeProcThreadAttributeList.cc',
        'kernel32/6.0/LCMapStringEx.cc',
        'kernel32/6.0/LocaleNameToLCID.cc',
        'kernel32/6.0/SetFileInformationByHandle.cc',
        'kernel32/6.0/UpdateProcThreadAttribute.cc')
    end
    if ntddi_version() < ntddi_win7() then
      add_files(
        'kernel32/6.1/GetActiveProcessorCount.cc',
        'kernel32/6.1/GetActiveProcessorGroupCount.cc',
        'kernel32/6.1/GetMaximumProcessorCount.cc',
        'kernel32/6.1/GetMaximumProcessorGroupCount.cc',
        'kernel32/6.1/SetWaitableTimerEx.cc')
    end
    if ntddi_version() < ntddi_win8() then
      add_files(
        'kernel32/6.2/AddDllDirectory.cc',
        'kernel32/6.2/CopyFile2.cc',
        'kernel32/6.2/GetCurrentThreadStackLimits.cc',
        'kernel32/6.2/GetSystemTimePreciseAsFileTime.cc',
        'kernel32/6.2/RemoveDllDirectory.cc')
    end
  end

  if profile_toolchain() then
    if ntddi_version() < ntddi_win98() then
      add_files(
        'kernel32/a/3.9999+4.10/CreateWaitableTimerA.cc',
        'kernel32/a/3.9999+4.10/GetFileAttributesExA.cc',
        'kernel32/a/3.9999+4.10/MultiByteToWideChar.cc',
        'kernel32/a/3.9999+4.10/MultiByteToWideChar.impl.cc',
        'kernel32/a/3.9999+4.10/WideCharToMultiByte.cc',
        'kernel32/a/3.9999+4.10/WideCharToMultiByte.impl.cc')
    end
    if ntddi_version() < ntddi_win4() then
      add_files(
        'kernel32/a/4.0/CopyFileExA.cc',
        'kernel32/a/4.0/MoveFileExA.cc')
    end
    if ntddi_version() < ntddi_win2k() then
      add_files('kernel32/a/5.0/GetCPInfoExA.cc')
    end
    if ntddi_version() < ntddi_winxp() then
      add_files('kernel32/a/5.1/GetSystemWow64DirectoryA.cc')
    end
    if ntddi_version() < ntddi_ws03() then
      add_files('kernel32/a/5.2/NeedCurrentDirectoryForExePathA.cc')
    end
    if ntddi_version() < ntddi_vista() then
      add_files(
        'kernel32/a/6.0/CreateSymbolicLinkA.cc',
        'kernel32/a/6.0/GetFinalPathNameByHandleA.cc')
    end
  end

  if profile_core() then
    if ntddi_version() < ntddi_win98() then
      add_files(
        'kernel32/3.9999+4.10/IsDebuggerPresent.cc',
        'kernel32/3.9999+4.10/SetProcessAffinityMask.cc')
      add_files(
        'kernel32/a/3.9999+4.10/GetFileAttributesExA.cc')
    end
    if ntddi_version() < ntddi_win4() then
      add_files(
        'kernel32/4.0/GetHandleInformation.cc',
        'kernel32/4.0/TryEnterCriticalSection.cc')
      add_files(
        'kernel32/a/4.0/MoveFileExA.cc')
      add_files(
        'kernel32/w/CreateFileW.cc',
        'kernel32/w/DeleteFileW.cc',
        'kernel32/w/FindFirstFileW.cc',
        'kernel32/w/FindNextFileW.cc',
        'kernel32/w/GetCurrentDirectoryW.cc',
        'kernel32/w/GetFileAttributesExW.cc',
        'kernel32/w/GetFileAttributesW.cc',
        'kernel32/w/GetFullPathNameW.cc',
        'kernel32/w/MoveFileExW.cc',
        'kernel32/w/RemoveDirectoryW.cc',
        'kernel32/w/WriteConsoleW.cc')
    end
    if ntddi_version() < ntddi_win2k() then
      add_files(
        'kernel32/5.0/CreateHardLinkW.cc',
        'kernel32/5.0/FindFirstVolumeW.cc',
        'kernel32/5.0/FindNextVolumeW.cc',
        'kernel32/5.0/FindVolumeClose.cc',
        'kernel32/5.0/GetFileSizeEx.cc')
    end
    if ntddi_version() < ntddi_winxp() then
      add_files(
        'kernel32/5.1/AddVectoredExceptionHandler.cc',
        'kernel32/5.1/RemoveVectoredExceptionHandler.cc')
    end
  end

target('alias-long-kernel32')
  set_kind('static')
  skip_install()

  if is_arch('i386', 'i686') then
    on_build(build_long_import_library({
      'def/lib32/kernel32.def',
      'def/lib32/kernel32_ntdll.def'}))
  else
    on_build(build_long_import_library({
      'def/kernel32.def',
      'def/kernel32_ntdll.def'}))
  end

target('thunk-kernel32')
  add_files(
    'kernel32/6.0/CancelIoEx.cc',
    'kernel32/6.0/CompareStringOrdinal.cc',
    'kernel32/6.0/CreateSymbolicLinkW.cc',
    'kernel32/6.0/CreateWaitableTimerExW.cc',
    'kernel32/6.0/DeleteProcThreadAttributeList.cc',
    'kernel32/6.0/GetFileInformationByHandleEx.cc',
    'kernel32/6.0/GetFinalPathNameByHandleW.cc',
    'kernel32/6.0/GetTickCount64.cc',
    'kernel32/6.0/InitializeProcThreadAttributeList.cc',
    'kernel32/6.0/LCMapStringEx.cc',
    'kernel32/6.0/LocaleNameToLCID.cc',
    'kernel32/6.0/SetFileInformationByHandle.cc',
    'kernel32/6.0/UpdateProcThreadAttribute.cc',
    'kernel32/6.1/GetActiveProcessorCount.cc',
    'kernel32/6.1/GetActiveProcessorGroupCount.cc',
    'kernel32/6.1/GetMaximumProcessorCount.cc',
    'kernel32/6.1/GetMaximumProcessorGroupCount.cc',
    'kernel32/6.1/SetWaitableTimerEx.cc',
    'kernel32/6.2/AddDllDirectory.cc',
    'kernel32/6.2/CopyFile2.cc',
    'kernel32/6.2/GetCurrentThreadStackLimits.cc',
    'kernel32/6.2/GetSystemTimePreciseAsFileTime.cc',
    'kernel32/6.2/RemoveDllDirectory.cc')
  enable_if_x86()
  enable_thunk_options()
  merge_win32_alias()
  skip_install()

  if is_arch('i386', 'i686') then
    add_files(
      'kernel32/3.9999+4.10/CancelIo.cc',
      'kernel32/3.9999+4.10/IsDebuggerPresent.cc',
      'kernel32/3.9999+4.10/SetProcessAffinityMask.cc',
      'kernel32/3.9999+4.10/SetWaitableTimer.cc',
      'kernel32/3.9999+4.10/SwitchToThread.cc',
      'kernel32/4.0/CreateThread.cc',
      'kernel32/4.0/GetHandleInformation.cc',
      'kernel32/4.0/LockFileEx.cc',
      'kernel32/4.0/SetHandleInformation.cc',
      'kernel32/4.0/TryEnterCriticalSection.cc',
      'kernel32/4.0/UnlockFileEx.cc',
      'kernel32/5.0/CreateHardLinkW.cc',
      'kernel32/5.0/CreateToolhelp32Snapshot.cc',
      'kernel32/5.0/FindFirstVolumeW.cc',
      'kernel32/5.0/FindNextVolumeW.cc',
      'kernel32/5.0/FindVolumeClose.cc',
      'kernel32/5.0/GetFileSizeEx.cc',
      'kernel32/5.0/GetLongPathNameW.cc',
      'kernel32/5.0/GetVolumePathNameW.cc',
      'kernel32/5.0/GlobalMemoryStatusEx.cc',
      'kernel32/5.0/Module32First.cc',
      'kernel32/5.0/Module32Next.cc',
      'kernel32/5.0/OpenThread.cc',
      'kernel32/5.0/SetFilePointerEx.cc',
      'kernel32/5.1/AddVectoredExceptionHandler.cc',
      'kernel32/5.1/GetConsoleProcessList.cc',
      'kernel32/5.1/GetModuleHandleExW.cc',
      'kernel32/5.1/GetNumaHighestNodeNumber.cc',
      'kernel32/5.1/GetNumaNodeProcessorMask.cc',
      'kernel32/5.1/GetSystemTimes.cc',
      'kernel32/5.1/GetSystemWow64DirectoryW.cc',
      'kernel32/5.1/GetVolumePathNamesForVolumeNameW.cc',
      'kernel32/5.1/LCMapStringW.cc',
      'kernel32/5.1/RemoveVectoredExceptionHandler.cc',
      'kernel32/5.2/FlsAlloc.cc',
      'kernel32/5.2/FlsFree.cc',
      'kernel32/5.2/FlsGetValue.cc',
      'kernel32/5.2/FlsSetValue.cc',
      'kernel32/5.2/GetCurrentProcessorNumber.cc',
      'kernel32/5.2/GetLargePageMinimum.cc',
      'kernel32/5.2/NeedCurrentDirectoryForExePathW.cc',
      'kernel32/5.2/SetThreadStackGuarantee.cc')
  end

target('test-kernel32')
  add_tests('default')
  add_deps('thunk-kernel32')
  add_files(
    'kernel32/6.2/CopyFile2.test.cc',
    'kernel32/6.2/GetSystemTimePreciseAsFileTime.test.cc')
  enable_if_x86()
  enable_test_options()
  skip_install()

target('thunk-kernel32-a')
  add_files(
    'kernel32/a/6.0/CreateSymbolicLinkA.cc',
    'kernel32/a/6.0/GetFinalPathNameByHandleA.cc')
  enable_if_x86()
  enable_thunk_options()
  merge_win32_alias()
  skip_install()

  if is_arch('i386', 'i686') then
    add_files(
      'kernel32/a/3.9999+4.10/CreateWaitableTimerA.cc',
      'kernel32/a/3.9999+4.10/GetFileAttributesExA.cc',
      'kernel32/a/3.9999+4.10/MultiByteToWideChar.cc',
      'kernel32/a/3.9999+4.10/MultiByteToWideChar.impl.cc',
      'kernel32/a/3.9999+4.10/WideCharToMultiByte.cc',
      'kernel32/a/3.9999+4.10/WideCharToMultiByte.impl.cc',
      'kernel32/a/4.0/CopyFileExA.cc',
      'kernel32/a/4.0/MoveFileExA.cc',
      'kernel32/a/5.0/GetCPInfoExA.cc',
      'kernel32/a/5.1/GetSystemWow64DirectoryA.cc',
      'kernel32/a/5.2/NeedCurrentDirectoryForExePathA.cc')
  end

target('test-kernel32-a')
  add_tests('default')
  add_deps('thunk-kernel32-a')
  add_files(
    'kernel32/a/3.9999+4.10/MultiByteToWideChar.test.cc',
    'kernel32/a/3.9999+4.10/WideCharToMultiByte.test.cc')
  enable_if_x86_32()
  enable_test_options()
  skip_install()

target('thunk-kernel32-w')
  add_files(
    'kernel32/w/CopyFileExW.cc',
    'kernel32/w/CopyFileW.cc',
    'kernel32/w/CreateDirectoryW.cc',
    'kernel32/w/CreateEventW.cc',
    'kernel32/w/CreateFileW.cc',
    'kernel32/w/CreateProcessW.cc',
    'kernel32/w/CreateSemaphoreW.cc',
    'kernel32/w/CreateWaitableTimerW.cc',
    'kernel32/w/DeleteFileW.cc',
    'kernel32/w/FindFirstFileW.cc',
    'kernel32/w/FindNextFileW.cc',
    'kernel32/w/GetCurrentDirectoryW.cc',
    'kernel32/w/GetEnvironmentVariableW.cc',
    'kernel32/w/GetFileAttributesExW.cc',
    'kernel32/w/GetFileAttributesW.cc',
    'kernel32/w/GetFullPathNameW.cc',
    'kernel32/w/GetModuleFileNameW.cc',
    'kernel32/w/GetTempPathW.cc',
    'kernel32/w/MoveFileExW.cc',
    'kernel32/w/ReadDirectoryChangesW.cc',
    'kernel32/w/RemoveDirectoryW.cc',
    'kernel32/w/SetCurrentDirectoryW.cc',
    'kernel32/w/SetEnvironmentVariableW.cc',
    'kernel32/w/WriteConsoleW.cc')
  enable_if_x86_32()
  enable_thunk_options()
  merge_win32_alias()
  skip_install()

target('test-kernel32-w')
  add_tests('default')
  add_deps('thunk-kernel32-w')
  add_files('kernel32/w/GetFullPathNameW.test.cc')
  enable_if_x86_32()
  enable_test_options()
  skip_install()
