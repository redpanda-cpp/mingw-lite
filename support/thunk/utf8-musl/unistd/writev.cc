#include "../include/sys/uio.h"

#include <thunk/utf8-musl.h>

namespace mingw_thunk
{
  namespace musl
  {
    ssize_t writev(int fd, const struct iovec *iov, int iovcnt)
    {
      ssize_t total = 0;
      for (int i = 0; i < iovcnt; i++) {
        ssize_t ret = write(fd, iov[i].iov_base, iov[i].iov_len);
        if (ret < 0) {
          return total > 0 ? total : -1;
        }
        total += ret;
        if (ret < iov[i].iov_len) {
          break;
        }
      }
      return total;
    }
  } // namespace musl
} // namespace mingw_thunk
