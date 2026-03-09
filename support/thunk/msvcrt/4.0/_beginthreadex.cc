#include "_beginthreadex.h"

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
    __DISPATCH_THUNK_2(_beginthreadex,
                       i::is_nt(),
                       &__ms__beginthreadex,
                       &f::win9x__beginthreadex);

    return dllimport__beginthreadex(
        security, stack_size, start_address, arglist, initflag, thrdaddr);
  }

  namespace f
  {
    uintptr_t win9x__beginthreadex(void *security,
                                   unsigned stack_size,
                                   unsigned(__stdcall *start_address)(void *),
                                   void *arglist,
                                   unsigned initflag,
                                   unsigned *thrdaddr)
    {
      // 9x: tid is required.
      unsigned tid = 0;
      return __ms__beginthreadex(security,
                                 stack_size,
                                 start_address,
                                 arglist,
                                 initflag,
                                 thrdaddr ? thrdaddr : &tid);
    }
  } // namespace f
} // namespace mingw_thunk
