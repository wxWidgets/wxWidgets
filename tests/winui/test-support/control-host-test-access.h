/////////////////////////////////////////////////////////////////////////////
// Private, non-installed ControlHost adapters and real-host lifecycle probe.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_CONTROL_HOST_TEST_ACCESS_H
#define WX_WINUI_CONTROL_HOST_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/defs.h"
#include "wx/string.h"
#include "wx/gdicmn.h"

#include <functional>

#if wxUSE_WINUI3

// Declarations only: callers load C++/WinRT through their existing private
// header, after the Windows macro cleanup required by those projections.
namespace winrt::Microsoft::UI::Xaml
{
struct UIElement;
}

class wxWindow;
class wxWinUIControlHost;

// Intra-wxcore adapters: only the already-exported probe below crosses the
// library boundary. Hook storage is owned by the implementation TU's registry.
class wxWinUIControlHostTestAccess final
{
public:
    static void SetNextContentLoadedHook(
        wxWinUIControlHost& host, std::function<void()> hook);
    static void DispatchPendingContentLoadedHook(wxWinUIControlHost& host);
};

// ----------------------------------------------------------------------------
// wxWinUIControlHostProbe: test-only handle onto a real wxWinUIControlHost.
//
// wxWinUIControlHost is an internal (non-exported) class and, since the
// per-TLW rearchitecture, nothing in the library drives its ClearContent()
// path.  This exported probe lets the lifecycle unit tests exercise that
// path directly: create a host on a window, set/clear its content and close
// it, observing the results through the shared host's introspection
// (FindSlot / GetContent / GetLiveLoadedHookCount).
// ----------------------------------------------------------------------------

class wxWinUIControlHost;

class WXDLLIMPEXP_CORE wxWinUIControlHostProbe
{
public:
    // Build an initialized proxy without registering a slot yet. This is the
    // deterministic entry point for the first-registration reentrancy tests.
    static wxWinUIControlHostProbe *CreateEmpty(wxWindow *window);

    // Build a control host on `window` and set an initial Button content.
    // Returns null on failure.  The probe owns the control host.
    static wxWinUIControlHostProbe *Create(wxWindow *window);
    ~wxWinUIControlHostProbe();

    // wxWinUIControlHost::SetContent() with a fresh Button (a new element).
    // The label makes nested generations distinguishable in the real slot.
    bool SetContent(const wxString& label = "probe");

    // Snapshot of the proxy's own committed content model, independent of the
    // shared slot. Tests compare both identities after nested publication.
    winrt::Microsoft::UI::Xaml::UIElement GetContentForTesting() const;

    // Delete the owned wxWinUIControlHost while leaving this wrapper alive.
    // Used only to exercise destruction at the synchronous attach boundary.
    void DestroyControlHostForTesting();

    // wxWinUIControlHost::ClearContent().
    void ClearContent();

    // Drive the exact physical-pixel clip seam through the real control host.
    // Used by migration tests whose detach callback must exercise the same
    // FindSlotOwner()/DIP conversion path as wxNotebook.
    void SetBridgeClipRect(const wxRect& physicalRect);

    // wxWinUIControlHost::Close().
    void Close();

private:
    wxWinUIControlHostProbe() = default;

    wxWinUIControlHost *m_host = nullptr;

    wxDECLARE_NO_COPY_CLASS(wxWinUIControlHostProbe);
};

#endif // wxUSE_WINUI3

#endif // WX_WINUI_CONTROL_HOST_TEST_ACCESS_H
