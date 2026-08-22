///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuimdi.cpp
// Purpose:     deterministic WinUI/native-MDI topology and lifecycle spike
// Author:      wxWidgets development team
// Created:     2026-08-08
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_MDI && wxUSE_BUTTON

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/button.h"
    #include "wx/frame.h"
    #include "wx/menu.h"
    #include "wx/panel.h"
    #include "wx/sizer.h"
    #include "wx/utils.h"
#endif // WX_PRECOMP

#include "wx/mdi.h"
#include "wx/msw/wrapwin.h"
#include "wx/weakref.h"
#include "wx/winui/private/dialogsession.h"
#include "wx/winui/private/tlwhost.h"
#include "wx/winui/private/tlwhostmsw.h"

#include <memory>
#include <vector>

namespace
{

enum
{
    ID_MDI_PARENT_COMMAND = wxID_HIGHEST + 7800,
    ID_MDI_CHILD_ONE_COMMAND,
    ID_MDI_CHILD_TWO_COMMAND
};

void DrainDispatch(unsigned rounds = 20)
{
    for ( unsigned i = 0; i < rounds; ++i )
    {
        wxYield();
        wxMilliSleep(2);
    }
}

template <typename Predicate>
bool DrainUntil(Predicate predicate, unsigned rounds = 180)
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
        const unsigned callbacks =
            wxWinUITopLevelHost::GetFlushCallbackAttemptCount();
        DrainDispatch(2);
        if ( schedules == wxWinUITopLevelHost::GetFlushScheduleCount() &&
             runs == wxWinUITopLevelHost::GetFlushRunCount() &&
             callbacks ==
                 wxWinUITopLevelHost::GetFlushCallbackAttemptCount() )
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
               wxWinUITopLevelHost::GetLiveLoadedHookCount() == loadedHooks &&
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
    unsigned pendingFocusMigrations = 0;
    unsigned slotHandlerAdds = 0;
    unsigned slotHandlerRevokes = 0;
    unsigned rootHandlerAdds = 0;
    unsigned rootHandlerRevokes = 0;
    wxWinUITransientRegistrySnapshot transients;
    wxWindow *wxCapture = nullptr;
    HWND nativeCapture = nullptr;
};

class MdiParentProbe final : public wxMDIParentFrame
{
public:
#if wxUSE_MENUS
    wxWindow *GetProjectedMenuBarForTesting() const
    {
        return m_winuiMenuBarWin;
    }
#endif // wxUSE_MENUS
};

class MdiChildProbe final : public wxMDIChildFrame
{
public:
#if wxUSE_MENUS
    wxWindow *GetProjectedMenuBarForTesting() const
    {
        return m_winuiMenuBarWin;
    }
#endif // wxUSE_MENUS
};

bool ShowOffscreenWithoutActivation(wxFrame *frame, unsigned ordinal)
{
    const HWND hwnd = static_cast<HWND>(frame->GetHWND());
    if ( !hwnd )
        return false;

    ::SetLastError(ERROR_SUCCESS);
    const LONG_PTR oldStyle = ::GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    if ( !oldStyle && ::GetLastError() != ERROR_SUCCESS )
        return false;

    const LONG_PTR requiredStyle = WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW;
    const LONG_PTR requestedStyle = oldStyle | requiredStyle;
    if ( requestedStyle != oldStyle )
    {
        ::SetLastError(ERROR_SUCCESS);
        const LONG_PTR previous =
            ::SetWindowLongPtr(hwnd, GWL_EXSTYLE, requestedStyle);
        if ( !previous && ::GetLastError() != ERROR_SUCCESS )
            return false;
    }

    ::SetLastError(ERROR_SUCCESS);
    const LONG_PTR observedStyle = ::GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    if ( (!observedStyle && ::GetLastError() != ERROR_SUCCESS) ||
         (observedStyle & requiredStyle) != requiredStyle )
    {
        return false;
    }

    const int desktopLeft = ::GetSystemMetrics(SM_XVIRTUALSCREEN);
    const int desktopTop = ::GetSystemMetrics(SM_YVIRTUALSCREEN);
    const int desktopWidth = ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
    const int desktopHeight = ::GetSystemMetrics(SM_CYVIRTUALSCREEN);
    if ( desktopWidth <= 0 || desktopHeight <= 0 )
        return false;

    RECT desktop = {
        desktopLeft, desktopTop,
        desktopLeft + desktopWidth, desktopTop + desktopHeight
    };
    const int relativeX = desktopLeft - 6000 -
                          static_cast<int>(ordinal) * 400;
    const int relativeY = desktopTop - 6000 -
                          static_cast<int>(ordinal) * 240;
    const int fixedX = -30000 - static_cast<int>(ordinal) * 400;
    const int fixedY = -30000 - static_cast<int>(ordinal) * 240;
    const int x = relativeX < fixedX ? relativeX : fixedX;
    const int y = relativeY < fixedY ? relativeY : fixedY;
    frame->Move(wxPoint(x, y));

    const auto isOutsideDesktop = [hwnd, &desktop]()
    {
        RECT window = {};
        RECT intersection = {};
        return ::GetWindowRect(hwnd, &window) &&
               !::IntersectRect(&intersection, &window, &desktop);
    };
    if ( !isOutsideDesktop() )
    {
        frame->Hide();
        return false;
    }

    frame->ShowWithoutActivating();
    if ( !::IsWindowVisible(hwnd) || !isOutsideDesktop() )
    {
        frame->Hide();
        return false;
    }

    return true;
}

class NativeFocusRestoreGuard final
{
public:
    NativeFocusRestoreGuard()
        : m_hwnd(::GetFocus()),
          m_generation(m_hwnd
              ? wxWinUIMSWGetNativeHwndGeneration(
                    reinterpret_cast<WXHWND>(m_hwnd))
              : 0)
    {
    }

