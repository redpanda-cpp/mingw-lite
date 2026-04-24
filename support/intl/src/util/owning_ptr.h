#pragma once

#include <stddef.h>
#include <stdlib.h>

namespace intl
{
  template <typename T>
  class owning_ptr_base
  {
  protected:
    T *ptr_;

  public:
    constexpr owning_ptr_base() noexcept : ptr_(nullptr)
    {
    }

    constexpr owning_ptr_base(decltype(nullptr)) noexcept : ptr_(nullptr)
    {
    }

    explicit constexpr owning_ptr_base(T *ptr) noexcept : ptr_(ptr)
    {
    }

    ~owning_ptr_base() noexcept
    {
      if (ptr_)
        free(ptr_);
    }

    owning_ptr_base(const owning_ptr_base &) = delete;
    owning_ptr_base &operator=(const owning_ptr_base &) = delete;

    owning_ptr_base(owning_ptr_base &&other) noexcept : ptr_(other.ptr_)
    {
      other.ptr_ = nullptr;
    }

    owning_ptr_base &operator=(owning_ptr_base &&other) noexcept
    {
      if (ptr_)
        free(ptr_);
      ptr_ = other.ptr_;
      other.ptr_ = nullptr;
      return *this;
    }

    owning_ptr_base &operator=(decltype(nullptr)) noexcept
    {
      if (ptr_)
        free(ptr_);
      ptr_ = nullptr;
      return *this;
    }

  public:
    T *observe() const noexcept
    {
      return ptr_;
    }

    T *release() noexcept
    {
      T *tmp = ptr_;
      ptr_ = nullptr;
      return tmp;
    }

    void reset(T *ptr = nullptr) noexcept
    {
      if (ptr_)
        free(ptr_);
      ptr_ = ptr;
    }

    void swap(owning_ptr_base &other) noexcept
    {
      T *tmp = ptr_;
      ptr_ = other.ptr_;
      other.ptr_ = tmp;
    }

    explicit operator bool() const noexcept
    {
      return ptr_ != nullptr;
    }

    bool operator==(decltype(nullptr)) const noexcept
    {
      return ptr_ == nullptr;
    }

    bool operator!=(decltype(nullptr)) const noexcept
    {
      return ptr_ != nullptr;
    }
  };

  template <typename T>
  class owning_ptr : public owning_ptr_base<T>
  {
    using base = owning_ptr_base<T>;

  public:
    using base::base;
    using base::observe;
    using base::release;
    using base::reset;
    using base::swap;
    using base::operator bool;
    using base::operator==;
    using base::operator!=;

    owning_ptr(owning_ptr &&other) noexcept : base(static_cast<base &&>(other))
    {
    }

    owning_ptr &operator=(owning_ptr &&other) noexcept
    {
      base::operator=(static_cast<base &&>(other));
      return *this;
    }

    owning_ptr &operator=(decltype(nullptr)) noexcept
    {
      base::operator=(nullptr);
      return *this;
    }

    T &operator*() const
    {
      return *base::observe();
    }

    T *operator->() const
    {
      return base::observe();
    }
  };

  template <typename T>
  class owning_ptr<T[]> : public owning_ptr_base<T>
  {
    using base = owning_ptr_base<T>;

  public:
    using base::base;
    using base::observe;
    using base::release;
    using base::reset;
    using base::swap;
    using base::operator bool;
    using base::operator==;
    using base::operator!=;

    owning_ptr(owning_ptr &&other) noexcept : base(static_cast<base &&>(other))
    {
    }

    owning_ptr &operator=(owning_ptr &&other) noexcept
    {
      base::operator=(static_cast<base &&>(other));
      return *this;
    }

    owning_ptr &operator=(decltype(nullptr)) noexcept
    {
      base::operator=(nullptr);
      return *this;
    }

    T &operator[](size_t index) const noexcept
    {
      return base::observe()[index];
    }
  };
} // namespace intl
