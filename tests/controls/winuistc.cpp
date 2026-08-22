///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuistc.cpp
// Purpose:     deterministic WinUI hybrid wxStyledTextCtrl qualification
// Author:      wxWidgets development team
// Created:     2026-08-08
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_STC && wxUSE_POPUPWIN

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/button.h"
    #include "wx/frame.h"
    #include "wx/menu.h"
    #include "wx/panel.h"
    #include "wx/utils.h"
#endif // WX_PRECOMP

#if wxUSE_CLIPBOARD
    #include "wx/clipbrd.h"
    #include "wx/dataobj.h"
#endif
#include "wx/msw/wrapwin.h"
#include "wx/stopwatch.h"
#include "wx/stc/stc.h"
#include "wx/weakref.h"
#include "wx/winui/private/dialogsession.h"
#include "wx/winui/private/tlwhost.h"
#if wxUSE_MENUS
    #include "wx/winui/private/menutest.h"
#endif
#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
    #include "wx/msw/private/dropsession.h"
    #include "wx/winui/private/dropbroker.h"
#endif

// Private exported seam from ScintillaWX.cpp. Deliberately not declared in
// the public wxStyledTextCtrl header: it is only a timer-lifetime oracle.
WXDLLIMPEXP_STC wxUIntPtr
wxSTCGetCaretTimerIdForTesting(wxStyledTextCtrl* control);

namespace
{

void DrainDispatch(unsigned rounds = 20)
{
    for ( unsigned i = 0; i < rounds; ++i )
    {
        wxYield();
        wxMilliSleep(2);
    }
}

template <typename Predicate>
bool DrainUntil(Predicate predicate, unsigned rounds = 150)
{
    for ( unsigned i = 0; i < rounds; ++i )
    {
        if ( predicate() )
            return true;
        DrainDispatch(1);
    }

    return predicate();
}

bool DrainToQuiescence()
{
    for ( unsigned i = 0; i < 60; ++i )
    {
        const unsigned schedules =
            wxWinUITopLevelHost::GetFlushScheduleCount();
        const unsigned runs = wxWinUITopLevelHost::GetFlushRunCount();
        DrainDispatch(2);
        if ( schedules == wxWinUITopLevelHost::GetFlushScheduleCount() &&
             runs == wxWinUITopLevelHost::GetFlushRunCount() )
        {
            return true;
        }
    }

    return false;
}

bool SameTransientRegistry(const wxWinUITransientRegistrySnapshot& lhs,
                           const wxWinUITransientRegistrySnapshot& rhs)
{
    return lhs.ownerCount == rhs.ownerCount &&
           lhs.transientCount == rhs.transientCount &&
           lhs.cancelCallbackCount == rhs.cancelCallbackCount &&
           lhs.modalCount == rhs.modalCount &&
           lhs.popupCount == rhs.popupCount &&
           lhs.teachingTipCount == rhs.teachingTipCount &&
           lhs.pendingOwnerRetireCount == rhs.pendingOwnerRetireCount &&
           lhs.activeOwnerRetirementCount ==
               rhs.activeOwnerRetirementCount;
}

struct RuntimeSnapshot
{
    static RuntimeSnapshot Capture()
    {
        RuntimeSnapshot snapshot;
        snapshot.hosts = wxWinUITopLevelHost::GetLiveHostCount();
        snapshot.hostLifetimes =
            wxWinUITopLevelHost::GetLiveHostLifetimeCount();
        snapshot.slots = wxWinUITopLevelHost::GetLiveSlotCount();
        snapshot.slotLifetimes =
            wxWinUITopLevelHost::GetLiveSlotLifetimeCount();
        snapshot.subclasses =
            wxWinUITopLevelHost::GetLiveSubclassContextCount();
        snapshot.loadedHooks =
            wxWinUITopLevelHost::GetLiveLoadedHookCount();
        snapshot.automationObservers =
            wxWinUITopLevelHost::
                GetLiveAutomationNameStyleObserverCountForTest();
        snapshot.contentObservers =
            wxWinUITopLevelHost::GetLiveContentLoadedObserverCountForTest();
        snapshot.pendingFocusMigrations =
            wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest();
        snapshot.slotHandlerAdds =
            wxWinUITopLevelHost::GetSlotHandlerAddCount();
        snapshot.slotHandlerRevokes =
            wxWinUITopLevelHost::GetSlotHandlerRevokeCount();
        snapshot.rootHandlerAdds =
            wxWinUITopLevelHost::GetRootHandlerAddCount();
        snapshot.rootHandlerRevokes =
            wxWinUITopLevelHost::GetRootHandlerRevokeCount();
        snapshot.transients =
            wxWinUIGetTransientRegistrySnapshotForTesting();
        snapshot.wxCapture = wxWindow::GetCapture();
        snapshot.nativeCapture = ::GetCapture();
        return snapshot;
    }

    bool IsRestored() const
    {
        return wxWinUITopLevelHost::GetLiveHostCount() == hosts &&
               wxWinUITopLevelHost::GetLiveHostLifetimeCount() ==
                   hostLifetimes &&
               wxWinUITopLevelHost::GetLiveSlotCount() == slots &&
               wxWinUITopLevelHost::GetLiveSlotLifetimeCount() ==
                   slotLifetimes &&
               wxWinUITopLevelHost::GetLiveSubclassContextCount() ==
                   subclasses &&
               wxWinUITopLevelHost::GetLiveLoadedHookCount() ==
                   loadedHooks &&
               wxWinUITopLevelHost::
                       GetLiveAutomationNameStyleObserverCountForTest() ==
                   automationObservers &&
               wxWinUITopLevelHost::
                       GetLiveContentLoadedObserverCountForTest() ==
                   contentObservers &&
               wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest() ==
                   pendingFocusMigrations &&
               wxWinUITopLevelHost::GetSlotHandlerAddCount() -
                       slotHandlerAdds ==
                   wxWinUITopLevelHost::GetSlotHandlerRevokeCount() -
                       slotHandlerRevokes &&
               wxWinUITopLevelHost::GetRootHandlerAddCount() -
                       rootHandlerAdds ==
                   wxWinUITopLevelHost::GetRootHandlerRevokeCount() -
                       rootHandlerRevokes &&
               SameTransientRegistry(
                   wxWinUIGetTransientRegistrySnapshotForTesting(),
                   transients) &&
               wxWindow::GetCapture() == wxCapture &&
               ::GetCapture() == nativeCapture;
    }

    void CheckRestored() const
    {
        CHECK(wxWinUITopLevelHost::GetLiveHostCount() == hosts);
        CHECK(wxWinUITopLevelHost::GetLiveHostLifetimeCount() ==
              hostLifetimes);
        CHECK(wxWinUITopLevelHost::GetLiveSlotCount() == slots);
        CHECK(wxWinUITopLevelHost::GetLiveSlotLifetimeCount() ==
              slotLifetimes);
        CHECK(wxWinUITopLevelHost::GetLiveSubclassContextCount() ==
              subclasses);
        CHECK(wxWinUITopLevelHost::GetLiveLoadedHookCount() == loadedHooks);
        CHECK(wxWinUITopLevelHost::
                  GetLiveAutomationNameStyleObserverCountForTest() ==
              automationObservers);
        CHECK(wxWinUITopLevelHost::
                  GetLiveContentLoadedObserverCountForTest() ==
              contentObservers);
        CHECK(wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest() ==
              pendingFocusMigrations);

        const unsigned slotAdds =
            wxWinUITopLevelHost::GetSlotHandlerAddCount() - slotHandlerAdds;
        const unsigned slotRevokes =
            wxWinUITopLevelHost::GetSlotHandlerRevokeCount() -
                slotHandlerRevokes;
        CHECK(slotAdds > 0);
        CHECK(slotAdds == slotRevokes);

        const unsigned rootAdds =
            wxWinUITopLevelHost::GetRootHandlerAddCount() - rootHandlerAdds;
        const unsigned rootRevokes =
            wxWinUITopLevelHost::GetRootHandlerRevokeCount() -
                rootHandlerRevokes;
        CHECK(rootAdds > 0);
        CHECK(rootAdds == rootRevokes);

        CHECK(SameTransientRegistry(
            wxWinUIGetTransientRegistrySnapshotForTesting(), transients));
        CHECK(wxWindow::GetCapture() == wxCapture);
        CHECK(::GetCapture() == nativeCapture);
    }

