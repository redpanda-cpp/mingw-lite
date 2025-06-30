#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef NOSTL_NOCRT
  #include <nocrt/stdlib.h>
#endif

#include "../__config.h"

#include "../__functional/less.h"
#include "../__iterator/iterator_traits.h"
#include "../__iterator/reverse_iterator.h"
#include "../__new/placement_new.h"
#include "../__type_traits/enable_if.h"
#include "../__type_traits/is_const.h"
#include "../__utility/declval.h"
#include "../__utility/forward.h"
#include "../__utility/move.h"

namespace NS_NOSTL
{
  namespace detail
  {
    template <typename T, typename Compare = less<T>>
    class avl_tree
    {
    public:
      using value_type = T;
      using pointer = T *;
      using const_pointer = const T *;
      using reference = T &;
      using const_reference = const T &;
      using size_type = size_t;
      using difference_type = ptrdiff_t;

    private:
      struct node_t
      {
        node_t *left;
        node_t *right;
        node_t *parent;
        int8_t left_height;
        int8_t right_height;
        value_type value;

        node_t() : node_t(value_type())
        {
        }

        node_t(const value_type &v,
               node_t *parent = nullptr,
               node_t *left = nullptr,
               node_t *right = nullptr)
            : left(left)
            , right(right)
            , parent(parent)
            , left_height(0)
            , right_height(0)
            , value(v)
        {
          update_height();
        }

        node_t(value_type &&v,
               node_t *parent = nullptr,
               node_t *left = nullptr,
               node_t *right = nullptr)
            : left(left)
            , right(right)
            , parent(parent)
            , left_height(0)
            , right_height(0)
            , value(NS_NOSTL::move(v))
        {
        }

        node_t(const node_t &other) = default;

        node_t(node_t &&other) = default;

        node_t &operator=(const node_t &other) = default;

        node_t &operator=(node_t &&other) = default;

        int8_t height() const
        {
          return (left_height > right_height ? left_height : right_height) + 1;
        }

        int8_t balance() const
        {
          return left_height - right_height;
        }

        void update_height()
        {
          left_height = left ? left->height() : 0;
          right_height = right ? right->height() : 0;
        }

        static node_t *alloc()
        {
          return alloc(value_type());
        }

        static node_t *alloc(const value_type &v,
                             node_t *parent = nullptr,
                             node_t *left = nullptr,
                             node_t *right = nullptr)
        {
          node_t *node =
              static_cast<node_t *>(NS_NOSTL_CRT::malloc(sizeof(node_t)));
          if (!node)
            NOSTL_RAISE_BAD_ALLOC();
          new (node) node_t(v, parent, left, right);
          return node;
        }

        static node_t *alloc(value_type &&v,
                             node_t *parent = nullptr,
                             node_t *left = nullptr,
                             node_t *right = nullptr)
        {
          node_t *node =
              static_cast<node_t *>(NS_NOSTL_CRT::malloc(sizeof(node_t)));
          if (!node)
            NOSTL_RAISE_BAD_ALLOC();
          new (node) node_t(NS_NOSTL::move(v), parent, left, right);
          return node;
        }

        static void dealloc(node_t *node)
        {
          node->~node_t();
          NS_NOSTL_CRT::free(node);
        }

        static void dealloc_all(node_t *node)
        {
          if (node->left)
            dealloc_all(node->left);
          if (node->right)
            dealloc_all(node->right);
          dealloc(node);
        }
      };

    public:
      template <typename U>
      class basic_iterator
      {
        friend class avl_tree;

      public:
        using difference_type = ptrdiff_t;
        using value_type = U;
        using pointer = U *;
        using reference = U &;
        using iterator_category = bidirectional_iterator_tag;

      private:
        node_t *node;

      private:
        basic_iterator() : node(nullptr)
        {
        }

        basic_iterator(node_t *n) : node(n)
        {
        }

      public:
        template <typename = type_identity_t<void>>
        basic_iterator(const basic_iterator<remove_const_t<U>> &mutable_other)
            : node(mutable_other.node)
        {
        }

        basic_iterator(const basic_iterator &other) = default;

        basic_iterator &operator=(const basic_iterator &other) = default;

        basic_iterator &operator++()
        {
          if (node->right) {
            //  current
            //     \.
            //      \.
            //     right
            //       /
            //     \/
            //    next
            node = node->right;
            while (node->left)
              node = node->left;
          } else {
            while (true) {
              node_t *current = node;
              node = node->parent;
              if (node->left == current) {
                //     next
                //      /\.
                //     /
                //  current
                break;
              } else {
                //     next
                //      /\.
                //     /
                //  parent
                //    /\.
                //      \.
                //    current
                continue;
              }
            }
          }
          return *this;
        }

        basic_iterator operator++(int)
        {
          basic_iterator temp = *this;
          ++(*this);
          return temp;
        }

