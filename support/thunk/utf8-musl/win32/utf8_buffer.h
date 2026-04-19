#pragma once

namespace mingw_thunk
{
  namespace musl
  {
    struct utf8_buffer
    {
      unsigned char buf[4];
      int len;
    };

    extern struct utf8_buffer g_utf8_buffer[3];
  } // namespace musl
} // namespace mingw_thunk
