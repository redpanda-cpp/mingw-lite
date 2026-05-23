#include <ntstatus.h>
#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/buffer.h>
#include <thunk/os.h>
#include <thunk/stdlib.h>
#include <thunk/string.h>
#include <thunk/yy/yy.h>

#include <windows.h>
#include <winnt.h>
#include <winternl.h>

namespace mingw_thunk
{
  // based on YY-Thunks 1.2.1-Beta.2 `src/Thunks/DllMainCRTStartup.hpp`.
  // heavily modified for mingw-w64: DllMainCRTStartup call our thunk.
  extern "C" ULONG _tls_index;

  namespace g
  {
    static ULONG tls_index_old;
  }

  extern "C" const IMAGE_TLS_DIRECTORY _tls_used;

  namespace d
  {
    enum class tls_mode
    {
      none,
      by_first_callback,
      by_thunk,
    };
  } // namespace d

  namespace g
  {
    // 用来记录第一次Tls回调的时机
    // 如果来自ByFirstCallback，那么说明当前模块的TLS功能完全正常（模块是EXE或者来自于隐式依赖的DLL），YY-Thunks无需额外修复
    // 如果来自ByDllMainCRTStartupForYY_Thunks，那么说明它是动态加载的DLL，TLS没有初始化
    static d::tls_mode tls_mode = d::tls_mode::none;
  } // namespace g

  namespace i
  {
    static void NTAPI first_callback(PVOID, DWORD reason, PVOID)
    {
      if (reason == DLL_PROCESS_ATTACH && g::tls_mode == d::tls_mode::none)
        g::tls_mode = d::tls_mode::by_first_callback;
    }
  } // namespace i

  namespace g
  {
    static const PIMAGE_TLS_CALLBACK first_callback
        __attribute__((section(".CRT$XLB"), used)) = &i::first_callback;
  }

  namespace d
  {
    struct tls_raw_item
    {
      tls_raw_item *next;
      tls_raw_item *prev;
      uintptr_t base;
      uintptr_t *old_tls_index;

      void free() noexcept
      {
        HeapFree(GetProcessHeap(), 0, old_tls_index);
        old_tls_index = nullptr;
        auto _base = base;
        base = 0;
        c::free((void *)_base);
      }
    };

    struct tls_header
    {
      tls_raw_item *volatile root = nullptr;

      void remove_item(tls_raw_item *item) noexcept
      {
        if (root == item) {
          item->prev = nullptr;
          root = item->next;
        } else {
          auto prev = item->prev;
          auto next = item->next;

          prev->next = next;

          if (next) {
            next->prev = prev;
          }
        }
      }

      void add_item(tls_raw_item *first, tls_raw_item *last = nullptr) noexcept
      {
        if (!last)
          last = first;

        first->prev = nullptr;
        auto _root = root;
        last->next = _root;
        if (_root) {
          _root->prev = last;
        }
        root = first;
      }

      tls_raw_item *flush() noexcept
      {
        return (tls_raw_item *)InterlockedExchange((uintptr_t *)&root, 0);
      }
    };

    enum class tls_status
    {
      // 没有人尝试释放Tls
      none,
      // 线程Tls数据需要申请或者释放
      thread_lock,
      // DLL正在释放
      dll_unload,
    };
  } // namespace d

  namespace g
  {
    static volatile LONG tls_status = 0;
    static d::tls_header tls_header;
  } // namespace g

  namespace i
  {
    static uintptr_t &get_tls_indices_pointer(TEB *teb)
    {
      constexpr ptrdiff_t offset = 11 * sizeof(void *);
      static_assert(offsetof(TEB, ProcessEnvironmentBlock) ==
                    12 * sizeof(void *));
      return *(uintptr_t *)((uintptr_t)teb + offset);
    }

    static uintptr_t *get_tls_indices(TEB *teb)
    {
      return (uintptr_t *)get_tls_indices_pointer(teb);
    }

    static size_t get_tls_index_buffer_count(TEB *teb)
    {
      auto tls_pointer = get_tls_indices(teb);
      if (!tls_pointer)
        return 0;

      return HeapSize(GetProcessHeap(), 0, tls_pointer) / sizeof(void *);
    }