        basic_iterator &operator--()
        {
          if (node->left) {
            node = node->left;
            while (node->right)
              node = node->right;
          } else {
            while (true) {
              node_t *current = node;
              node = node->parent;
              if (node->right == current)
                break;
              else
                continue;
            }
          }
          return *this;
        }

        basic_iterator operator--(int)
        {
          basic_iterator temp = *this;
          --(*this);
          return temp;
        }

        reference operator*() const
        {
          return node->value;
        }

        pointer operator->() const
        {
          return &node->value;
        }

        bool operator==(const basic_iterator &other) const
        {
          return node == other.node;
        }

        bool operator!=(const basic_iterator &other) const
        {
          return node != other.node;
        }
      };

    public:
      using iterator = basic_iterator<T>;
      using const_iterator = basic_iterator<const T>;

    private:
      //          end
      //        /     \.
      //       /       \.
      //     root     (null)
      //    /    \.
      //  left  right

      node_t *end_;
      size_type node_count;
      Compare comp;

    public:
      explicit avl_tree(const Compare &compare = Compare())
          : end_(node_t::alloc()), node_count(0), comp(compare)
      {
      }

      avl_tree(const avl_tree &other) = delete;

      avl_tree(avl_tree &&other)
          : end_(node_t::alloc())
          , node_count(other.node_count)
          , comp(NS_NOSTL::move(other.comp))
      {
        node_t *temp = end_;
        end_ = other.end_;
        other.end_ = temp;

        other.node_count = 0;
      }

      ~avl_tree()
      {
        clear();
        node_t::dealloc(end_);
      }

      avl_tree &operator=(const avl_tree &other) = delete;

      avl_tree &operator=(avl_tree &&other)
      {
        // intentionally not check self move assignment
        clear();

        node_t *temp = end_;
        end_ = other.end_;
        other.end_ = temp;

        node_count = other.node_count;
        other.node_count = 0;

        comp = NS_NOSTL::move(other.comp);

        return *this;
      }

      iterator begin()
      {
        node_t *current = end_;
        while (current->left)
          current = current->left;
        return iterator(current);
      }

      const_iterator begin() const
      {
        node_t *current = end_;
        while (current->left)
          current = current->left;
        return const_iterator(current);
      }

      iterator end()
      {
        return iterator(end_);
      }

      const_iterator end() const
      {
        return const_iterator(end_);
      }

      size_type empty() const
      {
        return size() == 0;
      }

      size_type size() const
      {
        return node_count;
      }

      void clear()
      {
        if (end_->left)
          node_t::dealloc_all(end_->left);

        end_->left = nullptr;
        end_->update_height();
        node_count = 0;
      }

      iterator insert(const_iterator pos, value_type &&value)
      {
        if (pos != end() && !comp(value, pos.node->value))
          return iterator(pos.node);

        node_t *new_node;

        if (pos.node->left) {
          //       pos
          //       /
          //      /
          //  existing
          //      \.
          //       \.
          //    new_node
          node_t *existing = pos.node->left;
          while (existing->right)
            existing = existing->right;

          new_node = node_t::alloc(NS_NOSTL::move(value), existing);
          existing->right = new_node;
          node_count++;
        } else {
          //       pos
          //       /
          //      /
          //  new_node
          new_node = node_t::alloc(NS_NOSTL::move(value), pos.node);
          pos.node->left = new_node;
          node_count++;
        }

        node_t *parent = new_node->parent;
        while (parent) {
          int old_height = parent->height();
          parent->update_height();
          if (parent->height() == old_height)
            break;
          parent = parent->parent;
        }

        return iterator(new_node);
      }

      template <typename U>
      auto find(const U &value)
          -> decltype(comp(value, declval<const T &>()), iterator())
      {
        iterator pos = lower_bound(value);
        if (pos == end() || comp(value, *pos))
          return end();
        return pos;
      }

      template <typename U>
      auto find(const U &value) const
          -> decltype(comp(value, declval<const T &>()), const_iterator())
      {
        const_iterator pos = lower_bound(value);
        if (pos == end() || comp(value, *pos))
          return end();
        return pos;
      }

      template <typename U>
      auto lower_bound(const U &value)
          -> decltype(comp(declval<const T &>(), value), iterator())
      {
        if (!end_->left)
          return end();

        node_t *root = end_->left;
        node_t *result = end_;
        while (root) {
          if (!comp(root->value, value)) {
            result = root;
            root = root->left;
          } else {
            root = root->right;
          }
        }
        return iterator(result);
      }

      template <typename U>
      auto lower_bound(const U &value) const
          -> decltype(comp(declval<const T &>(), value), const_iterator())
      {
        if (!end_->left)
          return end();

        node_t *root = end_->left;
        node_t *result = end_;
        while (root) {
          if (!comp(root->value, value)) {
            result = root;
            root = root->left;
          } else {
            root = root->right;
          }
        }
        return const_iterator(result);
      }

      Compare value_comp() const
      {
        return comp;
      }
    };
  } // namespace detail
} // namespace NS_NOSTL
