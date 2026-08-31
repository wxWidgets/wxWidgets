/////////////////////////////////////////////////////////////////////////////
// Private native-resize diagnostics: never replace USER32 dispatch/wndproc.
/////////////////////////////////////////////////////////////////////////////
#ifndef WX_WINUI_HOST_RESIZE_TEST_ACCESS_H
#define WX_WINUI_HOST_RESIZE_TEST_ACCESS_H
#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif
#include "wx/defs.h"
#if wxUSE_WINUI3
#include "wx/msw/wrapwin.h"
#include <cstdint>
class wxWindow;
enum class wxWinUINativeResizePhase
{
    Idle,
    Pending,
    Entered
};

struct wxWinUINativeResizeSnapshot
{
    wxWinUINativeResizePhase phase = wxWinUINativeResizePhase::Idle;
    std::uint64_t ticket = 0;
    unsigned scheduled = 0;
    unsigned entered = 0;
    unsigned exited = 0;
    unsigned cancelled = 0;
    int hitTest = HTNOWHERE;
    POINT screenPoint = { 0, 0 };
};


// The contact reader enables bounded begin/cancel integration without input
// injection. It does not establish that a physical held drag works.
class WXDLLIMPEXP_CORE wxWinUIHostResizeTestAccess final
{
public:
    static bool GetSnapshot(wxWindow *window, wxWinUINativeResizeSnapshot *snapshot);
    static void SetContactReader(bool (*reader)(void *), void *context = nullptr);
    static void FailNextPost();
};
#endif // wxUSE_WINUI3
#endif // WX_WINUI_HOST_RESIZE_TEST_ACCESS_H
