/////////////////////////////////////////////////////////////////////////////
// Private, non-installed external UIA helper protocol and completion state.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_COMBOBOX_UIA_HELPER_H
#define WX_WINUI_COMBOBOX_UIA_HELPER_H

#ifndef WXWINUI_TEST_SUPPORT
    #error This helper is only available in explicitly test-enabled builds
#endif

#include "wx/msw/wrapwin.h"
#include <atomic>
#include <memory>
#include <string>

struct wxWinUIComboAutomationTestState
{
    std::atomic<int> stage{0};
    std::atomic<long> hresult{0};
};

struct wxWinUIUiaHelperLaunchResult
{
    bool scheduled{false};
    HRESULT hresult{E_FAIL};
};

wxWinUIUiaHelperLaunchResult wxWinUILaunchUiaSetValueHelper(
    const std::shared_ptr<wxWinUIComboAutomationTestState>& operation,
    HWND bridge,
    const std::wstring& outerAutomationId,
    const std::wstring& editAutomationId,
    const std::wstring& requestedValue);

#endif
