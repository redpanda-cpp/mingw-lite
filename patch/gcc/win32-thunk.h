#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX 1
#include <windows.h>

#include <direct.h>
#include <dirent.h>
#include <io.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/utime.h>

#include <string>
#include <string_view>
#include <type_traits>

namespace Win32Thunk
{
  using std::string;
  using std::wstring;
#ifdef __cpp_lib_string_view
  using std::string_view;
  using std::wstring_view;
#else
  using string_view = std::string;
  using wstring_view = std::wstring;
#endif

  namespace Detail {
    inline bool IsNt() {
      static bool result = ({
        OSVERSIONINFOA osvi = {sizeof (OSVERSIONINFOA)};
        GetVersionExA(&osvi);
        osvi.dwPlatformId >= VER_PLATFORM_WIN32_NT;
      });
      return result;
    }

    inline string Narrow(wstring_view wpath) {
      int alen = WideCharToMultiByte(CP_ACP, 0, wpath.data(), wpath.length(), nullptr, 0, nullptr, nullptr);
      if (alen == 0)
        return {};
      string apath(alen, 0);
      WideCharToMultiByte(CP_ACP, 0, wpath.data(), wpath.length(), &apath[0], alen, nullptr, nullptr);
      return apath;
    }

    inline string Narrow(const wchar_t* wpath) {
      size_t wlen = wcslen(wpath);
      return Narrow(wstring_view{wpath, wlen});
    }

    inline string NarrowTrunc(const wchar_t* wpath) {
      string apath = Narrow(wpath);
      if (apath.length() > MAX_PATH - 1)
        apath.resize(MAX_PATH - 1);
      return apath;
    }

    inline wstring Widen(string_view apath) {
      int wlen = MultiByteToWideChar(CP_ACP, 0, apath.data(), apath.length(), nullptr, 0);
      if (wlen == 0)
        return {};
      wstring wpath(wlen, 0);
      MultiByteToWideChar(CP_ACP, 0, apath.data(), apath.length(), &wpath[0], wlen);
      return wpath;
    }

    inline wstring Widen(const char *apath) {
      size_t alen = strlen(apath);
      return Widen(string_view{apath, alen});
    }

    inline wstring WidenTrunc(const char *apath) {
      wstring wpath = Widen(apath);
      if (wpath.length() > MAX_PATH - 1)
        wpath.resize(MAX_PATH - 1);
      return wpath;
    }
  }

  namespace Detail {
    inline _finddata_t Narrow(const _wfinddata_t &wdta) {
      _finddata_t adta;
      adta.attrib = wdta.attrib;
      adta.time_create = wdta.time_create;
      adta.time_access = wdta.time_access;
      adta.time_write = wdta.time_write;
      adta.size = wdta.size;
      string aname = NarrowTrunc(wdta.name);
      memcpy(adta.name, aname.c_str(), aname.length() + 1);
      return adta;
    }

    inline dirent Narrow(const _wdirent &wdir) {
      dirent adir;
      adir.d_ino = 0;  // always zero
      adir.d_reclen = 0;  // always zero
      string aname = NarrowTrunc(wdir.d_name);
      adir.d_namlen = aname.length();
      memcpy(adir.d_name, aname.c_str(), aname.length() + 1);
      return adir;
    }

    inline DIR *Narrow(const _WDIR *wdirp) {
      string aname = Narrow(wdirp->dd_name);
      DIR *adirp = (DIR *) malloc(sizeof (DIR) + aname.length());  // undocumented
      adirp->dd_dta = Narrow(wdirp->dd_dta);
      adirp->dd_dir = Narrow(wdirp->dd_dir);
      adirp->dd_handle = wdirp->dd_handle;
      adirp->dd_stat = wdirp->dd_stat;
      memcpy(adirp->dd_name, aname.c_str(), aname.length() + 1);
      return adirp;
    }

    inline _wfinddata_t Widen(const _finddata_t &adta) {
      _wfinddata_t wdta;
      wdta.attrib = adta.attrib;
      wdta.time_create = adta.time_create;
      wdta.time_access = adta.time_access;
      wdta.time_write = adta.time_write;
      wdta.size = adta.size;
      wstring wname = WidenTrunc(adta.name);
      memcpy(wdta.name, wname.c_str(), (wname.length() + 1) * sizeof (wchar_t));
      return wdta;
    }

