. profile/common/base-15.bash
. profile/common/arch-32.bash
. profile/common/crt-vc6.bash
. profile/common/thread-posix.bash

# override base
export _WINPTHREADS_USE_VEH=0

# override base
export _GCCLIB_USE_ALIGNED_MALLOC=0