    ~NativeFocusRestoreGuard()
    {
        HWND target = nullptr;
        if ( m_hwnd && m_generation &&
             wxWinUIMSWGetNativeHwndGeneration(
                 reinterpret_cast<WXHWND>(m_hwnd)) == m_generation )
        {
            target = m_hwnd;
        }
        ::SetFocus(target);
    }

private:
    HWND m_hwnd;
    unsigned long long m_generation;

    wxDECLARE_NO_COPY_CLASS(NativeFocusRestoreGuard);
};

class MdiEnvironment final
{
public:
    ~MdiEnvironment()
    {
        Cleanup();
        DrainDispatch();
    }

    MdiParentProbe *CreateParent(const wxString& title,
                                 unsigned ordinal,
                                 bool withWindowMenu = false)
    {
        MdiParentProbe * const parent = new MdiParentProbe;
        long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL |
                     wxFRAME_NO_TASKBAR | wxFRAME_TOOL_WINDOW;
        if ( !withWindowMenu )
            style |= wxFRAME_NO_WINDOW_MENU;

        if ( !parent->Create(
                 nullptr, wxID_ANY, title, wxDefaultPosition,
                 wxSize(980, 700), style) ||
             !ShowOffscreenWithoutActivation(parent, ordinal) )
        {
            delete parent;
            return nullptr;
        }

        m_parents.emplace_back(parent);
        return parent;
    }

    bool DestroyAndWait(const RuntimeSnapshot& before)
    {
        Cleanup();
        return DrainUntil(
                   [this, &before]()
                   {
                       for ( const wxWeakRef<MdiParentProbe>& parent :
                             m_parents )
                       {
                           if ( parent )
                               return false;
                       }
                       return before.IsRestored();
                   },
                   500) &&
               DrainToQuiescence() && before.IsRestored();
    }

private:
    void Cleanup()
    {
        if ( m_cleaned )
            return;
        m_cleaned = true;

        for ( auto it = m_parents.rbegin(); it != m_parents.rend(); ++it )
        {
            if ( MdiParentProbe * const parent = it->get() )
            {
                if ( !parent->IsBeingDeleted() )
                    parent->Destroy();
            }
        }
    }

    bool m_cleaned = false;
    std::vector<wxWeakRef<MdiParentProbe>> m_parents;
};

struct MdiChildSurface
{
    MdiChildProbe *child = nullptr;
    wxPanel *panel = nullptr;
    wxButton *button = nullptr;
};

MdiChildSurface CreateChildSurface(MdiParentProbe *parent,
                                   const wxString& title,
                                   const wxRect& rect,
                                   bool withButton = true)
{
    MdiChildSurface surface;
    if ( !parent )
        return surface;

    surface.child = new MdiChildProbe;
    if ( !surface.child->Create(
             parent, wxID_ANY, title, rect.GetPosition(), rect.GetSize()) )
    {
        delete surface.child;
        surface.child = nullptr;
        return surface;
    }

    surface.panel = new wxPanel(surface.child, wxID_ANY);
    wxBoxSizer * const childSizer = new wxBoxSizer(wxVERTICAL);
    childSizer->Add(surface.panel, 1, wxEXPAND);
    surface.child->SetSizer(childSizer);

    if ( withButton )
    {
        surface.button = new wxButton(
            surface.panel, wxID_ANY, title + " XAML sentinel");
        wxBoxSizer * const panelSizer = new wxBoxSizer(wxVERTICAL);
        panelSizer->Add(surface.button, 1, wxEXPAND | wxALL, 6);
        surface.panel->SetSizer(panelSizer);
    }

    surface.child->SetSize(rect);
    surface.child->Show();
    surface.child->Layout();
    surface.panel->Layout();
    return surface;
}

bool HasSingleHostOwner(wxWindow *window,
                        wxWinUITopLevelHost *expectedHost)
{
    return window && expectedHost &&
           wxWinUITopLevelHost::FindSlotOwner(window) == expectedHost &&
           expectedHost->FindSlot(window);
}

bool IsNativeChildOf(HWND child, HWND parent)
{
    return child && parent && ::GetParent(child) == parent;
}

bool RectanglesOverlap(HWND lhs, HWND rhs)
{
    RECT lhsRect = {};
    RECT rhsRect = {};
    RECT intersection = {};
    return lhs && rhs && ::GetWindowRect(lhs, &lhsRect) &&
           ::GetWindowRect(rhs, &rhsRect) &&
           ::IntersectRect(&intersection, &lhsRect, &rhsRect) &&
           intersection.right > intersection.left &&
           intersection.bottom > intersection.top;
}

bool GetWindowRectInClient(HWND hwnd, HWND client, RECT *rect)
{
    if ( !hwnd || !client || !rect || !::GetWindowRect(hwnd, rect) )
        return false;

    POINT points[2] = {
        { rect->left, rect->top },
        { rect->right, rect->bottom }
    };
    ::SetLastError(ERROR_SUCCESS);
    const int mapped = ::MapWindowPoints(HWND_DESKTOP, client, points, 2);
    if ( !mapped && ::GetLastError() != ERROR_SUCCESS )
        return false;

    rect->left = points[0].x;
    rect->top = points[0].y;
    rect->right = points[1].x;
    rect->bottom = points[1].y;
    return rect->right > rect->left && rect->bottom > rect->top;
}

bool RectanglesOverlap(const RECT& lhs, const RECT& rhs)
{
    RECT intersection = {};
    return ::IntersectRect(&intersection, &lhs, &rhs) &&
           intersection.right > intersection.left &&
           intersection.bottom > intersection.top;
}