    unsigned hosts = 0;
    unsigned hostLifetimes = 0;
    unsigned slots = 0;
    unsigned slotLifetimes = 0;
    unsigned subclasses = 0;
    unsigned loadedHooks = 0;
    unsigned automationObservers = 0;
    unsigned contentObservers = 0;
    unsigned pendingFocusMigrations = 0;
    unsigned slotHandlerAdds = 0;
    unsigned slotHandlerRevokes = 0;
    unsigned rootHandlerAdds = 0;
    unsigned rootHandlerRevokes = 0;
    wxWinUITransientRegistrySnapshot transients;
    wxWindow *wxCapture = nullptr;
    HWND nativeCapture = nullptr;
};

bool ShowOffscreenWithoutActivation(wxFrame *frame, int ordinal)
{
    const HWND hwnd = static_cast<HWND>(frame->GetHWND());
    if ( !hwnd )
        return false;

    ::SetWindowLongPtr(
        hwnd,
        GWL_EXSTYLE,
        ::GetWindowLongPtr(hwnd, GWL_EXSTYLE) |
            WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW);
    frame->Move(wxPoint(-30000 + ordinal * 120, -30000));
    frame->ShowWithoutActivating();
    return true;
}

class STCFixture final
{
public:
    ~STCFixture()
    {
        Cleanup();
    }

    bool CreateShells(int ordinal = 0)
    {
        wxFrame * const frameA = new wxFrame(
            nullptr, wxID_ANY, "WinUI STC fixture A",
            wxDefaultPosition, wxSize(520, 340),
            wxDEFAULT_FRAME_STYLE | wxFRAME_NO_TASKBAR |
                wxFRAME_TOOL_WINDOW);
        wxFrame * const frameB = new wxFrame(
            nullptr, wxID_ANY, "WinUI STC fixture B",
            wxDefaultPosition, wxSize(520, 340),
            wxDEFAULT_FRAME_STYLE | wxFRAME_NO_TASKBAR |
                wxFRAME_TOOL_WINDOW);
        m_frameA = wxWeakRef<wxFrame>(frameA);
        m_frameB = wxWeakRef<wxFrame>(frameB);

        wxPanel * const panelA = new wxPanel(
            frameA, wxID_ANY, wxPoint(0, 0), wxSize(500, 310));
        wxPanel * const panelB = new wxPanel(
            frameB, wxID_ANY, wxPoint(0, 0), wxSize(500, 310));
        m_panelA = wxWeakRef<wxPanel>(panelA);
        m_panelB = wxWeakRef<wxPanel>(panelB);

        // Exactly one XAML slot per TLW primes the shared host. STC remains a
        // native/custom sibling and must not manufacture another slot/island.
        wxButton * const sentinelA = new wxButton(
            panelA, wxID_ANY, "shared slot A",
            wxPoint(340, 20), wxSize(130, 36));
        wxButton * const sentinelB = new wxButton(
            panelB, wxID_ANY, "shared slot B",
            wxPoint(340, 20), wxSize(130, 36));
        m_sentinelA = wxWeakRef<wxButton>(sentinelA);
        m_sentinelB = wxWeakRef<wxButton>(sentinelB);

        if ( !ShowOffscreenWithoutActivation(frameA, ordinal) ||
             !ShowOffscreenWithoutActivation(frameB, ordinal + 1) )
        {
            return false;
        }

        if ( !DrainToQuiescence() )
            return false;

        m_hostA = wxWinUITopLevelHost::FindSlotOwner(sentinelA);
        m_hostB = wxWinUITopLevelHost::FindSlotOwner(sentinelB);
        return m_hostA && m_hostB && m_hostA != m_hostB;
    }

    wxStyledTextCtrl *CreateStyledText(wxWindow *parent = nullptr)
    {
        if ( !parent )
            parent = GetPanelA();
        if ( !parent )
            return nullptr;

        wxStyledTextCtrl * const stc = new wxStyledTextCtrl(
            parent, wxID_ANY, wxPoint(20, 75), wxSize(290, 190));
        if ( !stc->GetHWND() )
        {
            delete stc;
            return nullptr;
        }

        m_stc = wxWeakRef<wxStyledTextCtrl>(stc);
        return stc;
    }

    void Cleanup()
    {
        if ( m_cleaned )
            return;
        m_cleaned = true;

        if ( wxStyledTextCtrl * const stc = GetSTC() )
        {
            if ( stc->AutoCompActive() )
                stc->AutoCompCancel();
            if ( stc->CallTipActive() )
                stc->CallTipCancel();
            stc->SetSTCFocus(false);
        }

        if ( wxFrame * const frame = GetFrameA() )
        {
            if ( !frame->IsBeingDeleted() )
                frame->Destroy();
        }
        if ( wxFrame * const frame = GetFrameB() )
        {
            if ( !frame->IsBeingDeleted() )
                frame->Destroy();
        }

        DrainUntil(
            [this]()
            {
                return !m_frameA && !m_frameB && !m_stc;
            },
            200);
        DrainDispatch(10);
    }