    inline _wdirent Widen(const dirent &adir) {
      _wdirent wdir;
      wdir.d_ino = 0;  // always zero
      wdir.d_reclen = 0;  // always zero
      wstring wname = WidenTrunc(adir.d_name);
      wdir.d_namlen = wname.length();
      memcpy(wdir.d_name, wname.c_str(), (wname.length() + 1) * sizeof (wchar_t));
      return wdir;
    }

    inline _WDIR *Widen(const DIR *adirp) {
      wstring wname = Widen(adirp->dd_name);
      _WDIR *wdirp = (_WDIR *) malloc(sizeof (_WDIR) + wname.length() * sizeof (wchar_t));
      wdirp->dd_dta = Widen(adirp->dd_dta);
      wdirp->dd_dir = Widen(adirp->dd_dir);
      wdirp->dd_handle = adirp->dd_handle;
      wdirp->dd_stat = adirp->dd_stat;
      memcpy(wdirp->dd_name, wname.c_str(), (wname.length() + 1) * sizeof (wchar_t));
      return wdirp;
    }
  }

  inline HANDLE WINAPI CreateFileW(
    _In_ LPCWSTR lpFileName,
    _In_ DWORD dwDesiredAccess,
    _In_ DWORD dwShareMode,
    _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    _In_ DWORD dwCreationDisposition,
    _In_ DWORD dwFlagsAndAttributes,
    _In_opt_ HANDLE hTemplateFile
  ) {
    if (Detail::IsNt())
      return ::CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);

    // Windows 9x: `FILE_SHARE_DELETE` not supported
    DWORD mode = dwShareMode & ~FILE_SHARE_DELETE;
    return ::CreateFileA(Detail::Narrow(lpFileName).c_str(), dwDesiredAccess, mode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
  }

  // Windows 2000
  inline BOOL WINAPI CreateHardLinkW(
    _In_ LPCWSTR lpFileName,
    _In_ LPCWSTR lpExistingFileName,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes
  ) {
    using type = decltype (&CreateHardLinkW);
    static_assert(std::is_same<type, decltype (&::CreateHardLinkW)>::value, "CreateHardLinkW signature mismatch");
    static type real = (type) GetProcAddress(GetModuleHandleA("kernel32.dll"), "CreateHardLinkW");
    if (real)
      return real(lpFileName, lpExistingFileName, lpSecurityAttributes);

    SetLastError(ERROR_NOT_SUPPORTED);
    return FALSE;
  }

  inline BOOL WINAPI DeleteFileW(
    _In_ LPCWSTR lpFileName
  ) {
    if (Detail::IsNt())
      return ::DeleteFileW(lpFileName);
    return ::DeleteFileA(Detail::Narrow(lpFileName).c_str());
  }

  inline DWORD WINAPI GetFullPathNameW(
    _In_ LPCWSTR lpFileName,
    _In_ DWORD nBufferLength,
    _Out_ LPWSTR lpBuffer,
    _Out_opt_ LPWSTR *lpFilePart
  ) {
    if (Detail::IsNt())
      return ::GetFullPathNameW(lpFileName, nBufferLength, lpBuffer, lpFilePart);

    string apath = Detail::Narrow(lpFileName);
    size_t abuflen = nBufferLength > MAX_PATH ? nBufferLength : MAX_PATH;
    string abuf(abuflen, 0);
    char *afilepart = nullptr;
    DWORD alen = ::GetFullPathNameA(apath.c_str(), abuflen, &abuf[0], &afilepart);
    if (alen == 0)
      return 0;
    if (alen > abuflen) {
      abuf.resize(alen);
      alen = ::GetFullPathNameA(apath.c_str(), abuflen, &abuf[0], &afilepart);
    }
    abuf.resize(alen);
    wstring wres = Detail::Widen(abuf);
    size_t len = wres.length() + 1;
    if (len > nBufferLength)
      return len;
    memcpy(lpBuffer, wres.data(), len * sizeof (wchar_t));
    if (lpFilePart) {
      size_t aprefixlen = afilepart - abuf.data();
      wstring wprefix = Detail::Widen(string_view{abuf.data(), aprefixlen});
      *lpFilePart = lpBuffer + wprefix.length();
    }
    return wres.length();
  }