bool RectanglesDiffer(const RECT& lhs, const RECT& rhs)
{
    return lhs.left != rhs.left || lhs.top != rhs.top ||
           lhs.right != rhs.right || lhs.bottom != rhs.bottom;
}

long Distance(long lhs, long rhs)
{
    return lhs > rhs ? lhs - rhs : rhs - lhs;
}

long long RectangleArea(const RECT& rect)
{
    return static_cast<long long>(rect.right - rect.left) *
           static_cast<long long>(rect.bottom - rect.top);
}

class ImpossibleZOrderResetGuard final
{
public:
    ImpossibleZOrderResetGuard()
    {
        wxWinUITopLevelHost::ResetImpossibleZOrderCountForTest();
    }

    ~ImpossibleZOrderResetGuard()
    {
        wxWinUITopLevelHost::ResetImpossibleZOrderCountForTest();
    }
};

} // anonymous namespace

TEST_CASE("WinUI native MDI routes every child slot through the parent host",
          "[winui-mdi][winui-015][winui][mdi][advanced][topology]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();
    ImpossibleZOrderResetGuard impossibleZOrderReset;
    NativeFocusRestoreGuard focusRestore;

    MdiEnvironment environment;
    MdiParentProbe * const parent =
        environment.CreateParent("WinUI native MDI topology", 0);
    REQUIRE(parent);
    REQUIRE(parent->GetClientWindow());

    MdiChildSurface first = CreateChildSurface(
        parent, "First document", wxRect(20, 30, 380, 280));
    MdiChildSurface second = CreateChildSurface(
        parent, "Second document", wxRect(470, 30, 380, 280));
    REQUIRE(first.child);
    REQUIRE(first.panel);
    REQUIRE(first.button);
    REQUIRE(second.child);
    REQUIRE(second.panel);
    REQUIRE(second.button);

    REQUIRE(DrainUntil(
        [first, second, parent]()
        {
            wxWinUITopLevelHost * const host =
                wxWinUITopLevelHost::FindForTLW(parent);
            return HasSingleHostOwner(first.button, host) &&
                   HasSingleHostOwner(second.button, host);
        }));

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindForTLW(parent);
    REQUIRE(host);
    host->FlushSync();

    const HWND parentHwnd = static_cast<HWND>(parent->GetHWND());
    const HWND clientHwnd =
        static_cast<HWND>(parent->GetClientWindow()->GetHWND());
    const HWND firstHwnd = static_cast<HWND>(first.child->GetHWND());
    const HWND secondHwnd = static_cast<HWND>(second.child->GetHWND());
    const HWND bridgeHwnd = host->GetBridgeHwnd();
    REQUIRE(parentHwnd);
    REQUIRE(clientHwnd);
    REQUIRE(firstHwnd);
    REQUIRE(secondHwnd);
    REQUIRE(bridgeHwnd);

    wchar_t clientClass[32] = L"";
    REQUIRE(::GetClassNameW(clientHwnd, clientClass, WXSIZEOF(clientClass)) > 0);
    CHECK(wxString(clientClass).CmpNoCase("MDICLIENT") == 0);
    CHECK(IsNativeChildOf(clientHwnd, parentHwnd));
    CHECK(IsNativeChildOf(firstHwnd, clientHwnd));
    CHECK(IsNativeChildOf(secondHwnd, clientHwnd));
    CHECK(IsNativeChildOf(bridgeHwnd, parentHwnd));
    CHECK_FALSE(::IsChild(clientHwnd, bridgeHwnd));
    CHECK_FALSE(::IsChild(firstHwnd, bridgeHwnd));

    CHECK_FALSE(first.child->IsTopLevel());
    CHECK_FALSE(second.child->IsTopLevel());
    CHECK(wxGetTopLevelParent(first.child) == parent);
    CHECK(wxGetTopLevelParent(first.button) == parent);
    CHECK(wxGetTopLevelParent(second.button) == parent);
    CHECK(wxWinUITopLevelHost::ForWindow(first.child, false) == host);
    CHECK(wxWinUITopLevelHost::ForWindow(second.child, false) == host);
    CHECK(wxWinUITopLevelHost::FindForTLW(first.child) == nullptr);
    CHECK(wxWinUITopLevelHost::FindForTLW(second.child) == nullptr);
    CHECK(wxWinUITopLevelHost::GetLiveHostCount() == before.hosts + 1);

    unsigned firstActivations = 0;
    unsigned secondActivations = 0;
    first.child->Bind(
        wxEVT_ACTIVATE,
        [&firstActivations](wxActivateEvent& event)
        {
            if ( event.GetActive() )
                ++firstActivations;
            event.Skip();
        });
    second.child->Bind(
        wxEVT_ACTIVATE,
        [&secondActivations](wxActivateEvent& event)
        {
            if ( event.GetActive() )
                ++secondActivations;
            event.Skip();
        });

    first.child->Activate();
    REQUIRE(DrainUntil([parent, first]()
                       { return parent->GetActiveChild() == first.child; }));
    parent->ActivateNext();
    REQUIRE(DrainUntil([parent, second]()
                       { return parent->GetActiveChild() == second.child; }));
    parent->ActivatePrevious();
    REQUIRE(DrainUntil([parent, first]()
                       { return parent->GetActiveChild() == first.child; }));
    CHECK(firstActivations > 0);
    CHECK(secondActivations > 0);

    RECT initialFirst = {};
    RECT initialSecond = {};
    REQUIRE(GetWindowRectInClient(firstHwnd, clientHwnd, &initialFirst));
    REQUIRE(GetWindowRectInClient(secondHwnd, clientHwnd, &initialSecond));

    parent->Cascade();
    REQUIRE(DrainUntil(
        [firstHwnd, secondHwnd, clientHwnd,
         initialFirst, initialSecond]()
        {
            RECT cascadedFirst = {};
            RECT cascadedSecond = {};
            return GetWindowRectInClient(
                       firstHwnd, clientHwnd, &cascadedFirst) &&
                   GetWindowRectInClient(
                       secondHwnd, clientHwnd, &cascadedSecond) &&
                   RectanglesDiffer(initialFirst, cascadedFirst) &&
                   RectanglesDiffer(initialSecond, cascadedSecond) &&
                   RectanglesOverlap(cascadedFirst, cascadedSecond);
        }));

    RECT cascadedFirst = {};
    RECT cascadedSecond = {};
    REQUIRE(GetWindowRectInClient(firstHwnd, clientHwnd, &cascadedFirst));
    REQUIRE(GetWindowRectInClient(secondHwnd, clientHwnd, &cascadedSecond));
    CHECK(RectanglesDiffer(initialFirst, cascadedFirst));
    CHECK(RectanglesDiffer(initialSecond, cascadedSecond));
    CHECK(RectanglesOverlap(cascadedFirst, cascadedSecond));

    parent->Tile(wxVERTICAL);
    REQUIRE(DrainUntil(
        [firstHwnd, secondHwnd, clientHwnd,
         cascadedFirst, cascadedSecond]()
        {
            RECT tiledFirst = {};
            RECT tiledSecond = {};
            return GetWindowRectInClient(
                       firstHwnd, clientHwnd, &tiledFirst) &&
                   GetWindowRectInClient(
                       secondHwnd, clientHwnd, &tiledSecond) &&
                   RectanglesDiffer(cascadedFirst, tiledFirst) &&
                   RectanglesDiffer(cascadedSecond, tiledSecond) &&
                   !RectanglesOverlap(tiledFirst, tiledSecond);
        }));

    RECT tiledFirst = {};
    RECT tiledSecond = {};
    RECT clientRect = {};
    RECT tiledUnion = {};
    REQUIRE(GetWindowRectInClient(firstHwnd, clientHwnd, &tiledFirst));
    REQUIRE(GetWindowRectInClient(secondHwnd, clientHwnd, &tiledSecond));
    REQUIRE(::GetClientRect(clientHwnd, &clientRect));
    REQUIRE(::UnionRect(&tiledUnion, &tiledFirst, &tiledSecond));
    CHECK_FALSE(RectanglesOverlap(tiledFirst, tiledSecond));
    CHECK(Distance(tiledUnion.left, clientRect.left) <= 2);
    CHECK(Distance(tiledUnion.top, clientRect.top) <= 2);
    CHECK(Distance(tiledUnion.right, clientRect.right) <= 2);
    CHECK(Distance(tiledUnion.bottom, clientRect.bottom) <= 2);
    CHECK(RectangleArea(tiledFirst) + RectangleArea(tiledSecond) >=
          RectangleArea(clientRect) * 95 / 100);

    first.child->Maximize();
    REQUIRE(DrainUntil([first]() { return first.child->IsMaximized(); }));
    first.child->Restore();
    REQUIRE(DrainUntil([first]() { return !first.child->IsMaximized(); }));
    first.child->Iconize();
    REQUIRE(DrainUntil([first]() { return first.child->IsIconized(); }));
    parent->ArrangeIcons();
    first.child->Restore();
    REQUIRE(DrainUntil([first]() { return !first.child->IsIconized(); }));

    first.child->Activate();
    first.button->SetFocus();
    REQUIRE(DrainUntil([first]()
                       { return wxWindow::FindFocus() == first.button; }));
    second.child->Activate();
    second.button->SetFocus();
    REQUIRE(DrainUntil([second]()
                       { return wxWindow::FindFocus() == second.button; }));

    REQUIRE(environment.DestroyAndWait(before));
    before.CheckRestored();
}