    wxFrame *GetFrameA() const { return m_frameA.get(); }
    wxFrame *GetFrameB() const { return m_frameB.get(); }
    wxPanel *GetPanelA() const { return m_panelA.get(); }
    wxPanel *GetPanelB() const { return m_panelB.get(); }
    wxButton *GetSentinelA() const { return m_sentinelA.get(); }
    wxButton *GetSentinelB() const { return m_sentinelB.get(); }
    wxStyledTextCtrl *GetSTC() const { return m_stc.get(); }
    wxWinUITopLevelHost *GetHostA() const { return m_hostA; }
    wxWinUITopLevelHost *GetHostB() const { return m_hostB; }

private:
    bool m_cleaned = false;
    wxWeakRef<wxFrame> m_frameA;
    wxWeakRef<wxFrame> m_frameB;
    wxWeakRef<wxPanel> m_panelA;
    wxWeakRef<wxPanel> m_panelB;
    wxWeakRef<wxButton> m_sentinelA;
    wxWeakRef<wxButton> m_sentinelB;
    wxWeakRef<wxStyledTextCtrl> m_stc;
    wxWinUITopLevelHost *m_hostA = nullptr;
    wxWinUITopLevelHost *m_hostB = nullptr;
};

wxString GetNativeClassName(HWND hwnd)
{
    wchar_t className[160] = L"";
    if ( hwnd )
        ::GetClassNameW(hwnd, className, WXSIZEOF(className));
    return wxString(className);
}

unsigned CountDescendantWindows(HWND parent, const wxString *exactClass)
{
    struct Context
    {
        const wxString *exactClass;
        unsigned count;
    } context = { exactClass, 0 };

    ::EnumChildWindows(
        parent,
        [](HWND hwnd, LPARAM data) -> BOOL
        {
            Context * const context = reinterpret_cast<Context *>(data);
            if ( !context->exactClass ||
                 GetNativeClassName(hwnd) == *context->exactClass )
            {
                ++context->count;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&context));
    return context.count;
}

unsigned CountSiteBridges(HWND tlw)
{
    struct Counter
    {
        unsigned count;
    } counter = { 0 };

    ::EnumChildWindows(
        tlw,
        [](HWND hwnd, LPARAM data) -> BOOL
        {
            Counter * const counter = reinterpret_cast<Counter *>(data);
            if ( GetNativeClassName(hwnd).Contains("DesktopChildSiteBridge") )
                ++counter->count;
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&counter));
    return counter.count;
}

bool IsSiblingAbove(HWND upper, HWND lower)
{
    if ( !upper || !lower || ::GetParent(upper) != ::GetParent(lower) )
        return false;

    for ( HWND hwnd = ::GetWindow(lower, GW_HWNDPREV);
          hwnd;
          hwnd = ::GetWindow(hwnd, GW_HWNDPREV) )
    {
        if ( hwnd == upper )
            return true;
    }

    return false;
}

wxWindow *FindDescendantNamed(wxWindow *root, const wxString& name)
{
    if ( !root )
        return nullptr;

    for ( wxWindow * const child : root->GetChildren() )
    {
        if ( child->GetName() == name )
            return child;
        if ( wxWindow * const found = FindDescendantNamed(child, name) )
            return found;
    }

    return nullptr;
}

wxWindow *FindShownPopupDescendantNamed(wxStyledTextCtrl *stc,
                                        const wxString& name)
{
    if ( !stc )
        return nullptr;

    // A hidden popup wrapper can remain in wxPendingDelete while a new
    // visibility generation is already live. Looking through the whole wx
    // child list would then select the retiring generation first.
    for ( wxWindow * const popup : stc->GetChildren() )
    {
        if ( popup->IsShown() )
        {
            if ( wxWindow * const found = FindDescendantNamed(popup, name) )
                return found;
        }
    }

    return nullptr;
}

wxWindow *GetPopupRoot(wxStyledTextCtrl *stc, wxWindow *descendant)
{
    wxWindow *root = descendant;
    while ( root && root->GetParent() != stc )
        root = root->GetParent();
    return root;
}

bool PopupRegistryRestored(const wxWinUITransientRegistrySnapshot& before)
{
    return SameTransientRegistry(
        wxWinUIGetTransientRegistrySnapshotForTesting(), before);
}

struct NativeTimerIdentity
{
    HWND hwnd = nullptr;
    UINT_PTR id = 0;
    unsigned deliveries = 0;
};

bool IsWxTimerMessage(const MSG& message)
{
    return message.message == WM_TIMER && message.hwnd &&
           GetNativeClassName(message.hwnd).Contains("wxTimerHiddenWindow");
}

bool DispatchUntilWxTimer(NativeTimerIdentity *identity,
                          UINT_PTR expectedId,
                          long timeoutMilliseconds = 750)
{
    wxStopWatch elapsed;
    do
    {
        MSG message{};
        bool dispatched = false;
        while ( ::PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE) )
        {
            dispatched = true;
            if ( message.message == WM_QUIT )
            {
                ::PostQuitMessage(static_cast<int>(message.wParam));
                return false;
            }

            const bool timer = IsWxTimerMessage(message) &&
                static_cast<UINT_PTR>(message.wParam) == expectedId;
            if ( timer && identity )
            {
                identity->hwnd = message.hwnd;
                identity->id = expectedId;
                ++identity->deliveries;
            }

            ::TranslateMessage(&message);
            ::DispatchMessageW(&message);
            if ( timer )
                return true;
        }

        if ( !dispatched )
            wxMilliSleep(1);
    }
    while ( elapsed.Time() < timeoutMilliseconds );

    return false;
}

unsigned DispatchAndCountTimer(const NativeTimerIdentity& identity,
                               long observationMilliseconds)
{
    unsigned deliveries = 0;
    wxStopWatch elapsed;
    do
    {
        MSG message{};
        bool dispatched = false;
        while ( ::PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE) )
        {
            dispatched = true;
            if ( message.message == WM_QUIT )
            {
                ::PostQuitMessage(static_cast<int>(message.wParam));
                return deliveries;
            }

            if ( message.message == WM_TIMER &&
                 message.hwnd == identity.hwnd &&
                 static_cast<UINT_PTR>(message.wParam) == identity.id )
            {
                ++deliveries;
            }

            ::TranslateMessage(&message);
            ::DispatchMessageW(&message);
        }

        if ( !dispatched )
            wxMilliSleep(1);
    }
    while ( elapsed.Time() < observationMilliseconds );

    return deliveries;
}

#if wxUSE_MENUS
class PopupMenuFailureReset final
{
public:
    ~PopupMenuFailureReset()
    {
        wxWinUI3FailNextPopupTimersForTesting(false, false);
    }
};
#endif

} // anonymous namespace

TEST_CASE("WinUISTC::SingleNativeSurfaceSharedHostGeometryFocusAndZ",
          "[winui-stc][winui-host][winui-zorder]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();

    STCFixture fixture;
    REQUIRE(fixture.CreateShells());
    REQUIRE(fixture.GetHostA());
    REQUIRE(fixture.GetHostB());

    const unsigned hostsBeforeSTC =
        wxWinUITopLevelHost::GetLiveHostCount();
    const unsigned slotsBeforeSTC =
        wxWinUITopLevelHost::GetLiveSlotCount();
    const unsigned subclassesBeforeSTC =
        wxWinUITopLevelHost::GetLiveSubclassContextCount();
    wxStyledTextCtrl * const stc = fixture.CreateStyledText();
    REQUIRE(stc);

    const HWND hwndSTC = static_cast<HWND>(stc->GetHWND());
    const HWND hwndFrameA = static_cast<HWND>(fixture.GetFrameA()->GetHWND());
    const wxString scintillaClass = GetNativeClassName(hwndSTC);
    REQUIRE_FALSE(scintillaClass.empty());
    CHECK(scintillaClass.Contains("Scintilla"));
    CHECK(CountDescendantWindows(hwndFrameA, &scintillaClass) == 1);
    CHECK(CountDescendantWindows(hwndSTC, nullptr) == 0);
    CHECK(::GetParent(hwndSTC) ==
          static_cast<HWND>(fixture.GetPanelA()->GetHWND()));

    // The STC HWND is the rendering surface. It contributes no XAML slot,
    // host, island bridge or host-owned subclass context of its own.
    CHECK(wxWinUITopLevelHost::GetLiveHostCount() == hostsBeforeSTC);
    CHECK(wxWinUITopLevelHost::GetLiveSlotCount() == slotsBeforeSTC);
    CHECK(wxWinUITopLevelHost::GetLiveSubclassContextCount() ==
          subclassesBeforeSTC);
    CHECK(wxWinUITopLevelHost::FindSlotOwner(stc) == nullptr);
    CHECK(fixture.GetHostA()->FindSlot(stc) == nullptr);
    CHECK(fixture.GetHostB()->FindSlot(stc) == nullptr);
    CHECK(CountSiteBridges(hwndFrameA) == 1);
    CHECK(CountSiteBridges(
              static_cast<HWND>(fixture.GetFrameB()->GetHWND())) == 1);

    stc->SetSize(28, 82, 264, 156);
    RECT nativeRect{};
    REQUIRE(::GetWindowRect(hwndSTC, &nativeRect));
    CHECK(nativeRect.right - nativeRect.left == 264);
    CHECK(nativeRect.bottom - nativeRect.top == 156);
    CHECK(stc->GetClientSize().x > 0);
    CHECK(stc->GetClientSize().y > 0);

    // Exercise Scintilla's focus/caret model without activating either TLW or
    // synthesizing physical keyboard input on the user's desktop.
    stc->SetSTCFocus(true);
    CHECK(stc->GetSTCFocus());
    stc->GotoPos(0);
    CHECK(stc->GetCurrentPos() == 0);
    stc->SetSTCFocus(false);
    CHECK_FALSE(stc->GetSTCFocus());

    const HWND bridge = fixture.GetHostA()->GetBridgeHwnd();
    const HWND nativeBand =
        static_cast<HWND>(fixture.GetPanelA()->GetHWND());
    REQUIRE(bridge);
    REQUIRE(nativeBand);
    REQUIRE(::GetParent(bridge) == ::GetParent(nativeBand));
    fixture.GetHostA()->FlushSync();
    CHECK(IsSiblingAbove(bridge, nativeBand));
    stc->Raise();
    fixture.GetHostA()->FlushSync();
    CHECK(IsSiblingAbove(bridge, nativeBand));
    stc->Lower();
    fixture.GetHostA()->FlushSync();
    CHECK(IsSiblingAbove(bridge, nativeBand));

    fixture.Cleanup();
    REQUIRE(DrainUntil([&before]() { return before.IsRestored(); }));
    before.CheckRestored();
}