    static ULONG get_max_tls_index() noexcept
    {
      ULONG max_tls_index = 0;

      auto ldr = NtCurrentTeb()->ProcessEnvironmentBlock->Ldr;
      auto header = &ldr->InMemoryOrderModuleList;
      for (auto item = header->Flink; item != header;) {
        auto entry =
            CONTAINING_RECORD(item, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);
        item = item->Flink;

        // XP不会为动态加载的DLL设置这个字段（符合预期，毕竟没有真的初始化）
        /* if (entry->TlsIndex) continue; */
        ULONG tls_size;
        auto tls_image =
            (IMAGE_TLS_DIRECTORY *)internal::YY_ImageDirectoryEntryToData(
                entry->DllBase, IMAGE_DIRECTORY_ENTRY_TLS, &tls_size);

        if (tls_image == nullptr || tls_image->AddressOfIndex == 0) {
          continue;
        }

        const auto tls_index = *(ULONG *)tls_image->AddressOfIndex;
        if (max_tls_index < tls_index)
          max_tls_index = tls_index;
      }

      return max_tls_index;
    }

    static SYSTEM_PROCESS_INFORMATION *
    get_current_process_info(d::buffer<4096, char> &buffer)
    {
#if THUNK_LEVEL >= NTDDI_WIN4
      const auto NtQuerySystemInformation = &__ms_NtQuerySystemInformation;
#else
      const auto NtQuerySystemInformation = ntdll_NtQuerySystemInformation();
#endif

      auto buffer_size = buffer.capacity() - 1;
      ULONG ret = 0;
      for (;;) {
        if (!buffer.resize(buffer_size))
          return nullptr;
        auto data = buffer.data();

        LONG status = NtQuerySystemInformation(
            SystemProcessInformation, data, buffer_size, &ret);
        if (status >= 0)
          break;

        if (STATUS_INFO_LENGTH_MISMATCH != status)
          return nullptr;

        if (ret <= buffer_size) {
          // Windows 2000: ret is not set, doubling
          buffer_size = buffer_size * 2;
        } else {
          buffer_size = ret;
        }
      }

      const auto current_process_id = GetCurrentProcessId();
      auto info = (SYSTEM_PROCESS_INFORMATION *)buffer.data();
      for (;;) {
        if ((DWORD)(uintptr_t)info->UniqueProcessId == current_process_id)
          return info;

        if (info->NextEntryOffset == 0)
          break;

        info = (SYSTEM_PROCESS_INFORMATION *)((char *)info +
                                              info->NextEntryOffset);
      }

      return nullptr;
    }

    static_assert(sizeof(SYSTEM_PROCESS_INFORMATION) ==
                  offsetof(SYSTEM_PROCESS_INFORMATION, IoCounters) +
                      sizeof(IO_COUNTERS));

    static size_t get_process_info_header_size() noexcept
    {
      if (i::os_version() >= g::win32_win2k)
        return sizeof(SYSTEM_PROCESS_INFORMATION);
      else
        return sizeof(SYSTEM_PROCESS_INFORMATION) - sizeof(IO_COUNTERS);
    }

    static SYSTEM_THREAD_INFORMATION *
    get_thread_info(SYSTEM_PROCESS_INFORMATION *process_info,
                    ULONG thread_index)
    {
#if THUNK_LEVEL >= NTDDI_WIN2K
      constexpr size_t header_size = sizeof(SYSTEM_PROCESS_INFORMATION);
#else
      const static size_t header_size = get_process_info_header_size();
#endif

      uintptr_t base = (uintptr_t)process_info;
      uintptr_t thread0 = base + header_size;
      uintptr_t thread =
          thread0 + thread_index * sizeof(SYSTEM_THREAD_INFORMATION);
      return (SYSTEM_THREAD_INFORMATION *)thread;
    }