TEST_CASE("WinUI native MDI records the single-bridge overlap STOP",
          "[winui-mdi][winui-mdi-stop][winui-015][winui][mdi][advanced][topology][zorder]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();
    ImpossibleZOrderResetGuard impossibleZOrderReset;
    NativeFocusRestoreGuard focusRestore;

    MdiEnvironment environment;
    MdiParentProbe * const parent =
        environment.CreateParent("WinUI native MDI overlap witness", 0);
    REQUIRE(parent);

    const wxRect overlapRect(70, 70, 520, 390);
    MdiChildSurface lower = CreateChildSurface(
        parent, "Lower document", overlapRect);
    MdiChildSurface upper = CreateChildSurface(
        parent, "Upper document", overlapRect);
    REQUIRE(lower.child);
    REQUIRE(lower.button);
    REQUIRE(upper.child);
    REQUIRE(upper.button);

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(lower.button, true);
    REQUIRE(host);
    REQUIRE(DrainUntil(
        [host, lower, upper]()
        {
            return HasSingleHostOwner(lower.button, host) &&
                   HasSingleHostOwner(upper.button, host);
        }));

    const HWND clientHwnd =
        static_cast<HWND>(parent->GetClientWindow()->GetHWND());
    const HWND lowerHwnd = static_cast<HWND>(lower.child->GetHWND());
    const HWND upperHwnd = static_cast<HWND>(upper.child->GetHWND());
    const HWND lowerButtonHwnd = static_cast<HWND>(lower.button->GetHWND());
    REQUIRE(clientHwnd);
    REQUIRE(lowerHwnd);
    REQUIRE(upperHwnd);
    REQUIRE(lowerButtonHwnd);

    REQUIRE(::SetWindowPos(
        lowerHwnd, HWND_BOTTOM, 70, 70, 520, 390,
        SWP_NOACTIVATE | SWP_SHOWWINDOW));
    REQUIRE(::SetWindowPos(
        upperHwnd, HWND_TOP, 70, 70, 520, 390,
        SWP_NOACTIVATE | SWP_SHOWWINDOW));
    REQUIRE(::GetWindow(clientHwnd, GW_CHILD) == upperHwnd);
    REQUIRE(RectanglesOverlap(lowerButtonHwnd, upperHwnd));

    // Explicit SetWindowPos() is a native boundary: publish it through the
    // same production notification used by WM_WINDOWPOSCHANGED. Reset only
    // after this point so creation/initial layout cannot satisfy the oracle.
    wxWinUITLWHostNotifyNativeLayout(upper.child, true);
    wxWinUITopLevelHost::ResetImpossibleZOrderCountForTest();

    // The lower child's XAML button must be below the upper child's native
    // non-client/client surface, while the upper child's own XAML button must
    // remain above that same native band. A single globally-topmost island
    // cannot express this interleave. The shared host deliberately records
    // the unsupported relation instead of manufacturing a second island.
    host->FlushSync();
    const unsigned impossible =
        wxWinUITopLevelHost::GetImpossibleZOrderCount();
    INFO("native-MDI overlap diagnostics: " << impossible);
    CHECK(impossible == 1);
    CHECK(wxWinUITopLevelHost::GetLiveHostCount() == before.hosts + 1);
    CHECK(wxWinUITopLevelHost::FindForTLW(lower.child) == nullptr);
    CHECK(wxWinUITopLevelHost::FindForTLW(upper.child) == nullptr);

    REQUIRE(environment.DestroyAndWait(before));
    before.CheckRestored();
}