TEST_CASE("WinUISTC::TextSelectionUndoStylesMarkersFoldsSearchAndLargeBuffer",
          "[winui-stc][model][large-buffer]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();

    STCFixture fixture;
    REQUIRE(fixture.CreateShells(2));
    wxStyledTextCtrl * const stc = fixture.CreateStyledText();
    REQUIRE(stc);

    const wxString original = "alpha\nbeta\ngamma\n";
    stc->SetText(original);
    stc->EmptyUndoBuffer();
    stc->SetSavePoint();
    CHECK(stc->GetText() == original);
    CHECK(stc->GetLineCount() == 4);

    stc->SetSelection(1, 4);
    CHECK(stc->GetSelectedText() == "lph");
    CHECK(stc->GetSelectionStart() == 1);
    CHECK(stc->GetSelectionEnd() == 4);
    CHECK(stc->GetAnchor() == 1);
    CHECK(stc->GetCurrentPos() == 4);

    stc->BeginUndoAction();
    stc->ReplaceSelection("LPH");
    stc->InsertText(stc->GetLength(), "delta\n");
    stc->EndUndoAction();
    REQUIRE(stc->CanUndo());
    CHECK(stc->GetText() == "aLPHa\nbeta\ngamma\ndelta\n");
    stc->Undo();
    CHECK(stc->GetText() == original);
    REQUIRE(stc->CanRedo());
    stc->Redo();
    CHECK(stc->GetText() == "aLPHa\nbeta\ngamma\ndelta\n");

    stc->GotoPos(stc->PositionFromLine(1) + 2);
    CHECK(stc->GetCurrentLine() == 1);
    CHECK(stc->GetColumn(stc->GetCurrentPos()) == 2);

    stc->StartStyling(0);
    stc->SetStyling(5, 17);
    CHECK(stc->GetStyleAt(0) == 17);
    CHECK(stc->GetStyleAt(4) == 17);

    constexpr int MarkerNumber = 3;
    stc->MarkerDefine(MarkerNumber, wxSTC_MARK_CIRCLE);
    const int markerHandle = stc->MarkerAdd(1, MarkerNumber);
    REQUIRE(markerHandle >= 0);
    CHECK((stc->MarkerGet(1) & (1 << MarkerNumber)) != 0);
    CHECK(stc->MarkerLineFromHandle(markerHandle) == 1);
    stc->MarkerDeleteHandle(markerHandle);
    CHECK((stc->MarkerGet(1) & (1 << MarkerNumber)) == 0);

    stc->SetFoldLevel(
        0, wxSTC_FOLDLEVELBASE | wxSTC_FOLDLEVELHEADERFLAG);
    stc->SetFoldLevel(1, wxSTC_FOLDLEVELBASE + 1);
    stc->SetFoldExpanded(0, true);
    stc->ToggleFold(0);
    CHECK_FALSE(stc->GetFoldExpanded(0));
    stc->ToggleFold(0);
    CHECK(stc->GetFoldExpanded(0));

    stc->SetSearchFlags(wxSTC_FIND_MATCHCASE);
    stc->SetTargetStart(0);
    stc->SetTargetEnd(stc->GetLength());
    const int found = stc->SearchInTarget("gamma");
    REQUIRE(found >= 0);
    CHECK(stc->GetTextRange(found, stc->GetTargetEnd()) == "gamma");
    CHECK(stc->GetTargetStart() == found);

    wxString large;
    large.reserve(180000);
    for ( int line = 0; line < 5000; ++line )
    {
        large += wxString::Format(
            "line-%04d deterministic payload 0123456789\n", line);
    }
    stc->SetText(large);
    CHECK(stc->GetLength() == static_cast<int>(large.length()));
    CHECK(stc->GetLineCount() == 5001);
    CHECK(stc->GetLine(4321).StartsWith("line-4321"));
    stc->SetFirstVisibleLine(4000);
    CHECK(stc->GetFirstVisibleLine() == 4000);
    stc->GotoLine(4500);
    stc->EnsureCaretVisible();
    CHECK(stc->GetCurrentLine() == 4500);
    CHECK(stc->GetFirstVisibleLine() > 0);

    stc->SetTargetStart(stc->PositionFromLine(4900));
    stc->SetTargetEnd(stc->GetLength());
    CHECK(stc->SearchInTarget("line-4999") >= 0);

    fixture.Cleanup();
    REQUIRE(DrainUntil([&before]() { return before.IsRestored(); }));
    before.CheckRestored();
}

#if wxUSE_CLIPBOARD
TEST_CASE("WinUISTC::ClipboardCopyPasteUsesPublicAPI",
          "[.][winui-stc-clipboard-manual][clipboard]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();

    STCFixture fixture;
    REQUIRE(fixture.CreateShells(4));
    wxStyledTextCtrl * const stc = fixture.CreateStyledText();
    REQUIRE(stc);

    // This explicit manual gate touches the window-station clipboard. It is
    // hidden from the automatic [winui-stc] suite and never uses simulated
    // Ctrl+C/Ctrl+V input.
    {
        wxClipboardLocker clipboard;
        REQUIRE_FALSE(!clipboard);
        REQUIRE(wxTheClipboard->SetData(
            new wxTextDataObject("clipboard")));
    }

    stc->SetText("tail-");
    stc->GotoPos(stc->GetLength());
    REQUIRE(stc->CanPaste());
    stc->Paste();
    CHECK(stc->GetText() == "tail-clipboard");

    stc->SetSelection(0, 4);
    stc->Copy();
    {
        wxClipboardLocker clipboard;
        REQUIRE_FALSE(!clipboard);
        wxTextDataObject copied;
        REQUIRE(wxTheClipboard->GetData(copied));
        CHECK(copied.GetText() == "tail");
    }

    fixture.Cleanup();
    REQUIRE(DrainUntil([&before]() { return before.IsRestored(); }));
    before.CheckRestored();
}
#endif // wxUSE_CLIPBOARD

