#pragma once

#include <stddef.h>
#include <stdlib.h>

namespace intl
{
  class raw_data_proxy
  {
  private:
    char *data;

  private:
    raw_data_proxy(char *data) : data(data)
    {
    }

  public:
    template <typename T>
    T *as()
    {
      return reinterpret_cast<T *>(data);
    }

  public:
    operator char *() const
    {
      return data;
    }

    friend class raw_data;
  };

  class raw_data
  {
  private:
    char *data;

  public:
    raw_data() : data(nullptr)
    {
    }

    raw_data(void *data) : data(reinterpret_cast<char *>(data))
    {
    }

    raw_data(const raw_data &) = delete;

    raw_data &operator=(const raw_data &) = delete;

    raw_data(raw_data &&other) : data(other.data)
    {
      other.data = nullptr;
    }

    raw_data &operator=(raw_data &&other)
    {
      // intentionally not check self move assignment
      if (data)
        free(data);
      data = other.data;
      other.data = nullptr;
      return *this;
    }

    ~raw_data()
    {
      if (data)
        free(data);
    }

  public:
    template <typename T>
    T *as()
    {
      return reinterpret_cast<T *>(data);
    }

  public:
    explicit operator bool() const
    {
      return data != nullptr;
    }

    operator char *() const
    {
      return data;
    }

    char &operator[](size_t index)
    {
      return data[index];
    }

    raw_data_proxy operator+(ptrdiff_t offset)
    {
      return data + offset;
    }
  };
} // namespace intl