#if wxUSE_MENUS && wxUSE_ACCEL
TEST_CASE("WinUI native MDI switches one effective menu and accelerators",
          "[winui-mdi][winui-015][winui][mdi][advanced][menu][accel]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();
    ImpossibleZOrderResetGuard impossibleZOrderReset;
    NativeFocusRestoreGuard focusRestore;

    MdiEnvironment environment;
    MdiParentProbe * const parent =
        environment.CreateParent("WinUI native MDI menu", 0, true);
    REQUIRE(parent);

    wxMenuBar * const parentBar = new wxMenuBar;
    wxMenu * const parentMenu = new wxMenu;
    parentMenu->Append(ID_MDI_PARENT_COMMAND, "Parent command\tF5");
    parentBar->Append(parentMenu, "&Parent");
    parent->SetMenuBar(parentBar);

    unsigned parentCommands = 0;
    parent->Bind(
        wxEVT_MENU,
        [&parentCommands](wxCommandEvent&) { ++parentCommands; },
        ID_MDI_PARENT_COMMAND);

    MdiChildSurface first = CreateChildSurface(
        parent, "Menu document one", wxRect(40, 40, 380, 280));
    MdiChildSurface second = CreateChildSurface(
        parent, "Menu document two", wxRect(450, 40, 380, 280));
    REQUIRE(first.child);
    REQUIRE(first.button);
    REQUIRE(second.child);
    REQUIRE(second.button);

    wxMenuBar * const firstBar = new wxMenuBar;
    wxMenu * const firstMenu = new wxMenu;
    firstMenu->Append(ID_MDI_CHILD_ONE_COMMAND, "First command\tF6");
    firstBar->Append(firstMenu, "&First");
    first.child->SetMenuBar(firstBar);

    wxMenuBar * const secondBar = new wxMenuBar;
    wxMenu * const secondMenu = new wxMenu;
    secondMenu->Append(ID_MDI_CHILD_TWO_COMMAND, "Second command\tF7");
    secondBar->Append(secondMenu, "&Second");
    second.child->SetMenuBar(secondBar);

    unsigned firstCommands = 0;
    unsigned secondCommands = 0;
    first.child->Bind(
        wxEVT_MENU,
        [&firstCommands](wxCommandEvent&) { ++firstCommands; },
        ID_MDI_CHILD_ONE_COMMAND);
    second.child->Bind(
        wxEVT_MENU,
        [&secondCommands](wxCommandEvent&) { ++secondCommands; },
        ID_MDI_CHILD_TWO_COMMAND);

    first.child->Activate();
    REQUIRE(DrainUntil([parent, first]()
                       { return parent->GetActiveChild() == first.child; }));
    CHECK(parent->MSWGetActiveMenu() == firstBar->GetHMenu());

    const HWND parentHwnd = static_cast<HWND>(parent->GetHWND());
    REQUIRE(parentHwnd);
    const bool nativeMenu = ::GetMenu(parentHwnd) != nullptr;
    const bool projectedMenu =
        parent->GetProjectedMenuBarForTesting() != nullptr;
    INFO("native MDI menu surface: native=" << nativeMenu
         << ", projected=" << projectedMenu);
    REQUIRE(nativeMenu != projectedMenu);
    CHECK(first.child->GetProjectedMenuBarForTesting() == nullptr);
    CHECK(second.child->GetProjectedMenuBarForTesting() == nullptr);
    if ( nativeMenu )
        CHECK(::GetMenu(parentHwnd) == firstBar->GetHMenu());
    if ( projectedMenu )
    {
        // A future XAML projection must remain a single parent-owned surface,
        // never one island/menu-bar window per MDI child.
        CHECK(::GetMenu(parentHwnd) == nullptr);
    }

    MSG firstKey = {};
    firstKey.hwnd = static_cast<HWND>(first.button->GetHWND());
    firstKey.message = WM_KEYDOWN;
    firstKey.wParam = VK_F6;
    REQUIRE(parent->MSWTranslateMessage(
        reinterpret_cast<WXMSG *>(&firstKey)));
    CHECK(firstCommands == 1);
    CHECK(secondCommands == 0);

    second.child->Activate();
    REQUIRE(DrainUntil([parent, second]()
                       { return parent->GetActiveChild() == second.child; }));
    CHECK(parent->MSWGetActiveMenu() == secondBar->GetHMenu());
    if ( nativeMenu )
        CHECK(::GetMenu(parentHwnd) == secondBar->GetHMenu());

    MSG secondKey = {};
    secondKey.hwnd = static_cast<HWND>(second.button->GetHWND());
    secondKey.message = WM_KEYDOWN;
    secondKey.wParam = VK_F7;
    REQUIRE(parent->MSWTranslateMessage(
        reinterpret_cast<WXMSG *>(&secondKey)));
    CHECK(firstCommands == 1);
    CHECK(secondCommands == 1);

    wxCommandEvent routed(wxEVT_MENU, ID_MDI_CHILD_TWO_COMMAND);
    routed.SetEventObject(parent);
    parent->ProcessWindowEvent(routed);
    CHECK(secondCommands == 2);

    MSG parentKey = {};
    parentKey.hwnd = static_cast<HWND>(second.button->GetHWND());
    parentKey.message = WM_KEYDOWN;
    parentKey.wParam = VK_F5;
    REQUIRE(parent->MSWTranslateMessage(
        reinterpret_cast<WXMSG *>(&parentKey)));
    CHECK(parentCommands == 1);
    CHECK(firstCommands == 1);
    CHECK(secondCommands == 2);

    REQUIRE(environment.DestroyAndWait(before));
    before.CheckRestored();
}
#endif // wxUSE_MENUS && wxUSE_ACCEL