TEST_CASE("WinUISTC::AutocompleteAndCallTipUseTransientOwnerAndAPIClick",
          "[winui-stc][popup][transient]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();

    STCFixture fixture;
    REQUIRE(fixture.CreateShells(6));
    wxStyledTextCtrl * const stc = fixture.CreateStyledText();
    REQUIRE(stc);
    wxFrame * const frame = fixture.GetFrameA();
    REQUIRE(frame);

    const unsigned hosts = wxWinUITopLevelHost::GetLiveHostCount();
    const unsigned slots = wxWinUITopLevelHost::GetLiveSlotCount();
    const wxWinUITransientRegistrySnapshot transients =
        wxWinUIGetTransientRegistrySnapshotForTesting();

    stc->SetText("ab");
    stc->GotoPos(stc->GetLength());
    stc->SetSTCFocus(true);
    stc->AutoCompShow(2, "ability able about above");
    REQUIRE(stc->AutoCompActive());
    // This API returns the caret position at popup creation, not the start of
    // the prefix selected by lengthEntered.
    CHECK(stc->AutoCompPosStart() == 2);
    CHECK(wxWinUITransientCountForTesting(
              frame, wxWinUITransientKind::Popup) == 1);

    wxWindow * const list = FindDescendantNamed(stc, "AutoCompListBox");
    REQUIRE(list);
    wxWindow * const listPopup = GetPopupRoot(stc, list);
    REQUIRE(listPopup);
    const HWND listPopupHwnd = static_cast<HWND>(listPopup->GetHWND());
    REQUIRE(listPopupHwnd);
    CHECK((::GetWindowLongPtr(listPopupHwnd, GWL_EXSTYLE) &
           WS_EX_NOACTIVATE) != 0);
    CHECK(::GetWindow(listPopupHwnd, GW_OWNER) ==
          static_cast<HWND>(frame->GetHWND()));
    CHECK(CountSiteBridges(listPopupHwnd) == 0);
    CHECK(wxWinUITopLevelHost::GetLiveHostCount() == hosts);
    CHECK(wxWinUITopLevelHost::GetLiveSlotCount() == slots);

    stc->AutoCompSelect("ability");
    CHECK(stc->AutoCompGetCurrentText() == "ability");
    stc->AutoCompComplete();
    CHECK_FALSE(stc->AutoCompActive());
    CHECK(stc->GetText() == "ability");
    REQUIRE(DrainUntil(
        [frame]()
        {
            return wxWinUITransientCountForTesting(
                       frame, wxWinUITransientKind::Popup) == 0;
        }));

    unsigned callTipClicks = 0;
    int callTipClickPosition = -1;
    stc->Bind(
        wxEVT_STC_CALLTIP_CLICK,
        [&](wxStyledTextEvent& event)
        {
            ++callTipClicks;
            callTipClickPosition = event.GetPosition();
        });
    stc->CallTipShow(stc->GetCurrentPos(), "deterministic call tip");
    REQUIRE(stc->CallTipActive());
    CHECK(wxWinUITransientCountForTesting(
              frame, wxWinUITransientKind::Popup) == 1);

    wxWindow * const callTip = FindDescendantNamed(stc, "wxSTCCallTip");
    REQUIRE(callTip);
    const HWND callTipHwnd = static_cast<HWND>(callTip->GetHWND());
    REQUIRE(callTipHwnd);
    CHECK((::GetWindowLongPtr(callTipHwnd, GWL_EXSTYLE) &
           WS_EX_NOACTIVATE) != 0);
    CHECK(::GetWindow(callTipHwnd, GW_OWNER) ==
          static_cast<HWND>(frame->GetHWND()));

    // Deliver directly to the public wx event surface. This exercises the
    // exact call-tip click notification without a screen coordinate or mouse.
    wxMouseEvent click(wxEVT_LEFT_DOWN);
    click.SetEventObject(callTip);
    click.SetPosition(wxPoint(2, 2));
    REQUIRE(callTip->GetEventHandler()->ProcessEvent(click));
    CHECK(callTipClicks == 1);
    CHECK(callTipClickPosition == 0);
    CHECK(stc->GetSTCFocus());

    stc->CallTipCancel();
    stc->SetSTCFocus(false);
    REQUIRE(DrainUntil(
        [&transients]() { return PopupRegistryRestored(transients); }));
    CHECK(wxWinUITopLevelHost::GetLiveHostCount() == hosts);
    CHECK(wxWinUITopLevelHost::GetLiveSlotCount() == slots);

    fixture.Cleanup();
    REQUIRE(DrainUntil([&before]() { return before.IsRestored(); }));
    before.CheckRestored();
}

#if wxUSE_MENUS || wxUSE_DRAG_AND_DROP
TEST_CASE("WinUISTC::LogicalContextMenuAndDragDropUseCommonBrokers",
          "[winui-stc][menu][dnd]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();

    STCFixture fixture;
    REQUIRE(fixture.CreateShells(8));
    wxStyledTextCtrl * const stc = fixture.CreateStyledText();
    REQUIRE(stc);

#if wxUSE_MENUS
    unsigned contextEvents = 0;
    stc->UsePopUp(wxSTC_POPUP_NEVER);
    stc->Bind(
        wxEVT_CONTEXT_MENU,
        [&](wxContextMenuEvent&)
        {
            ++contextEvents;
        });
    wxContextMenuEvent context(
        wxEVT_CONTEXT_MENU, stc->GetId(), wxPoint(-1, -1));
    context.SetEventObject(stc);
    REQUIRE(stc->GetEventHandler()->ProcessEvent(context));
    CHECK(contextEvents == 1);

    wxMenu menu;
    menu.Append(wxID_HIGHEST + 1401, "Deterministic STC command");
    unsigned opens = 0;
    unsigned closes = 0;
    stc->Bind(
        wxEVT_MENU_OPEN,
        [&](wxMenuEvent& event)
        {
            ++opens;
            event.Skip();
        });
    stc->Bind(
        wxEVT_MENU_CLOSE,
        [&](wxMenuEvent& event)
        {
            ++closes;
            event.Skip();
        });

    PopupMenuFailureReset resetPopupFailures;
    wxWinUI3FailNextPopupTimersForTesting(true, true);
    wxStopWatch popupElapsed;
    CHECK(wxWinUI3PopupMenuForTesting(stc, &menu, 6, 6));
    CHECK(popupElapsed.Time() < 2000);
    CHECK(opens == 1);
    CHECK(closes == 1);
#endif // wxUSE_MENUS

#if wxUSE_DRAG_AND_DROP
    unsigned dragOvers = 0;
    unsigned drops = 0;
    stc->SetText("drop-base");
    stc->Bind(
        wxEVT_STC_DRAG_OVER,
        [&](wxStyledTextEvent& event)
        {
            ++dragOvers;
            event.SetDragResult(wxDragMove);
        });
    stc->Bind(
        wxEVT_STC_DO_DROP,
        [&](wxStyledTextEvent& event)
        {
            ++drops;
            event.SetPosition(stc->GetLength());
            event.SetString("::drop");
            event.SetDragResult(wxDragCopy);
        });

    CHECK(stc->DoDragEnter(3, 3, wxDragCopy) == wxDragCopy);
    CHECK(stc->DoDragOver(4, 4, wxDragCopy) == wxDragMove);
    CHECK(dragOvers == 1);
    CHECK(stc->DoDropText(4, 4, "ignored"));
    CHECK(drops == 1);
    CHECK(stc->GetText() == "drop-base::drop");
    stc->DoDragLeave();

#if wxUSE_OLE
    wxDropTarget * const target = stc->GetDropTarget();
    REQUIRE(target);
    wxMSWOleDropTargetBinding binding;
    REQUIRE(wxMSWOleLookupDropTarget(stc, &binding) ==
            wxMSWOleDropTargetLookup::Found);
    CHECK(binding.GetTargetIfCurrent() == target);
    CHECK(binding.GetOwnerIfCurrent() == stc);
    CHECK(binding.GetOwnerHwndIfCurrent() == stc->GetHWND());
    REQUIRE(fixture.GetHostA()->OwnsOleDropRegistration());
    REQUIRE(fixture.GetHostA()->GetDropBrokerForTest());
    CHECK(fixture.GetHostA()
              ->GetDropBrokerForTest()
              ->GetSnapshotForTest()
              .active);
    CHECK_FALSE(wxMSWOleIsShellDropTargetRegistered(
        target, stc->GetHWND()));
#endif // wxUSE_OLE
#endif // wxUSE_DRAG_AND_DROP

    fixture.Cleanup();
    REQUIRE(DrainUntil([&before]() { return before.IsRestored(); }));
    before.CheckRestored();
}
#endif // wxUSE_MENUS || wxUSE_DRAG_AND_DROP

