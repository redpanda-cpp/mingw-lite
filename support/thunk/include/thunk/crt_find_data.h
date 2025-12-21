#include "string.h"

#include <io.h>

namespace mingw_thunk
{
  namespace internal
  {
    template <typename W>
    struct crt_find_data_a
    {
    };

    template <typename W>
    using crt_find_data_a_t = typename crt_find_data_a<W>::type;

    template <>
    struct crt_find_data_a<_wfinddata32_t>
    {
      using type = _finddata32_t;
    };

    template <>
    struct crt_find_data_a<_wfinddata32i64_t>
    {
      using type = _finddata32i64_t;
    };

    template <>
    struct crt_find_data_a<_wfinddata64_t>
    {
      using type = __finddata64_t;
    };

    template <>
    struct crt_find_data_a<_wfinddata64i32_t>
    {
      using type = _finddata64i32_t;
    };

    template <typename W>
    crt_find_data_a_t<W> w2u(const W &w_file_info)
    {
      crt_find_data_a_t<W> u_file_info;

      u_file_info.attrib = w_file_info.attrib;
      u_file_info.time_create = w_file_info.time_create;
      u_file_info.time_access = w_file_info.time_access;
      u_file_info.time_write = w_file_info.time_write;
      u_file_info.size = w_file_info.size;

      stl::string u_name = w2u(w_file_info.name);
      if (u_name.size() >= MAX_PATH) {
        memcpy(u_file_info.name, u_name.c_str(), MAX_PATH - 1);
        u_file_info.name[MAX_PATH - 1] = 0;
      } else {
        memcpy(u_file_info.name, u_name.c_str(), u_name.size());
        u_file_info.name[u_name.size()] = 0;
      }

      return u_file_info;
    }
  } // namespace internal
} // namespace mingw_thunk
