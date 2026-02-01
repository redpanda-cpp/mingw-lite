#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>

#include <process.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt,
                 0,
                 uintptr_t,
                 __cdecl,
                 _beginthreadex,
                 void *security,
                 unsigned stack_size,
                 unsigned(__stdcall *start_address)(void *),
                 void *arglist,
                 unsigned initflag,
                 unsigned *thrdaddr)
  {
    if (internal::is_nt())
      return __ms__beginthreadex(
          security, stack_size, start_address, arglist, initflag, thrdaddr);

    // 9x: tid is required.
    unsigned tid = 0;
    return __ms__beginthreadex(security,
                               stack_size,
                               start_address,
                               arglist,
                               initflag,
                               thrdaddr ? thrdaddr : &tid);
  }
} // namespace mingw_thunk