TEST_CASE("WinUISTC::TwoTLWReparentPopupOwnerAndHundredCycles",
          "[winui-stc][reparent][popup][stress]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();

    STCFixture fixture;
    REQUIRE(fixture.CreateShells(10));
    wxStyledTextCtrl * const stc = fixture.CreateStyledText();
    REQUIRE(stc);
    const HWND hwndSTC = static_cast<HWND>(stc->GetHWND());
    const wxString scintillaClass = GetNativeClassName(hwndSTC);
    const unsigned steadyHosts = wxWinUITopLevelHost::GetLiveHostCount();
    const unsigned steadySlots = wxWinUITopLevelHost::GetLiveSlotCount();
    const unsigned steadySubclasses =
        wxWinUITopLevelHost::GetLiveSubclassContextCount();

    stc->SetText("reparent-stable");
    stc->SetSTCFocus(true);
    unsigned autoCancelNotifications = 0;
    stc->Bind(
        wxEVT_STC_AUTOCOMP_CANCELLED,
        [&autoCancelNotifications](wxStyledTextEvent&)
        {
            ++autoCancelNotifications;
        });
    stc->AutoCompShow(0, "active-before-reparent");
    REQUIRE(stc->AutoCompActive());
    REQUIRE(wxWinUITransientCountForTesting(
                fixture.GetFrameA(), wxWinUITransientKind::Popup) == 1);
    wxWindow * const oldList =
        FindDescendantNamed(stc, "AutoCompListBox");
    REQUIRE(oldList);
    wxWindow * const oldPopup = GetPopupRoot(stc, oldList);
    REQUIRE(oldPopup);
    const wxWeakRef<wxWindow> oldPopupLifetime(oldPopup);

    // The old transient generation must retire synchronously at SetParent's
    // HWND boundary. It must never survive under A while the STC belongs to B.
    REQUIRE(stc->Reparent(fixture.GetPanelB()));
    CHECK_FALSE(stc->AutoCompActive());
    CHECK(autoCancelNotifications == 0);
    REQUIRE(DrainUntil(
        [&fixture]()
        {
            return wxWinUITransientCountForTesting(
                       fixture.GetFrameA(),
                       wxWinUITransientKind::Popup) == 0 &&
                   wxWinUITransientCountForTesting(
                       fixture.GetFrameB(),
                       wxWinUITransientKind::Popup) == 0;
        }));
    CHECK(static_cast<HWND>(stc->GetHWND()) == hwndSTC);
    CHECK(stc->GetSTCFocus());

    stc->AutoCompShow(0, "owned-by-B");
    REQUIRE(stc->AutoCompActive());
    wxWindow * const list =
        FindShownPopupDescendantNamed(stc, "AutoCompListBox");
    REQUIRE(list);
    wxWindow * const popup = GetPopupRoot(stc, list);
    REQUIRE(popup);
    CHECK(popup != oldPopupLifetime.get());
    const wxWeakRef<wxWindow> popupLifetime(popup);
    const HWND popupHwnd = static_cast<HWND>(popup->GetHWND());
    REQUIRE(popupHwnd);
    const HWND nativeOwner = ::GetWindow(popupHwnd, GW_OWNER);
    INFO("popup=" << reinterpret_cast<UINT_PTR>(popupHwnd)
         << ", native owner=" << reinterpret_cast<UINT_PTR>(nativeOwner)
         << " (" << GetNativeClassName(nativeOwner).ToStdString() << ")"
         << ", frame A=" << reinterpret_cast<UINT_PTR>(
                static_cast<HWND>(fixture.GetFrameA()->GetHWND()))
         << ", frame B=" << reinterpret_cast<UINT_PTR>(
                static_cast<HWND>(fixture.GetFrameB()->GetHWND()))
         << ", STC=" << reinterpret_cast<UINT_PTR>(hwndSTC));
    CHECK(nativeOwner ==
          static_cast<HWND>(fixture.GetFrameB()->GetHWND()));

    RECT popupBeforeMove{};
    REQUIRE(::GetWindowRect(popupHwnd, &popupBeforeMove));
    const wxPoint frameBPosition = fixture.GetFrameB()->GetPosition();
    fixture.GetFrameB()->Move(frameBPosition + wxPoint(37, 19));
    DrainDispatch(5);
    CHECK_FALSE(oldPopupLifetime);
    REQUIRE(popupLifetime);
    REQUIRE(::IsWindow(popupHwnd));
    RECT popupAfterMove{};
    REQUIRE(::GetWindowRect(popupHwnd, &popupAfterMove));
    CHECK(popupAfterMove.left - popupBeforeMove.left == 37);
    CHECK(popupAfterMove.top - popupBeforeMove.top == 19);
    stc->AutoCompCancel();
    stc->SetSTCFocus(false);

    for ( unsigned cycle = 0; cycle < 100; ++cycle )
    {
        CAPTURE(cycle);
        wxPanel * const targetPanel =
            cycle % 2 == 0 ? fixture.GetPanelA() : fixture.GetPanelB();
        wxFrame * const targetFrame =
            cycle % 2 == 0 ? fixture.GetFrameA() : fixture.GetFrameB();
        wxFrame * const otherFrame =
            cycle % 2 == 0 ? fixture.GetFrameB() : fixture.GetFrameA();
        wxWinUITopLevelHost * const targetHost =
            cycle % 2 == 0 ? fixture.GetHostA() : fixture.GetHostB();

        REQUIRE(stc->Reparent(targetPanel));
        CHECK(stc->GetParent() == targetPanel);
        CHECK(static_cast<HWND>(stc->GetHWND()) == hwndSTC);
        CHECK(::GetParent(hwndSTC) ==
              static_cast<HWND>(targetPanel->GetHWND()));
        CHECK(wxWinUITopLevelHost::FindSlotOwner(stc) == nullptr);
        CHECK(targetHost->FindSlot(stc) == nullptr);
        CHECK(CountDescendantWindows(
                  static_cast<HWND>(targetFrame->GetHWND()),
                  &scintillaClass) == 1);
        CHECK(CountDescendantWindows(
                  static_cast<HWND>(otherFrame->GetHWND()),
                  &scintillaClass) == 0);

        stc->SetText(wxString::Format("cycle-%03u", cycle));
        if ( cycle % 2 == 0 )
            stc->AutoCompShow(0, "cycle-item");
        else
            stc->CallTipShow(0, "cycle-tip");

        CHECK(wxWinUITransientCountForTesting(
                  targetFrame, wxWinUITransientKind::Popup) == 1);
        CHECK(wxWinUITransientCountForTesting(
                  otherFrame, wxWinUITransientKind::Popup) == 0);
        if ( stc->AutoCompActive() )
            stc->AutoCompCancel();
        if ( stc->CallTipActive() )
            stc->CallTipCancel();
        DrainDispatch(1);
        REQUIRE(DrainUntil(
            [targetFrame]()
            {
                return wxWinUITransientCountForTesting(
                           targetFrame,
                           wxWinUITransientKind::Popup) == 0;
            },
            30));

#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
        wxMSWOleDropTargetBinding binding;
        REQUIRE(wxMSWOleLookupDropTarget(stc, &binding) ==
                wxMSWOleDropTargetLookup::Found);
        CHECK(binding.GetOwnerIfCurrent() == stc);
        CHECK(binding.GetOwnerHwndIfCurrent() == stc->GetHWND());
        CHECK_FALSE(wxMSWOleIsShellDropTargetRegistered(
            stc->GetDropTarget(), stc->GetHWND()));
#endif

        CHECK(wxWinUITopLevelHost::GetLiveHostCount() == steadyHosts);
        CHECK(wxWinUITopLevelHost::GetLiveSlotCount() == steadySlots);
        CHECK(wxWinUITopLevelHost::GetLiveSubclassContextCount() ==
              steadySubclasses);
        CHECK(CountSiteBridges(
                  static_cast<HWND>(targetFrame->GetHWND())) == 1);
    }

    fixture.Cleanup();
    REQUIRE(DrainUntil([&before]() { return before.IsRestored(); }, 250));
    before.CheckRestored();
}

