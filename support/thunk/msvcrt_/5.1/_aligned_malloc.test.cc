#include <thunk/test.h>

#include <malloc.h>

TEST_CASE("_aligned_malloc and _aligned_free")
{
  size_t alignment = 64;
  for (size_t size = 1; size < (1 << 20); size <<= 1) {
    CLEAR_TOUCH_FLAG();
    void *ptr = _aligned_malloc(size, alignment);
    REQUIRE_TOUCHED();

    REQUIRE(ptr != nullptr);
    REQUIRE((reinterpret_cast<uintptr_t>(ptr) & (alignment - 1)) == 0);

    CLEAR_TOUCH_FLAG();
    _aligned_free(ptr);
    REQUIRE_TOUCHED();
  }
}