    static bool alloc_tls_data(TEB *teb = nullptr) noexcept
    {
      const size_t tls_raw_size =
          _tls_used.EndAddressOfRawData - _tls_used.StartAddressOfRawData;
      if (tls_raw_size == 0)
        return true;

      if (_tls_index == 0)
        return false;

      if (!teb)
        teb = NtCurrentTeb();

      auto tls_indices = get_tls_indices(teb);
      const auto tls_indices_length = get_tls_index_buffer_count(teb);
      if (tls_indices_length > _tls_index) {
        InterlockedExchange(&tls_indices[_tls_index], 0);
      }

      uintptr_t raw_tls_data =
          (uintptr_t)c::calloc(1, tls_raw_size + sizeof(d::tls_raw_item));
      if (!raw_tls_data) {
        return false;
      }

      auto current_node = (d::tls_raw_item *)(raw_tls_data + tls_raw_size);
      if (tls_indices_length <= _tls_index) {
        // Index不足，扩充……
        auto new_tls_indices_length = _tls_index + 128;
        auto new_tls_indices =
            (uintptr_t *)HeapAlloc(GetProcessHeap(),
                                   HEAP_ZERO_MEMORY,
                                   new_tls_indices_length * sizeof(void *));
        if (!new_tls_indices) {
          c::free((void *)raw_tls_data);
          return false;
        }

        c::memcpy(
            new_tls_indices, tls_indices, tls_indices_length * sizeof(void *));
        if ((void *)InterlockedCompareExchange(&get_tls_indices_pointer(teb),
                                               (uintptr_t)new_tls_indices,
                                               (uintptr_t)tls_indices) !=
            tls_indices) {
          // 这是什么情况，DllMain期间桌怎么会有其他线程操作Tls？
          HeapFree(GetProcessHeap(), 0, new_tls_indices);
          c::free((void *)raw_tls_data);
          return false;
        }

        if (teb == NtCurrentTeb()) {
          HeapFree(GetProcessHeap(), 0, tls_indices);
        } else {
          // 其他线程的无法直接释放，玩意缓存里恰好正在使用这块那么会崩溃的
          current_node->old_tls_index = tls_indices;
        }
        tls_indices = new_tls_indices;
      }

      c::memcpy((void *)raw_tls_data,
                (void *)_tls_used.StartAddressOfRawData,
                tls_raw_size);
      current_node->base = raw_tls_data;
      for (;;) {
        const auto status = (d::tls_status)InterlockedCompareExchange(
            &g::tls_status,
            LONG(d::tls_status::thread_lock),
            LONG(d::tls_status::none));
        // 锁定成功？
        if (status == d::tls_status::none) {
          g::tls_header.add_item(current_node);
          // 解除锁定
          InterlockedExchange(&g::tls_status, LONG(d::tls_status::none));
          InterlockedExchange(&tls_indices[_tls_index], raw_tls_data);
          break;
        }

        // 当前Dll正在卸载，不能再添加Tls
        if (status == d::tls_status::dll_unload) {
          current_node->free();
          return false;
        }
      }
      return true;
    }

    static void free_tls_data()
    {
      if (_tls_index == 0)
        return;

      const size_t tls_raw_size =
          _tls_used.EndAddressOfRawData - _tls_used.StartAddressOfRawData;
      if (tls_raw_size == 0)
        return;

      TEB *teb = NtCurrentTeb();
      if (_tls_index >= get_tls_index_buffer_count(teb))
        return;

      uintptr_t *tls_indices = get_tls_indices(teb);
      uintptr_t tls_raw_data = InterlockedExchange(&tls_indices[_tls_index], 0);
      if (tls_raw_data == 0)
        return;

      auto current_node = (d::tls_raw_item *)(tls_raw_data + tls_raw_size);

      for (;;) {
        const auto status = (d::tls_status)InterlockedCompareExchange(
            &g::tls_status,
            LONG(d::tls_status::thread_lock),
            LONG(d::tls_status::none));
        // 锁定成功？
        if (status == d::tls_status::none) {
          // 检查一下这块Tls数据是否是我们申请的
          if (current_node->base == tls_raw_data) {
            g::tls_header.remove_item(current_node);
          } else {
            current_node = nullptr;
          }

          // 解除锁定
          InterlockedExchange(&g::tls_status, LONG(d::tls_status::none));

          if (current_node)
            current_node->free();
          return;
        }

        // 当前Dll正在卸载，这些内存统一由DllMain接管，FreeTlsIndex会统一释放内存
        if (status == d::tls_status::dll_unload)
          return;
      }
    }
  } // namespace i