#if wxUSE_MENUS
TEST_CASE("WinUI native MDI accepts a menu bar installed after its first child",
          "[winui-mdi][winui-015][winui][mdi][advanced][menu][late-menubar][lifetime]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();
    ImpossibleZOrderResetGuard impossibleZOrderReset;
    NativeFocusRestoreGuard focusRestore;

    MdiEnvironment environment;
    MdiParentProbe * const parent =
        environment.CreateParent("WinUI native MDI late menu", 0, true);
    REQUIRE(parent);
    REQUIRE(parent->GetMenuBar() == nullptr);

    wxMenu * const windowMenu = parent->GetWindowMenu();
    REQUIRE(windowMenu);
    CHECK_FALSE(windowMenu->IsAttached());

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(parent, true);
    REQUIRE(host);
    const unsigned steadySlots = wxWinUITopLevelHost::GetLiveSlotCount();

    MdiChildSurface first = CreateChildSurface(
        parent, "Document before menu", wxRect(40, 40, 420, 300));
    REQUIRE(first.child);
    REQUIRE(first.button);
    wxWeakRef<MdiChildProbe> weakFirst(first.child);
    wxWeakRef<wxButton> weakFirstButton(first.button);

    REQUIRE(DrainUntil(
        [parent, first, host]()
        {
            return parent->GetActiveChild() == first.child &&
                   HasSingleHostOwner(first.button, host);
        }));
    CHECK(parent->GetMenuBar() == nullptr);
    CHECK_FALSE(windowMenu->IsAttached());

    wxMenuBar * const lateBar = new wxMenuBar;
    wxMenu * const applicationMenu = new wxMenu;
    applicationMenu->Append(ID_MDI_PARENT_COMMAND, "Late parent command");
    lateBar->Append(applicationMenu, "&Application");
    parent->SetMenuBar(lateBar);

    REQUIRE(parent->GetMenuBar() == lateBar);
    REQUIRE(windowMenu->IsAttached());
    CHECK(windowMenu->GetMenuBar() == lateBar);

    const HWND parentHwnd = static_cast<HWND>(parent->GetHWND());
    const HMENU lateFrameMenu = static_cast<HMENU>(lateBar->GetHMenu());
    const HMENU windowHMenu = static_cast<HMENU>(windowMenu->GetHMenu());
    REQUIRE(parentHwnd);
    REQUIRE(lateFrameMenu);
    REQUIRE(windowHMenu);
    CHECK(::GetMenu(parentHwnd) == lateFrameMenu);

    const auto countWindowMenu = [windowHMenu](HMENU frameMenu)
    {
        unsigned count = 0;
        const int itemCount = ::GetMenuItemCount(frameMenu);
        for ( int pos = 0; pos < itemCount; ++pos )
        {
            if ( ::GetSubMenu(frameMenu, pos) == windowHMenu )
                ++count;
        }
        return count;
    };

    CHECK(::GetMenuItemCount(lateFrameMenu) == 2);
    CHECK(countWindowMenu(lateFrameMenu) == 1);
    CHECK(HasSingleHostOwner(first.button, host));
    CHECK(wxWinUITopLevelHost::GetLiveSlotCount() == steadySlots + 1);

    // Replacing the bar must first remove the separately inserted Window menu
    // from the old HMENU and detach its wx ownership. The replacement then
    // becomes the sole owner and receives exactly one insertion.
    wxMenuBar * const replacementBar = new wxMenuBar;
    wxMenu * const replacementApplicationMenu = new wxMenu;
    replacementApplicationMenu->Append(
        ID_MDI_PARENT_COMMAND, "Replacement parent command");
    replacementBar->Append(replacementApplicationMenu, "&Replacement");
    parent->SetMenuBar(replacementBar);
    std::unique_ptr<wxMenuBar> detachedLateBar(lateBar);

    REQUIRE(parent->GetMenuBar() == replacementBar);
    CHECK_FALSE(lateBar->IsAttached());
    CHECK(::GetMenuItemCount(lateFrameMenu) == 1);
    CHECK(countWindowMenu(lateFrameMenu) == 0);
    REQUIRE(windowMenu->IsAttached());
    CHECK(windowMenu->GetMenuBar() == replacementBar);

    const HMENU replacementFrameMenu =
        static_cast<HMENU>(replacementBar->GetHMenu());
    REQUIRE(replacementFrameMenu);
    CHECK(::GetMenu(parentHwnd) == replacementFrameMenu);
    CHECK(::GetMenuItemCount(replacementFrameMenu) == 2);
    CHECK(countWindowMenu(replacementFrameMenu) == 1);
    CHECK(HasSingleHostOwner(first.button, host));

    // Removing the bar while the child remains active must clear the wx
    // association and remove Window from the detached HMENU. The detached bar
    // is owned by this scope while the parent owns only its current bar.
    parent->SetMenuBar(nullptr);
    std::unique_ptr<wxMenuBar> detachedReplacementBar(replacementBar);

    REQUIRE(parent->GetMenuBar() == nullptr);
    CHECK_FALSE(windowMenu->IsAttached());
    CHECK(::GetMenuItemCount(replacementFrameMenu) == 1);
    CHECK(countWindowMenu(replacementFrameMenu) == 0);
    CHECK(HasSingleHostOwner(first.button, host));

    // A third bar proves that the null transition didn't poison the next
    // association. Leave the original child and slot alive for the parent-
    // cascade teardown; any double detach remains a deterministic assertion.
    wxMenuBar * const finalBar = new wxMenuBar;
    wxMenu * const finalApplicationMenu = new wxMenu;
    finalApplicationMenu->Append(
        ID_MDI_PARENT_COMMAND, "Final parent command");
    finalBar->Append(finalApplicationMenu, "&Final");
    parent->SetMenuBar(finalBar);

    REQUIRE(parent->GetMenuBar() == finalBar);
    REQUIRE(windowMenu->IsAttached());
    CHECK(windowMenu->GetMenuBar() == finalBar);
    const HMENU finalFrameMenu = static_cast<HMENU>(finalBar->GetHMenu());
    REQUIRE(finalFrameMenu);
    CHECK(::GetMenu(parentHwnd) == finalFrameMenu);
    CHECK(::GetMenuItemCount(finalFrameMenu) == 2);
    CHECK(countWindowMenu(finalFrameMenu) == 1);
    CHECK(HasSingleHostOwner(first.button, host));
    CHECK(wxWinUITopLevelHost::GetLiveSlotCount() == steadySlots + 1);

    REQUIRE(environment.DestroyAndWait(before));
    CHECK_FALSE(weakFirst);
    CHECK_FALSE(weakFirstButton);
    before.CheckRestored();
}
#endif // wxUSE_MENUS

