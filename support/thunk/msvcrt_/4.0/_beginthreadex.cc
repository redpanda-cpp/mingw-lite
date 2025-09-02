#include <thunk/_common.h>
#include <thunk/os.h>

#include <process.h>

namespace mingw_thunk
{
  __DEFINE_CRT_THUNK(uintptr_t,
                     _beginthreadex,
                     void *security,
                     unsigned stack_size,
                     unsigned(__stdcall *start_address)(void *),
                     void *arglist,
                     unsigned initflag,
                     unsigned *thrdaddr)
  {
    if (internal::is_nt())
      return get__beginthreadex()(
          security, stack_size, start_address, arglist, initflag, thrdaddr);

    // 9x: tid is required.
    unsigned tid = 0;
    return get__beginthreadex()(security,
                                stack_size,
                                start_address,
                                arglist,
                                initflag,
                                thrdaddr ? thrdaddr : &tid);
  }
} // namespace mingw_thunk