  namespace d
  {
    struct thread_basic_information
    {
      NTSTATUS ExitStatus;
      TEB *TebBaseAddress;
      CLIENT_ID ClientId;
      KAFFINITY AffinityMask;
      KPRIORITY Priority;
      KPRIORITY BasePriority;
    };
  } // namespace d

  namespace i
  {
    static bool create_tls_index() noexcept
    {
      if (_tls_index != 0)
        return false;

      _tls_index = get_max_tls_index() + 1;
      if (!alloc_tls_data(NtCurrentTeb()))
        return false;

      // 同时给所有历史的线程追加新DLL产生的Tls内存
      do {
#if THUNK_LEVEL >= NTDDI_WIN4
        const auto NtQueryInformationThread = &__ms_NtQueryInformationThread;
#else
        const auto NtQueryInformationThread = ntdll_NtQueryInformationThread();
#endif

        d::buffer<4096, char> buffer;
        SYSTEM_PROCESS_INFORMATION *process_info =
            get_current_process_info(buffer);
        if (!process_info) {
          break;
        }

        const auto current_thread_id = GetCurrentThreadId();

        for (ULONG i = 0; i != process_info->NumberOfThreads; ++i) {
          auto *thread = get_thread_info(process_info, i);

          if (current_thread_id ==
              (DWORD)(uintptr_t)thread->ClientId.UniqueThread)
            continue;

          auto h = OpenThread(THREAD_QUERY_INFORMATION,
                              FALSE,
                              (DWORD)(uintptr_t)thread->ClientId.UniqueThread);
          if (!h) {
            continue;
          }

          d::thread_basic_information thread_basic_info = {};
          LONG status = NtQueryInformationThread(h,
                                                 ThreadBasicInformation,
                                                 &thread_basic_info,
                                                 sizeof(thread_basic_info),
                                                 nullptr);
          if (status >= 0 && thread_basic_info.TebBaseAddress) {
            alloc_tls_data(thread_basic_info.TebBaseAddress);
          }

          CloseHandle(h);
        }
      } while (false);

      return true;
    }

    static void free_tls_index() noexcept
    {
      if (_tls_index == 0)
        return;

      for (;;) {
        const auto status = (d::tls_status)InterlockedCompareExchange(
            &g::tls_status,
            LONG(d::tls_status::dll_unload),
            LONG(d::tls_status::none));
        // 锁定成功？
        if (status == d::tls_status::none) {
          for (auto item = g::tls_header.flush(); item;) {
            auto next = item->next;
            item->free();
            item = next;
          }
          return;
        }

        // DllUnload 代表全局数据已经释放，只能进入一次
        if (status == d::tls_status::dll_unload)
          return;
      }
    }

    static void call_tls_callback(HINSTANCE instance, DWORD reason)
    {
      if (_tls_index == 0)
        return;

      TEB *teb = NtCurrentTeb();
      uintptr_t *tls_pointer = get_tls_indices(teb);
      if (!tls_pointer)
        return;

      if (i::is_nt() && _tls_index >= get_tls_index_buffer_count(teb))
        return;

      if (!tls_pointer[_tls_index])
        return;

      auto tls_callback = (PIMAGE_TLS_CALLBACK *)_tls_used.AddressOfCallBacks;
      if (tls_callback) {
        for (; *tls_callback; ++tls_callback)
          (*tls_callback)(instance, reason, nullptr);
      }
    }
  } // namespace i

  namespace d
  {
    struct win9x_node
    {
      win9x_node *next;
      win9x_node *prev;
      void *data;
    };

    struct win9x_list
    {
      win9x_node *first;
      win9x_node *last;
      win9x_node *current;
    };
  } // namespace d

