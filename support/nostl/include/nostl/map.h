#pragma once

#include <stddef.h>

#include "__config.h"

#include "__functional/less.h"
#include "__iterator/reverse_iterator.h"
#include "__map/avl_tree.h"
#include "__utility/pair.h"

namespace NS_NOSTL
{
  template <class Key, class T, class Compare = less<Key>>
  class map
  {
  public:
    class value_compare;

  private:
    using tree_type = detail::avl_tree<pair<const Key, T>, value_compare>;

  public:
    using key_type = Key;
    using mapped_type = T;
    using value_type = pair<const Key, T>;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using key_compare = Compare;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using iterator = typename tree_type::iterator;
    using const_iterator = typename tree_type::const_iterator;
    using reverse_iterator = NS_NOSTL::reverse_iterator<iterator>;
    using const_reverse_iterator = NS_NOSTL::reverse_iterator<const_iterator>;

  private:
    tree_type tree;

  public:
    class value_compare
    {
      friend class map;

    protected:
      Compare comp;

    protected:
      value_compare(Compare c) : comp(c)
      {
      }

    public:
      constexpr bool operator()(const value_type &lhs,
                                const value_type &rhs) const
      {
        return comp(lhs.first, rhs.first);
      }

      constexpr bool operator()(const key_type &lhs,
                                const value_type &rhs) const
      {
        return comp(lhs, rhs.first);
      }

      constexpr bool operator()(const value_type &lhs,
                                const key_type &rhs) const
      {
        return comp(lhs.first, rhs);
      }
    };

  public:
    map() : map(Compare())
    {
    }

    explicit map(const Compare &comp) : tree(value_compare(comp))
    {
    }

    map(const map &other) = default;

    map(map &&other) = default;

    ~map() = default;

    map &operator=(const map &other) = default;

    map &operator=(map &&other) = default;

    T &operator[](const Key &key)
    {
      iterator it = tree.lower_bound(key);
      if (it == tree.end() || key_comp()(key, it->first))
        it = tree.insert(it, value_type(key, T()));
      return it->second;
    }

    iterator begin()
    {
      return tree.begin();
    }

    const_iterator begin() const
    {
      return tree.begin();
    }

    const_iterator cbegin() const
    {
      return tree.cbegin();
    }

    iterator end()
    {
      return tree.end();
    }

    const_iterator end() const
    {
      return tree.end();
    }

    const_iterator cend() const
    {
      return tree.cend();
    }

    reverse_iterator rbegin()
    {
      return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const
    {
      return const_reverse_iterator(end());
    }

    const_reverse_iterator crbegin() const
    {
      return const_reverse_iterator(end());
    }

    reverse_iterator rend()
    {
      return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const
    {
      return const_reverse_iterator(begin());
    }

    const_reverse_iterator crend() const
    {
      return const_reverse_iterator(begin());
    }

    bool empty() const
    {
      return tree.empty();
    }

    size_type size() const
    {
      return tree.size();
    }

    void clear()
    {
      tree.clear();
    }

    iterator find(const Key &key)
    {
      return tree.find(key);
    }

    const_iterator find(const Key &key) const
    {
      return tree.find(key);
    }

    iterator lower_bound(const Key &key)
    {
      return tree.lower_bound(key);
    }

    const_iterator lower_bound(const Key &key) const
    {
      return tree.lower_bound(key);
    }

    key_compare key_comp() const
    {
      return tree.value_comp().comp;
    }

    value_compare value_comp() const
    {
      return tree.value_comp();
    }
  };
} // namespace NS_NOSTL
