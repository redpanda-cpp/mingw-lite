#include "_aligned_malloc.h"
#include "_aligned_free.h"

#include <catch_amalgamated.hpp>

#include <malloc.h>
#include <stdint.h>

TEST_CASE("_aligned_malloc and _aligned_free")
{
  size_t alignment = 64;
  for (size_t size = 1; size < (1 << 20); size <<= 1) {
    void *ptr = mingw_thunk::impl::fallback__aligned_malloc(size, alignment);

    REQUIRE(ptr != nullptr);
    REQUIRE((reinterpret_cast<uintptr_t>(ptr) & (alignment - 1)) == 0);

    mingw_thunk::impl::fallback__aligned_free(ptr);
  }
}
