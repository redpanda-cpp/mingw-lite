#pragma once

#include <synchapi.h>

namespace mingw_thunk
{
  struct mutex
  {
    using native_handle_type = CRITICAL_SECTION;

    mutex()
    {
      InitializeCriticalSection(&cs);
    }

    mutex(const mutex &) = delete;

    ~mutex()
    {
      DeleteCriticalSection(&cs);
    }

    void lock()
    {
      EnterCriticalSection(&cs);
    }

    void try_lock()
    {
      TryEnterCriticalSection(&cs);
    }

    void unlock()
    {
      LeaveCriticalSection(&cs);
    }

  private:
    CRITICAL_SECTION cs;
  };
} // namespace mingw_thunk