  inline BOOL WINAPI MoveFileExW(
    _In_ LPCWSTR lpExistingFileName,
    _In_opt_ LPCWSTR lpNewFileName,
    _In_ DWORD dwFlags
  ) {
    if (Detail::IsNt())
      return ::MoveFileExW(lpExistingFileName, lpNewFileName, dwFlags);

    // Windows 9x: `MoveFileExA` is a stub
    string aold = Detail::Narrow(lpExistingFileName);
    string anew = Detail::Narrow(lpNewFileName);
    int ok = ::MoveFileA(aold.c_str(), anew.c_str());
    if (!ok && (dwFlags & (MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING))) {
      bool replace = dwFlags & MOVEFILE_REPLACE_EXISTING;
      ok = ::CopyFileA(aold.c_str(), anew.c_str(), !replace);
      if (ok)
        ::DeleteFileA(aold.c_str());
    }
    return ok;
  }

  inline BOOL WINAPI RemoveDirectoryW(
    _In_ LPCWSTR lpPathName
  ) {
    if (Detail::IsNt())
      return ::RemoveDirectoryW(lpPathName);
    return ::RemoveDirectoryA(Detail::Narrow(lpPathName).c_str());
  }

  inline BOOL WINAPI WriteConsoleW(
    _In_ HANDLE hConsoleOutput,
    _In_ const VOID *lpBuffer,
    _In_ DWORD nNumberOfCharsToWrite,
    _Out_opt_ LPDWORD lpNumberOfCharsWritten,
    _Reserved_ LPVOID lpReserved
  ) {
    if (Detail::IsNt())
      return ::WriteConsoleW(hConsoleOutput, lpBuffer, nNumberOfCharsToWrite, lpNumberOfCharsWritten, lpReserved);

    string abuf = Detail::Narrow(wstring_view{(const wchar_t *)lpBuffer, nNumberOfCharsToWrite});
    DWORD awritten;
    int ok = ::WriteConsoleA(hConsoleOutput, abuf.c_str(), abuf.length(), &awritten, lpReserved);
    if (lpNumberOfCharsWritten) {
      if (awritten == abuf.length())
        // fast path
        *lpNumberOfCharsWritten = nNumberOfCharsToWrite;
      else {
        wstring wwritten = Detail::Widen(string_view{abuf.data(), awritten});
        *lpNumberOfCharsWritten = wwritten.length();
      }
    }
    return ok;
  }

  inline int _wchdir(
    const wchar_t *dirname
  ) {
    if (Detail::IsNt())
      return ::_wchdir(dirname);
    return ::_chdir(Detail::Narrow(dirname).c_str());
  }

  inline int _wchmod(
    const wchar_t *filename,
    int pmode
  ) {
    if (Detail::IsNt())
      return ::_wchmod(filename, pmode);
    return ::_chmod(Detail::Narrow(filename).c_str(), pmode);
  }

  inline int _wclosedir(
    _WDIR *wdirp
  ) {
    if (Detail::IsNt())
      return ::_wclosedir(wdirp);

    DIR *adirp = Detail::Narrow(wdirp);
    int ret = ::closedir(adirp);
    free(wdirp);  // undocumented
    return ret;
  }

  inline FILE *_wfopen(
    const wchar_t *filename,
    const wchar_t *mode
  ) {
    if (Detail::IsNt())
      return ::_wfopen(filename, mode);
    return ::fopen(Detail::Narrow(filename).c_str(), Detail::Narrow(mode).c_str());
  }