  namespace g
  {
    constexpr uintptr_t win9x_pdb_offset_thread_list = 0x50;
    constexpr uintptr_t win9x_tdb_offset_tib = 0x08;
    constexpr uintptr_t win9x_tdb_offset_tls_slots = 0x90;
    constexpr size_t win9x_tls_slots_count = 80;
    constexpr uint8_t win9x_k32obj_thread = 0x07;
  } // namespace g

  namespace i
  {
    static void *win9x_get_pdb() noexcept
    {
      return *(void **)((char *)NtCurrentTeb() + 0x30);
    }

    static void *win9x_get_current_tdb() noexcept
    {
      return (char *)NtCurrentTeb() - g::win9x_tdb_offset_tib;
    }

    static uintptr_t *win9x_get_tls_slots(void *tdb) noexcept
    {
      return (uintptr_t *)((char *)tdb + g::win9x_tdb_offset_tls_slots);
    }

    static bool win9x_alloc_tls_data(void *tdb) noexcept
    {
      const size_t tls_raw_size =
          _tls_used.EndAddressOfRawData - _tls_used.StartAddressOfRawData;
      if (tls_raw_size == 0)
        return true;

      if (_tls_index == 0 || _tls_index >= g::win9x_tls_slots_count)
        return false;

      auto tls_slots = win9x_get_tls_slots(tdb);
      InterlockedExchange(&tls_slots[_tls_index], 0);

      uintptr_t raw_tls_data =
          (uintptr_t)c::calloc(1, tls_raw_size + sizeof(d::tls_raw_item));
      if (!raw_tls_data)
        return false;

      auto current_node = (d::tls_raw_item *)(raw_tls_data + tls_raw_size);

      c::memcpy((void *)raw_tls_data,
                (void *)_tls_used.StartAddressOfRawData,
                tls_raw_size);
      current_node->base = raw_tls_data;

      for (;;) {
        const auto status = (d::tls_status)InterlockedCompareExchange(
            &g::tls_status,
            LONG(d::tls_status::thread_lock),
            LONG(d::tls_status::none));
        if (status == d::tls_status::none) {
          g::tls_header.add_item(current_node);
          InterlockedExchange(&g::tls_status, LONG(d::tls_status::none));
          InterlockedExchange(&tls_slots[_tls_index], raw_tls_data);
          break;
        }

        if (status == d::tls_status::dll_unload) {
          current_node->free();
          return false;
        }
      }
      return true;
    }

    static bool win9x_create_tls_index() noexcept
    {
      if (_tls_index != 0)
        return false;

      _tls_index = TlsAlloc();
      if (_tls_index == TLS_OUT_OF_INDEXES)
        return false;

      if (!win9x_alloc_tls_data(win9x_get_current_tdb()))
        return false;

      auto pdb = (char *)win9x_get_pdb();
      auto thread_list =
          *(d::win9x_list **)(pdb + g::win9x_pdb_offset_thread_list);
      auto current_tdb = win9x_get_current_tdb();

      for (auto node = thread_list->first; node; node = node->next) {
        auto tdb = node->data;
        if (tdb == current_tdb)
          continue;
        if (*(uint8_t *)tdb != g::win9x_k32obj_thread)
          continue;
        win9x_alloc_tls_data(tdb);
      }
      return true;
    }

    static void win9x_free_tls_data() noexcept
    {
      if (_tls_index == 0 || _tls_index >= g::win9x_tls_slots_count)
        return;

      const size_t tls_raw_size =
          _tls_used.EndAddressOfRawData - _tls_used.StartAddressOfRawData;
      if (tls_raw_size == 0)
        return;

      auto tls_slots = win9x_get_tls_slots(win9x_get_current_tdb());
      uintptr_t tls_raw_data = InterlockedExchange(&tls_slots[_tls_index], 0);
      if (tls_raw_data == 0)
        return;

      auto current_node = (d::tls_raw_item *)(tls_raw_data + tls_raw_size);

      for (;;) {
        const auto status = (d::tls_status)InterlockedCompareExchange(
            &g::tls_status,
            LONG(d::tls_status::thread_lock),
            LONG(d::tls_status::none));
        if (status == d::tls_status::none) {
          if (current_node->base == tls_raw_data) {
            g::tls_header.remove_item(current_node);
          } else {
            current_node = nullptr;
          }

          InterlockedExchange(&g::tls_status, LONG(d::tls_status::none));

          if (current_node)
            current_node->free();
          return;
        }

        if (status == d::tls_status::dll_unload)
          return;
      }
    }