TEST_CASE("WinUISTC::NestedReparentDuringPopupRetirementConvergesSafely",
          "[winui-stc][reparent][popup][reentrant]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();

    STCFixture fixture;
    REQUIRE(fixture.CreateShells(15));
    wxStyledTextCtrl * const stc = fixture.CreateStyledText();
    REQUIRE(stc);
    const HWND hwndSTC = static_cast<HWND>(stc->GetHWND());
    const wxWeakRef<wxStyledTextCtrl> stcLifetime(stc);

    stc->AutoCompShow(0, "outer-popup");
    REQUIRE(stc->AutoCompActive());
    wxWindow * const list = FindDescendantNamed(stc, "AutoCompListBox");
    REQUIRE(list);
    wxWindow * const listPopup = GetPopupRoot(stc, list);
    REQUIRE(listPopup);

    bool callbackRan = false;
    bool nestedReparentAccepted = false;
    wxWinUITransientRegistration nested = wxWinUIRegisterTransient(
        listPopup,
        wxWinUITransientKind::TeachingTip,
        [&]()
        {
            callbackRan = true;
            if ( wxStyledTextCtrl * const live = stcLifetime.get() )
                nestedReparentAccepted = live->Reparent(fixture.GetPanelA());
        });
    REQUIRE(nested);
    CHECK(wxWinUITransientCountForTesting(
              listPopup,
              wxWinUITransientKind::TeachingTip) == 1);

    // The outer A->B USER32 SetParent boundary retires autocomplete. The
    // common popup ending transaction cancels a popup-owned transient which
    // attempts B->A while Window::Destroy() and the outer SetParent are still
    // on the stack. The nested call is accepted, but USER32 completes the
    // already-running outer SetParent last, so B is the native winner. The
    // important invariant is that the same wid is not destroyed twice and
    // the wx/native topology converges on that winner.
    const bool outerReparentAccepted = stc->Reparent(fixture.GetPanelB());
    CHECK(outerReparentAccepted);
    CHECK(callbackRan);
    CHECK(nestedReparentAccepted);
    REQUIRE(stcLifetime);
    CHECK(stc->GetParent() == fixture.GetPanelB());
    CHECK(static_cast<HWND>(stc->GetHWND()) == hwndSTC);
    CHECK(::GetParent(hwndSTC) ==
          static_cast<HWND>(fixture.GetPanelB()->GetHWND()));
    CHECK_FALSE(stc->AutoCompActive());
    CHECK_FALSE(static_cast<bool>(nested));
    CHECK(wxWinUITransientCountForTesting(
              fixture.GetFrameA(), wxWinUITransientKind::TeachingTip) == 0);
    CHECK(wxWinUITransientCountForTesting(
              fixture.GetFrameB(), wxWinUITransientKind::TeachingTip) == 0);

    // The next popup generation must follow the converged native winner.
    stc->AutoCompShow(0, "after-nested-reparent");
    REQUIRE(stc->AutoCompActive());
    wxWindow * const nextList =
        FindShownPopupDescendantNamed(stc, "AutoCompListBox");
    REQUIRE(nextList);
    wxWindow * const nextPopup = GetPopupRoot(stc, nextList);
    REQUIRE(nextPopup);
    CHECK(::GetWindow(static_cast<HWND>(nextPopup->GetHWND()), GW_OWNER) ==
          static_cast<HWND>(fixture.GetFrameB()->GetHWND()));
    stc->AutoCompCancel();

    fixture.Cleanup();
    REQUIRE(DrainUntil([&before]() { return before.IsRestored(); }, 250));
    before.CheckRestored();
}

TEST_CASE("WinUISTC::DestroyDuringPopupRetirementStopsStaleWindowMessage",
          "[winui-stc][reparent][popup][destroy][reentrant]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();

    STCFixture fixture;
    REQUIRE(fixture.CreateShells(17));
    wxStyledTextCtrl * const stc = fixture.CreateStyledText();
    REQUIRE(stc);
    const wxWeakRef<wxStyledTextCtrl> stcLifetime(stc);

    stc->AutoCompShow(0, "destroy-during-retirement");
    REQUIRE(stc->AutoCompActive());
    wxWindow * const list = FindDescendantNamed(stc, "AutoCompListBox");
    REQUIRE(list);
    wxWindow * const listPopup = GetPopupRoot(stc, list);
    REQUIRE(listPopup);

    bool callbackRan = false;
    bool destroyAccepted = false;
    wxWinUITransientRegistration nested = wxWinUIRegisterTransient(
        listPopup,
        wxWinUITransientKind::TeachingTip,
        [&]()
        {
            callbackRan = true;
            if ( wxStyledTextCtrl * const live = stcLifetime.get() )
                destroyAccepted = live->Destroy();
        });
    REQUIRE(nested);

    // Do not inspect the raw STC after this synchronous native boundary: the
    // nested callback is allowed to schedule or complete its destruction.
    (void)stc->Reparent(fixture.GetPanelB());
    CHECK(callbackRan);
    CHECK(destroyAccepted);
    CHECK_FALSE(static_cast<bool>(nested));
    REQUIRE(DrainUntil([&stcLifetime]() { return !stcLifetime; }, 250));

    fixture.Cleanup();
    REQUIRE(DrainUntil([&before]() { return before.IsRestored(); }, 250));
    before.CheckRestored();
}

TEST_CASE("WinUISTC::PopupDestroyIsIdempotentAndOwnerSafe",
          "[winui-stc][popup][destroy][owner]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();

    STCFixture fixture;
    REQUIRE(fixture.CreateShells(13));
    wxStyledTextCtrl * const stc = fixture.CreateStyledText();
    REQUIRE(stc);
    wxFrame * const owner = fixture.GetFrameA();
    REQUIRE(owner);

    stc->AutoCompShow(0, "double-destroy");
    REQUIRE(stc->AutoCompActive());
    wxWindow * const list = FindDescendantNamed(stc, "AutoCompListBox");
    REQUIRE(list);
    wxWindow * const listPopup = GetPopupRoot(stc, list);
    REQUIRE(listPopup);
    const wxWeakRef<wxWindow> listPopupLifetime(listPopup);

    // STC historically treats repeated delayed Destroy() calls as one
    // request. Both calls must enter/converge on the common terminal popup
    // transaction without creating a second pending-delete generation.
    REQUIRE(listPopup->Destroy());
    CHECK(listPopup->Destroy());
    stc->AutoCompCancel();
    CHECK_FALSE(stc->AutoCompActive());
    REQUIRE(DrainUntil(
        [&listPopupLifetime]() { return !listPopupLifetime; }, 200));
    CHECK(wxWinUITransientCountForTesting(
              owner, wxWinUITransientKind::Popup) == 0);

    stc->CallTipShow(0, "owner teardown");
    REQUIRE(stc->CallTipActive());
    wxWindow * const callTip = FindDescendantNamed(stc, "wxSTCCallTip");
    REQUIRE(callTip);
    const wxWeakRef<wxWindow> callTipLifetime(callTip);
    const wxWeakRef<wxStyledTextCtrl> stcLifetime(stc);
    const wxWeakRef<wxFrame> ownerLifetime(owner);

    REQUIRE(owner->Destroy());
    if ( wxStyledTextCtrl * const liveSTC = stcLifetime.get() )
    {
        // The direct parent is the STC, but the scheduled top-level owner is
        // already terminal. This must choose immediate owner-safe teardown,
        // not leave the child in wxPendingDelete behind the dying hierarchy.
        // Cancel through Scintilla so its platform Window clears the native
        // ID before the STC itself reaches Finalise().
        liveSTC->CallTipCancel();
        CHECK_FALSE(liveSTC->CallTipActive());
        CHECK_FALSE(callTipLifetime);
    }

    REQUIRE(DrainUntil(
        [&ownerLifetime, &stcLifetime, &callTipLifetime]()
        {
            return !ownerLifetime && !stcLifetime && !callTipLifetime;
        },
        250));

    fixture.Cleanup();
    REQUIRE(DrainUntil([&before]() { return before.IsRestored(); }, 200));
    before.CheckRestored();
}

