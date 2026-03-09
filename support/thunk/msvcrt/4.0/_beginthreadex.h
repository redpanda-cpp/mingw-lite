#pragma once

#include <process.h>

namespace mingw_thunk
{
  namespace f
  {
    uintptr_t win9x__beginthreadex(void *security,
                                   unsigned stack_size,
                                   unsigned(__stdcall *start_address)(void *),
                                   void *arglist,
                                   unsigned initflag,
                                   unsigned *thrdaddr);
  } // namespace f
} // namespace mingw_thunk
