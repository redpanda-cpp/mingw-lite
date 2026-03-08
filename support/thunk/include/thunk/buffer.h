#pragma once

#include <stddef.h>
#include <windows.h>

namespace mingw_thunk
{
  namespace d
  {
    template <size_t static_size, typename T = char>
    class buffer
    {
    private:
      T *data_;
      size_t size_;
      size_t capacity_;
      T static_buffer_[static_size];

      bool is_heap() const noexcept
      {
        return data_ != nullptr;
      }

      static constexpr size_t aligned_alloc_size(size_t size) noexcept
      {
        size_t required = size + 1;
        size_t aligned = (required + 15) & ~15;
        return aligned;
      }

    public:
      buffer() noexcept : data_(nullptr), size_(0), capacity_(static_size)
      {
        static_buffer_[0] = T();
      }

      ~buffer() noexcept
      {
        if (is_heap()) {
          HeapFree(GetProcessHeap(), 0, data_);
        }
      }

      buffer(const buffer &) = delete;
      buffer &operator=(const buffer &) = delete;

      buffer(buffer &&other) noexcept
          : data_(other.data_), size_(other.size_), capacity_(other.capacity_)
      {
        if (other.is_heap()) {
          other.data_ = nullptr;
        } else {
          for (size_t i = 0; i <= size_; ++i) {
            static_buffer_[i] = other.static_buffer_[i];
          }
        }
        other.size_ = 0;
        other.capacity_ = static_size;
        other.static_buffer_[0] = T();
      }

      buffer &operator=(buffer &&other) noexcept
      {
        if (this != &other) {
          if (is_heap()) {
            HeapFree(GetProcessHeap(), 0, data_);
          }

          data_ = other.data_;
          size_ = other.size_;
          capacity_ = other.capacity_;

          if (!other.is_heap()) {
            for (size_t i = 0; i <= size_; ++i) {
              static_buffer_[i] = other.static_buffer_[i];
            }
          }

          other.data_ = nullptr;
          other.size_ = 0;
          other.capacity_ = static_size;
          other.static_buffer_[0] = T();
        }
        return *this;
      }

      T *data() noexcept
      {
        return is_heap() ? data_ : static_buffer_;
      }

      const T *data() const noexcept
      {
        return is_heap() ? data_ : static_buffer_;
      }

      T &operator[](size_t index) noexcept
      {
        return data()[index];
      }

      const T &operator[](size_t index) const noexcept
      {
        return data()[index];
      }

      size_t size() const noexcept
      {
        return size_;
      }

      size_t capacity() const noexcept
      {
        return capacity_;
      }

      bool empty() const noexcept
      {
        return size_ == 0;
      }

      void clear() noexcept
      {
        size_ = 0;
        data()[0] = T();
      }

      bool resize(size_t new_size) noexcept
      {
        if (new_size + 1 <= capacity_) {
          for (size_t i = size_; i < new_size; ++i) {
            data()[i] = T();
          }
          data()[new_size] = T();
          size_ = new_size;
          return true;
        }

        size_t new_capacity = aligned_alloc_size(new_size);
        T *new_data = static_cast<T *>(
            HeapAlloc(GetProcessHeap(), 0, new_capacity * sizeof(T)));

        if (!new_data) {
          return false;
        }

        for (size_t i = 0; i < size_; ++i) {
          new_data[i] = data()[i];
        }

        for (size_t i = size_; i < new_size; ++i) {
          new_data[i] = T();
        }
        new_data[new_size] = T();

        if (is_heap()) {
          HeapFree(GetProcessHeap(), 0, data_);
        }

        data_ = new_data;
        size_ = new_size;
        capacity_ = new_capacity;
        return true;
      }

      bool push_back(T ch) noexcept
      {
        if (!resize(size_ + 1)) {
          return false;
        }
        data()[size_ - 1] = ch;
        data()[size_] = T();
        return true;
      }

      bool append(const T *str, size_t count) noexcept
      {
        if (count == 0) {
          return true;
        }
        size_t old_size = size_;
        if (!resize(old_size + count)) {
          return false;
        }
        for (size_t i = 0; i < count; ++i) {
          data()[old_size + i] = str[i];
        }
        return true;
      }
    };
  } // namespace d
} // namespace mingw_thunk