TEST_CASE("WinUI native MDI migrates child content between parent hosts",
          "[winui-mdi][winui-015][winui][mdi][advanced][reparent][focus][lifetime]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();
    ImpossibleZOrderResetGuard impossibleZOrderReset;
    NativeFocusRestoreGuard focusRestore;

    MdiEnvironment environment;
    MdiParentProbe * const parentA =
        environment.CreateParent("WinUI native MDI parent A", 0);
    MdiParentProbe * const parentB =
        environment.CreateParent("WinUI native MDI parent B", 1);
    REQUIRE(parentA);
    REQUIRE(parentB);

    MdiChildSurface childA = CreateChildSurface(
        parentA, "Document A", wxRect(40, 40, 460, 340), false);
    MdiChildSurface childB = CreateChildSurface(
        parentB, "Document B", wxRect(40, 40, 460, 340), false);
    REQUIRE(childA.child);
    REQUIRE(childA.panel);
    REQUIRE(childB.child);
    REQUIRE(childB.panel);

    wxButton * const migrating = new wxButton(
        childA.panel, wxID_ANY, "Cross-parent XAML sentinel",
        wxPoint(16, 16), wxSize(300, 120));
    wxWeakRef<wxButton> weakMigrating(migrating);
    wxWinUITopLevelHost * const hostA =
        wxWinUITopLevelHost::ForWindow(migrating, true);
    wxWinUITopLevelHost * const hostB =
        wxWinUITopLevelHost::ForWindow(childB.panel, true);
    REQUIRE(hostA);
    REQUIRE(hostB);
    REQUIRE(hostA != hostB);
    REQUIRE(DrainUntil([migrating, hostA]()
                       { return HasSingleHostOwner(migrating, hostA); }));
    CHECK(wxWinUITopLevelHost::GetLiveHostCount() == before.hosts + 2);

    childA.child->Activate();
    migrating->SetFocus();
    REQUIRE(DrainUntil(
        [migrating, hostA]()
        {
            return wxWindow::FindFocus() == migrating &&
                   hostA->GetFocusOwner() == migrating;
        }));
    const unsigned pendingFocusMigrations =
        wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest();

    const auto moveFocused =
        [migrating, pendingFocusMigrations](
            MdiChildSurface& destination,
            wxWinUITopLevelHost *destinationHost,
            wxWinUITopLevelHost *sourceHost)
        {
            REQUIRE(migrating->Reparent(destination.panel));
            migrating->SetSize(wxRect(16, 16, 300, 120));
            REQUIRE(DrainUntil(
                [migrating, destinationHost, sourceHost]()
                {
                    return HasSingleHostOwner(migrating, destinationHost) &&
                           !sourceHost->FindSlot(migrating) &&
                           destinationHost->GetFocusOwner() == migrating &&
                           wxWindow::FindFocus() == migrating;
                }));
            CHECK(wxGetTopLevelParent(migrating) ==
                  destination.child->GetMDIParent());
            CHECK(wxWinUITopLevelHost::
                      GetPendingFocusMigrationCountForTest() ==
                  pendingFocusMigrations);
        };

    // Exercise the focused transaction in both directions before the bulk
    // stress. This is the path that parks focus on the shell while slot
    // ownership moves atomically between two parent hosts.
    moveFocused(childB, hostB, hostA);
    moveFocused(childA, hostA, hostB);

    for ( unsigned cycle = 0; cycle < 100; ++cycle )
    {
        const bool toB = (cycle % 2) == 0;
        MdiChildSurface& destination = toB ? childB : childA;
        wxWinUITopLevelHost * const destinationHost = toB ? hostB : hostA;
        wxWinUITopLevelHost * const sourceHost = toB ? hostA : hostB;

        moveFocused(destination, destinationHost, sourceHost);
    }

    REQUIRE(wxGetTopLevelParent(migrating) == parentA);
    REQUIRE(wxWindow::FindFocus() == migrating);
    CHECK(hostA->GetFocusOwner() == migrating);
    CHECK(HasSingleHostOwner(migrating, hostA));
    CHECK_FALSE(hostB->FindSlot(migrating));
    CHECK(wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest() ==
          pendingFocusMigrations);

    REQUIRE(environment.DestroyAndWait(before));
    CHECK_FALSE(weakMigrating);
    before.CheckRestored();
}

