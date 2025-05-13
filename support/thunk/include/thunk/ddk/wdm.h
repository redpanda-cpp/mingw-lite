#pragma once

#include <winternl.h>

namespace mingw_thunk
{
  namespace
  {
    constexpr FILE_INFORMATION_CLASS FileIoPriorityHintInformation =
        FILE_INFORMATION_CLASS(43);
    constexpr FILE_INFORMATION_CLASS FileRemoteProtocolInformation =
        FILE_INFORMATION_CLASS(55);
  } // namespace
} // namespace mingw_thunk