TEST_CASE("WinUISTC::OldTLWCanDieBeforeNextPopupGeneration",
          "[winui-stc][reparent][popup][destroy]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();

    STCFixture fixture;
    REQUIRE(fixture.CreateShells(11));
    wxStyledTextCtrl * const stc = fixture.CreateStyledText();
    REQUIRE(stc);
    wxFrame * const oldFrame = fixture.GetFrameA();
    wxFrame * const newFrame = fixture.GetFrameB();
    REQUIRE(oldFrame);
    REQUIRE(newFrame);
    const wxWeakRef<wxFrame> oldFrameLifetime(oldFrame);

    stc->SetText("old-owner-lifetime");
    stc->CallTipShow(0, "owned by A");
    REQUIRE(stc->CallTipActive());
    REQUIRE(wxWinUITransientCountForTesting(
                oldFrame, wxWinUITransientKind::Popup) == 1);

    REQUIRE(stc->Reparent(fixture.GetPanelB()));
    CHECK_FALSE(stc->CallTipActive());
    REQUIRE(DrainUntil(
        [oldFrame, newFrame]()
        {
            return wxWinUITransientCountForTesting(
                       oldFrame, wxWinUITransientKind::Popup) == 0 &&
                   wxWinUITransientCountForTesting(
                       newFrame, wxWinUITransientKind::Popup) == 0;
        }));

    // The popup's move subscription and Scintilla's remembered TLW are weak.
    // Destroy A before the pending popup object necessarily reaches its
    // destructor, then open and retire the next generation under B.
    REQUIRE(oldFrame->Destroy());
    REQUIRE(DrainUntil(
        [&oldFrameLifetime]() { return !oldFrameLifetime; }, 200));
    CHECK(fixture.GetFrameA() == nullptr);

    stc->AutoCompShow(0, "safe-after-old-owner-destroy");
    REQUIRE(stc->AutoCompActive());
    CHECK(wxWinUITransientCountForTesting(
              newFrame, wxWinUITransientKind::Popup) == 1);
    wxWindow * const list = FindDescendantNamed(stc, "AutoCompListBox");
    REQUIRE(list);
    wxWindow * const popup = GetPopupRoot(stc, list);
    REQUIRE(popup);
    CHECK(::GetWindow(static_cast<HWND>(popup->GetHWND()), GW_OWNER) ==
          static_cast<HWND>(newFrame->GetHWND()));
    stc->AutoCompCancel();
    REQUIRE(DrainUntil(
        [newFrame]()
        {
            return wxWinUITransientCountForTesting(
                       newFrame, wxWinUITransientKind::Popup) == 0;
        }));

    fixture.Cleanup();
    REQUIRE(DrainUntil([&before]() { return before.IsRestored(); }, 200));
    before.CheckRestored();
}

TEST_CASE("WinUISTC::DestroyFromCallTipCallbackRetiresPopupAndArmedTimer",
          "[winui-stc][destroy][callback][timer]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();

    STCFixture fixture;
    REQUIRE(fixture.CreateShells(12));
    wxStyledTextCtrl * const stc = fixture.CreateStyledText();
    REQUIRE(stc);
    wxFrame * const frame = fixture.GetFrameA();
    const wxWeakRef<wxStyledTextCtrl> weakSTC(stc);

    stc->SetCaretPeriod(5);
    stc->SetSTCFocus(true);
    stc->CallTipShow(0, "destroy from callback");
    REQUIRE(stc->CallTipActive());
    REQUIRE(wxWinUITransientCountForTesting(
                frame, wxWinUITransientKind::Popup) == 1);
    wxWindow * const callTip = FindDescendantNamed(stc, "wxSTCCallTip");
    REQUIRE(callTip);

    bool callbackRan = false;
    bool destroyAccepted = false;
    stc->Bind(
        wxEVT_STC_CALLTIP_CLICK,
        [&](wxStyledTextEvent&)
        {
            callbackRan = true;
            destroyAccepted = stc->Destroy();
        });
    wxMouseEvent click(wxEVT_LEFT_DOWN);
    click.SetEventObject(callTip);
    click.SetPosition(wxPoint(2, 2));
    REQUIRE(callTip->GetEventHandler()->ProcessEvent(click));
    CHECK(callbackRan);
    CHECK(destroyAccepted);

    REQUIRE(DrainUntil([&weakSTC]() { return !weakSTC; }, 200));
    CHECK(wxWinUITransientCountForTesting(
              frame, wxWinUITransientKind::Popup) == 0);

    fixture.Cleanup();
    REQUIRE(DrainUntil([&before]() { return before.IsRestored(); }, 200));
    before.CheckRestored();
}

TEST_CASE("WinUISTC::NativeCaretTimerIdentityIsKilledOnImmediateDestroy",
          "[winui-stc][destroy][timer]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();

    STCFixture fixture;
    REQUIRE(fixture.CreateShells(14));
    wxStyledTextCtrl * const stc = fixture.CreateStyledText();
    REQUIRE(stc);
    const wxWeakRef<wxStyledTextCtrl> weakSTC(stc);

    // USER32 clamps shorter SetTimer() intervals to USER_TIMER_MINIMUM.
    // Request the effective 10 ms minimum so the observation window below is
    // exactly ten former native periods rather than ten nominal wx periods.
    stc->SetCaretPeriod(10);
    stc->SetSTCFocus(true);
    const wxUIntPtr caretTimerId = wxSTCGetCaretTimerIdForTesting(stc);
    REQUIRE(caretTimerId > 0);
    NativeTimerIdentity timer;
    REQUIRE(DispatchUntilWxTimer(
        &timer, static_cast<UINT_PTR>(caretTimerId)));
    REQUIRE(timer.hwnd);
    REQUIRE(timer.id == static_cast<UINT_PTR>(caretTimerId));
    CHECK(timer.deliveries == 1);

    // Keep the exact hidden timer HWND/id as the snapshot. Deleting STC must
    // synchronously destroy all four wxSTCTimer objects and KillTimer() this
    // active caret identity before any later dispatch can reach ScintillaWX.
    delete stc;
    REQUIRE_FALSE(weakSTC);

    // KillTimer() deliberately does not remove a WM_TIMER that USER32 had
    // already queued. Dispatch such a message once: wx's retired timer map
    // must ignore it safely. Then observe ten full former periods and require
    // that USER32 produces no new message for this exact HWND/id generation.
    const unsigned alreadyQueued = DispatchAndCountTimer(timer, 0);
    INFO("retired timer messages already queued: " << alreadyQueued);
    CHECK(DispatchAndCountTimer(timer, 100) == 0);

    fixture.Cleanup();
    REQUIRE(DrainUntil([&before]() { return before.IsRestored(); }, 200));
    before.CheckRestored();
}

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_STC && wxUSE_POPUPWIN