TEST_CASE("WinUI native MDI revokes child slots and destroys live children",
          "[winui-mdi][winui-015][winui][mdi][advanced][lifetime][stress]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();
    ImpossibleZOrderResetGuard impossibleZOrderReset;
    NativeFocusRestoreGuard focusRestore;

    MdiEnvironment environment;
    MdiParentProbe * const parent =
        environment.CreateParent("WinUI native MDI lifetime", 0);
    REQUIRE(parent);
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(parent, true);
    REQUIRE(host);
    const unsigned steadySlots = wxWinUITopLevelHost::GetLiveSlotCount();

    for ( unsigned cycle = 0; cycle < 100; ++cycle )
    {
        MdiChildSurface surface = CreateChildSurface(
            parent,
            wxString::Format("Stress document %u", cycle),
            wxRect(30, 30, 420, 300));
        REQUIRE(surface.child);
        REQUIRE(surface.button);
        wxWeakRef<MdiChildProbe> weakChild(surface.child);
        wxWeakRef<wxButton> weakButton(surface.button);
        REQUIRE(DrainUntil([surface, host]()
                           { return HasSingleHostOwner(surface.button, host); }));

        REQUIRE(surface.child->Destroy());
        REQUIRE(DrainUntil(
            [&weakChild, &weakButton, steadySlots]()
            {
                return !weakChild && !weakButton &&
                       wxWinUITopLevelHost::GetLiveSlotCount() == steadySlots;
            }));
    }

    MdiChildSurface vetoed = CreateChildSurface(
        parent, "Vetoed document", wxRect(30, 30, 420, 300));
    MdiChildSurface liveAtParentDestroy = CreateChildSurface(
        parent, "Live during parent destroy", wxRect(170, 130, 420, 300));
    REQUIRE(vetoed.child);
    REQUIRE(vetoed.button);
    REQUIRE(liveAtParentDestroy.child);
    REQUIRE(liveAtParentDestroy.button);
    wxWeakRef<MdiChildProbe> weakVetoed(vetoed.child);
    wxWeakRef<wxButton> weakVetoedButton(vetoed.button);
    wxWeakRef<MdiChildProbe> weakLive(liveAtParentDestroy.child);
    wxWeakRef<wxButton> weakLiveButton(liveAtParentDestroy.button);

    REQUIRE(DrainUntil(
        [vetoed, liveAtParentDestroy, host, steadySlots]()
        {
            return HasSingleHostOwner(vetoed.button, host) &&
                   HasSingleHostOwner(liveAtParentDestroy.button, host) &&
                   wxWinUITopLevelHost::GetLiveSlotCount() ==
                       steadySlots + 2;
        }));
    CHECK(wxWinUITopLevelHost::GetLiveSlotCount() == steadySlots + 2);

    bool vetoClose = true;
    vetoed.child->Bind(
        wxEVT_CLOSE_WINDOW,
        [&vetoClose](wxCloseEvent& event)
        {
            if ( vetoClose )
                event.Veto();
            else
                event.Skip();
        });
    CHECK_FALSE(vetoed.child->Close());
    CHECK(weakVetoed);
    CHECK(weakVetoedButton);
    CHECK(HasSingleHostOwner(vetoed.button, host));
    CHECK(HasSingleHostOwner(liveAtParentDestroy.button, host));
    CHECK(wxWinUITopLevelHost::GetLiveSlotCount() == steadySlots + 2);
    vetoClose = false;
    REQUIRE(vetoed.child->Close());
    REQUIRE(DrainUntil(
        [&weakVetoed, &weakVetoedButton,
         liveAtParentDestroy, host, steadySlots]()
        {
            return !weakVetoed && !weakVetoedButton &&
                   HasSingleHostOwner(liveAtParentDestroy.button, host) &&
                   wxWinUITopLevelHost::GetLiveSlotCount() ==
                       steadySlots + 1;
        }));
    CHECK(wxWinUITopLevelHost::GetLiveSlotCount() == steadySlots + 1);
    CHECK(HasSingleHostOwner(liveAtParentDestroy.button, host));

    // Deliberately leave the second child and its slot alive. Unlike AUI MDI,
    // native MSW MDI owns a separate MDICLIENT and is expected to destroy wx
    // children before deleting that client. This is the exact parent-cascade
    // teardown contract, not a test that quietly closes every child first.
    REQUIRE(environment.DestroyAndWait(before));
    CHECK_FALSE(weakLive);
    CHECK_FALSE(weakLiveButton);
    before.CheckRestored();
}

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_MDI && wxUSE_BUTTON
