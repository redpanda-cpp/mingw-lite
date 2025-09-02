#pragma once

#include <stddef.h>

#include "__config.h"

#include "__iterator/reverse_iterator.h"
#include "__new/placement_new.h"
#include "__utility/forward.h"
#include "__utility/move.h"

#ifdef NOSTL_NOCRT
  #include <nocrt/stdlib.h>
#endif

namespace NS_NOSTL
{
  template <class T>
  class vector
  {
  public:
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using iterator = value_type *;
    using const_iterator = const value_type *;
    using reverse_iterator = NS_NOSTL::reverse_iterator<iterator>;
    using const_reverse_iterator = NS_NOSTL::reverse_iterator<const_iterator>;

  private:
    T *block;
    size_t block_size;
    size_t block_cap;

  public:
    vector() : block(nullptr), block_size(0), block_cap(0)
    {
    }

    vector(const vector &other) = delete;

    vector(vector &&other)
        : block(other.block)
        , block_size(other.block_size)
        , block_cap(other.block_cap)
    {
      other.block = nullptr;
      other.block_size = 0;
      other.block_cap = 0;
    }

    ~vector()
    {
      free_block();
    }

    vector &operator=(const vector &other) = delete;

    vector &operator=(vector &&other)
    {
      // intentionally not check self move assignment
      free_block();
      block = other.block;
      block_size = other.block_size;
      block_cap = other.block_cap;
      other.block = nullptr;
      other.block_size = 0;
      other.block_cap = 0;
      return *this;
    }

    // element access
  public:
    reference operator[](size_type pos)
    {
      return block[pos];
    }

    const_reference operator[](size_type pos) const
    {
      return block[pos];
    }

    T *data()
    {
      return block;
    }

    const T *data() const
    {
      return block;
    }

    // iterators
  public:
    iterator begin()
    {
      return block;
    }

    const_iterator begin() const
    {
      return block;
    }

    const_iterator cbegin() const
    {
      return begin();
    }

    iterator end()
    {
      return block + block_size;
    }

    const_iterator end() const
    {
      return block + block_size;
    }

    const_iterator cend() const
    {
      return end();
    }

    // capacity
  public:
    bool empty() const
    {
      return block_size == 0;
    }

    size_type size() const
    {
      return block_size;
    }

    void reserve(size_type new_cap)
    {
      if (new_cap > block_cap) {
        T *new_block = (T *)NS_NOSTL_CRT::malloc(new_cap * sizeof(T));
        if (!new_block)
          NOSTL_RAISE_BAD_ALLOC();

        for (size_t i = 0; i < block_size; ++i)
          new (new_block + i) T(NS_NOSTL::move(block[i]));

        free_block();
        block = new_block;
        block_cap = new_cap;
      }
    }

    size_type capacity() const
    {
      return block_cap;
    }

    // modifiers
  public:
    void push_back(const T &value)
    {
      emplace_back(value);
    }

    void push_back(T &&value)
    {
      emplace_back(NS_NOSTL::move(value));
    }

    template <class... Args>
    reference emplace_back(Args &&...args)
    {
      if (block_size == block_cap) {
        size_t new_cap = (block_cap == 0) ? 4 : block_cap * 2;
        reserve(new_cap);
      }

      new (block + block_size) T(NS_NOSTL::forward<Args>(args)...);
      return block[block_size++];
    }

  private:
    void free_block()
    {
      for (size_t i = 0; i < block_size; ++i)
        block[i].~T();
      NS_NOSTL_CRT::free(block);
    }
  };
} // namespace NS_NOSTL