  inline wchar_t *_wgetcwd(
    wchar_t *buffer,
    int maxlen
  ) {
    if (Detail::IsNt())
      return ::_wgetcwd(buffer, maxlen);

    char *abuf = _getcwd(nullptr, 0);
    if (!abuf)
      return nullptr;
    wstring wres = Detail::Widen(abuf);
    ssize_t len = wres.length() + 1;
    free(abuf);
    if (buffer) {
      if (len > maxlen) {
        _set_errno(ERANGE);
        return nullptr;
      }
    } else {
      // The `buffer` argument can be `NULL`; a buffer of at least size `maxlen` (more only if necessary) is automatically allocated, using `malloc`, to store the path.
      if (len < maxlen)
        len = maxlen;
      buffer = (wchar_t *) malloc(len * sizeof (wchar_t));
      if (!buffer) {
        _set_errno(ENOMEM);
        return nullptr;
      }
    }
    memcpy(buffer, wres.data(), len * sizeof (wchar_t));
    return buffer;
  }

  inline int _wmkdir(
    const wchar_t *dirname
  ) {
    if (Detail::IsNt())
      return ::_wmkdir(dirname);
    return ::_mkdir(Detail::Narrow(dirname).c_str());
  }

  inline int _wopen(
    const wchar_t *filename,
    int oflag
  ) {
    if (Detail::IsNt())
      return ::_wopen(filename, oflag);
    return ::_open(Detail::Narrow(filename).c_str(), oflag);
  }

  inline int _wopen(
    const wchar_t *filename,
    int oflag,
    int pmode
  ) {
    if (Detail::IsNt())
      return ::_wopen(filename, oflag, pmode);
    return ::_open(Detail::Narrow(filename).c_str(), oflag, pmode);
  }

  inline _WDIR *_wopendir(
    const wchar_t *name
  ) {
    if (Detail::IsNt())
      return ::_wopendir(name);

    DIR *adirp = ::opendir(Detail::Narrow(name).c_str());
    if (!adirp)
      return nullptr;
    _WDIR *wdirp = Detail::Widen(adirp);
    free(adirp);  // undocumented
    return wdirp;
  }

  inline _wdirent *_wreaddir(_WDIR *wdirp) {
    if (Detail::IsNt())
      return ::_wreaddir(wdirp);

    DIR *adirp = Detail::Narrow(wdirp);
    dirent *adir = ::readdir(adirp);
    if (adir)
      wdirp->dd_dir = Detail::Widen(adirp->dd_dir);
    wdirp->dd_handle = adirp->dd_handle;
    wdirp->dd_stat = adirp->dd_stat;
    free(adirp);
    return adir ? &wdirp->dd_dir : nullptr;
  }

#ifdef _UCRT
  using ::_wstat64;
#else
  // Windows 2000
  inline int _wstat64(
    const wchar_t *path,
    struct __stat64 *buffer
  ) {
    using type = decltype (&_wstat64);
    static_assert(std::is_same<type, decltype (&::_wstat64)>::value, "_wstat64 signature mismatch");
    static type real = (type) GetProcAddress(GetModuleHandleA("msvcrt.dll"), "_wstat64");
    if (real)
      return real(path, buffer);

    struct _stat32i64 buffer32;
    int ret;
    if (Detail::IsNt())
      ret = ::_wstat32i64(path, &buffer32);
    else
      ret = ::_stat32i64(Detail::Narrow(path).c_str(), &buffer32);
    if (ret == -1) {
      *buffer = {};
      return -1;
    }
    buffer->st_dev = buffer32.st_dev;
    buffer->st_ino = buffer32.st_ino;
    buffer->st_mode = buffer32.st_mode;
    buffer->st_nlink = buffer32.st_nlink;
    buffer->st_uid = buffer32.st_uid;
    buffer->st_gid = buffer32.st_gid;
    buffer->st_rdev = buffer32.st_rdev;
    buffer->st_size = buffer32.st_size;
    buffer->st_atime = buffer32.st_atime;
    buffer->st_mtime = buffer32.st_mtime;
    buffer->st_ctime = buffer32.st_ctime;
    return ret;
  }
#endif

  inline int _wutime(
    const wchar_t *filename,
    struct _utimbuf *times
  ) {
    if (Detail::IsNt())
      return ::_wutime(filename, times);
    return ::_utime(Detail::Narrow(filename).c_str(), times);
  }
}