    static void win9x_free_tls_index() noexcept
    {
      if (_tls_index == 0)
        return;

      for (;;) {
        const auto status = (d::tls_status)InterlockedCompareExchange(
            &g::tls_status,
            LONG(d::tls_status::dll_unload),
            LONG(d::tls_status::none));
        if (status == d::tls_status::none) {
          auto pdb = (char *)win9x_get_pdb();
          auto thread_list =
              *(d::win9x_list **)(pdb + g::win9x_pdb_offset_thread_list);
          for (auto node = thread_list->first; node; node = node->next) {
            auto tdb = node->data;
            if (*(uint8_t *)tdb != g::win9x_k32obj_thread)
              continue;
            win9x_get_tls_slots(tdb)[_tls_index] = 0;
          }

          for (auto item = g::tls_header.flush(); item;) {
            auto next = item->next;
            item->free();
            item = next;
          }

          TlsFree(_tls_index);
          _tls_index = 0;
          return;
        }

        if (status == d::tls_status::dll_unload)
          return;
      }
    }
  } // namespace i

  extern "C" int __mingw_thunk_fix_loadlibrary_tls_data_pre(HINSTANCE instance,
                                                            DWORD reason,
                                                            LPVOID)
  {
    if (i::os_version() >= g::win32_vista)
      return true;

    const bool is_nt = i::is_nt();

    switch (reason) {
    case DLL_PROCESS_ATTACH:
      if (g::tls_mode == d::tls_mode::none) {
        g::tls_mode = d::tls_mode::by_thunk;
      }
      g::tls_index_old = _tls_index;

      if (_tls_index == 0 && g::tls_mode == d::tls_mode::by_thunk) {
        if (is_nt)
          return i::create_tls_index();
        else
          return i::win9x_create_tls_index();
      }
      return true;

    case DLL_THREAD_ATTACH:
      if (g::tls_index_old == 0 && g::tls_mode == d::tls_mode::by_thunk) {
        if (is_nt)
          i::alloc_tls_data();
        else
          i::win9x_alloc_tls_data(i::win9x_get_current_tdb());
        i::call_tls_callback(instance, reason);
      }
      return true;

    case DLL_THREAD_DETACH:
      if (g::tls_index_old == 0 && g::tls_mode == d::tls_mode::by_thunk) {
        i::call_tls_callback(instance, reason);
      }
      return true;

    case DLL_PROCESS_DETACH:
      if (g::tls_index_old == 0 && g::tls_mode == d::tls_mode::by_thunk) {
        i::call_tls_callback(instance, reason);
      }
      return true;

    default:
      return true;
    }
  }

  extern "C" void __mingw_thunk_fix_loadlibrary_tls_data_post(
      HINSTANCE instance, DWORD reason, LPVOID reserved)
  {
    if (i::os_version() >= g::win32_vista)
      return;

    const bool is_nt = i::is_nt();

    switch (reason) {
    case DLL_THREAD_DETACH:
      if (g::tls_index_old == 0 && g::tls_mode == d::tls_mode::by_thunk) {
        if (is_nt)
          i::free_tls_data();
        else
          i::win9x_free_tls_data();
      }
      return;

    case DLL_PROCESS_DETACH:
      if (g::tls_index_old == 0 && g::tls_mode == d::tls_mode::by_thunk) {
        if (reserved == nullptr) {
          if (is_nt)
            i::free_tls_index();
          else
            i::win9x_free_tls_index();
        }
      }
      return;
    }
  }
} // namespace mingw_thunk
