#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  // YY-Thunks 1.1.6
  // Minimum supported client	Windows 7 [desktop apps | UWP apps]
  // Minimum supported server	Windows Server 2008 R2 [desktop apps | UWP apps]
  __DEFINE_THUNK(kernel32,
                 28,
                 BOOL,
                 WINAPI,
                 SetWaitableTimerEx,
                 _In_ HANDLE _hTimer,
                 _In_ const LARGE_INTEGER *_lpDueTime,
                 _In_ LONG _iPeriod,
                 _In_opt_ PTIMERAPCROUTINE _pfnCompletionRoutine,
                 _In_opt_ LPVOID _lpArgToCompletionRoutine,
                 _In_opt_ PREASON_CONTEXT _pWakeContext,
                 _In_ ULONG _uTolerableDelay)
  {
    if (auto const _pfnSetWaitableTimerEx = try_get_SetWaitableTimerEx()) {
      return _pfnSetWaitableTimerEx(_hTimer,
                                    _lpDueTime,
                                    _iPeriod,
                                    _pfnCompletionRoutine,
                                    _lpArgToCompletionRoutine,
                                    _pWakeContext,
                                    _uTolerableDelay);
    }

    UNREFERENCED_PARAMETER(_uTolerableDelay);

    LSTATUS _lStatus = ERROR_SUCCESS;
    // 检查 _pWakeContext 参数是否合法
    // Version 必须为 POWER_REQUEST_CONTEXT_VERSION
    // Flags 无效标记可参考 DIAGNOSTIC_REASON_INVALID_FLAGS，此外
    // POWER_REQUEST_CONTEXT_DETAILED_STRING，POWER_REQUEST_CONTEXT_SIMPLE_STRING
    // 必须设置。 有种特殊情况是
    // DIAGNOSTIC_REASON_NOT_SPECIFIED，它会被直接放行。不要问我……逆向结果如此，微软文档并未公开。
    // 微软文档链接：https://learn.microsoft.com/zh-cn/windows/win32/api/minwinbase/ns-minwinbase-reason_context
    do {
      if (!_pWakeContext)
        break;

      if (_pWakeContext->Version != POWER_REQUEST_CONTEXT_VERSION) {
        _lStatus = ERROR_INVALID_PARAMETER;
        break;
      }

      const auto _fFlags = _pWakeContext->Flags;

      // 微软原版就是如此，定义 DIAGNOSTIC_REASON_NOT_SPECIFIED 时会直接放行……
      // 我们保持微软的逻辑，这时不进一步的参数验证。
      if (_fFlags & DIAGNOSTIC_REASON_NOT_SPECIFIED)
        break;

      // 无效的Flags组合
      if ((_fFlags & DIAGNOSTIC_REASON_INVALID_FLAGS) != 0 ||
          (_fFlags & (POWER_REQUEST_CONTEXT_DETAILED_STRING |
                      POWER_REQUEST_CONTEXT_SIMPLE_STRING)) == 0) {
        _lStatus = ERROR_INVALID_PARAMETER;
        break;
      }

      if (_fFlags & POWER_REQUEST_CONTEXT_DETAILED_STRING) {
        if (_pWakeContext->Reason.Detailed.LocalizedReasonId > 0xFFFFu) {
          _lStatus = ERROR_INVALID_PARAMETER;
          break;
        }
      }
    } while (false);

    if (_lStatus != ERROR_SUCCESS) {
      SetLastError(_lStatus);
      return FALSE;
    }

    return SetWaitableTimer(_hTimer,
                            _lpDueTime,
                            _iPeriod,
                            _pfnCompletionRoutine,
                            _lpArgToCompletionRoutine,
                            _pWakeContext != nullptr);
  }
} // namespace mingw_thunk
