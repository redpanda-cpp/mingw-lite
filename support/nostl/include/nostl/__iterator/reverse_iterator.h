#pragma once

#include "../__config.h"

#include "iterator_traits.h"

namespace NS_NOSTL
{
  template <class Iter>
  class reverse_iterator
  {
  public:
    using iterator_type = Iter;
    using iterator_category = typename iterator_traits<Iter>::iterator_category;
    using value_type = typename iterator_traits<Iter>::value_type;
    using difference_type = typename iterator_traits<Iter>::difference_type;
    using pointer = typename iterator_traits<Iter>::pointer;
    using reference = typename iterator_traits<Iter>::reference;

  protected:
    Iter current;
  };
} // namespace NS_NOSTL
