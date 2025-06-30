#pragma once

#include <stdio.h>

namespace intl
{
  class file
  {
  private:
    FILE *fp;

  public:
    file() : fp(nullptr)
    {
    }

    file(const char *path, const char *mode)
    {
      fp = fopen(path, mode);
    }

    file(const wchar_t *path, const wchar_t *mode)
    {
      fp = _wfopen(path, mode);
    }

    file(const file &) = delete;

    file &operator=(const file &) = delete;

    file(file &&other) : fp(other.fp)
    {
      other.fp = nullptr;
    }

    file &operator=(file &&other)
    {
      // intentionally not check self move assignment
      if (fp)
        fclose(fp);
      fp = other.fp;
      other.fp = nullptr;
      return *this;
    }

    ~file()
    {
      if (fp)
        fclose(fp);
    }

  public:
    int seek(long offset, int whence)
    {
      return fseek(fp, offset, whence);
    }

    long tell()
    {
      return ftell(fp);
    }

    size_t read(void *ptr, size_t size, size_t count)
    {
      return fread(ptr, size, count, fp);
    }

  public:
    operator bool() const
    {
      return fp != nullptr;
    }
  };
} // namespace intl
