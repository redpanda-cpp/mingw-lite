#pragma once

#include <stddef.h>

#include "../__config.h"

#include "../__type_traits/remove_cv.h"

namespace NS_NOSTL
{
  struct input_iterator_tag
  {
  };

  struct output_iterator_tag
  {
  };

  struct forward_iterator_tag : public input_iterator_tag
  {
  };

  struct bidirectional_iterator_tag : public forward_iterator_tag
  {
  };

  struct random_access_iterator_tag : public bidirectional_iterator_tag
  {
  };

  struct contiguous_iterator_tag : public random_access_iterator_tag
  {
  };

  template <class Iter>
  struct iterator_traits
  {
    using difference_type = typename Iter::difference_type;
    using value_type = typename Iter::value_type;
    using pointer = typename Iter::pointer;
    using reference = typename Iter::reference;
    using iterator_category = typename Iter::iterator_category;
  };

  template <class T>
  struct iterator_traits<T *>
  {
    using difference_type = ptrdiff_t;
    using value_type = remove_cv_t<T>;
    using pointer = T *;
    using reference = T &;
    using iterator_category = random_access_iterator_tag;
  };
} // namespace NS_NOSTL
