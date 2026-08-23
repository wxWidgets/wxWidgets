///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuiaui.cpp
// Purpose:     WinUI AUI fallback public-contract and host-lifecycle tests
// Author:      wxWidgets development team
// Created:     2026-08-08
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_AUI

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/bitmap.h"
    #include "wx/button.h"
    #include "wx/frame.h"
    #include "wx/panel.h"
    #include "wx/sizer.h"
    #include "wx/textctrl.h"
    #include "wx/utils.h"
#endif // WX_PRECOMP

#include "wx/aui/auibar.h"
#include "wx/aui/barartwinui.h"
#include "wx/aui/auibook.h"
#include "wx/aui/floatpane.h"
#include "wx/aui/framemanager.h"
#if wxUSE_MDI
    #include "wx/aui/tabmdi.h"
#endif
#include "wx/dcclient.h"
#include "wx/dcmemory.h"
#if wxUSE_MENUS
    #include "wx/menu.h"
    #include "wx/menuitem.h"
#endif
#include "wx/msw/wrapwin.h"
#include "wx/weakref.h"
#include "wx/winui/private/dialogsession.h"
#include "wx/winui/private/tlwhost.h"

#include <array>
#include <memory>
#include <vector>

namespace
{

enum
{
    ID_AUI_TOOL_NORMAL = wxID_HIGHEST + 6100,
    ID_AUI_TOOL_CHECK,
    ID_AUI_TOOL_RADIO_A,
    ID_AUI_TOOL_RADIO_B,
    ID_AUI_TOOL_DROPDOWN,
    ID_AUI_TOOL_TEMPORARY,
    ID_AUI_TOOL_CONTROL,
    ID_AUI_MDI_COMMAND
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
    frame->Move(wxPoint(-30000 + ordinal * 16, -30000));
    frame->ShowWithoutActivating();
    return true;
}

void DrainDispatch(unsigned rounds = 20)
{
    for ( unsigned i = 0; i < rounds; ++i )
    {
        wxYield();
        wxMilliSleep(2);
    }
}

template <typename Predicate>
bool DrainUntil(Predicate predicate, unsigned rounds = 120)
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
    for ( unsigned i = 0; i < 50; ++i )
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

    void CheckRestored(bool requireHandlerTraffic = true) const
    {
        CHECK(wxWinUITopLevelHost::GetLiveHostCount() == hosts);
        CHECK(wxWinUITopLevelHost::GetLiveHostLifetimeCount() ==
              hostLifetimes);
        CHECK(wxWinUITopLevelHost::GetLiveSlotCount() == slots);
        CHECK(wxWinUITopLevelHost::GetLiveSlotLifetimeCount() ==
              slotLifetimes);
        CHECK(wxWinUITopLevelHost::GetLiveSubclassContextCount() ==
              subclasses);
        CHECK(wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest() ==
              pendingFocusMigrations);

        const unsigned slotAdds =
            wxWinUITopLevelHost::GetSlotHandlerAddCount() - slotHandlerAdds;
        const unsigned slotRevokes =
            wxWinUITopLevelHost::GetSlotHandlerRevokeCount() -
                slotHandlerRevokes;
        if ( requireHandlerTraffic )
            CHECK(slotAdds > 0);
        CHECK(slotAdds == slotRevokes);

        const unsigned rootAdds =
            wxWinUITopLevelHost::GetRootHandlerAddCount() - rootHandlerAdds;
        const unsigned rootRevokes =
            wxWinUITopLevelHost::GetRootHandlerRevokeCount() -
                rootHandlerRevokes;
        if ( requireHandlerTraffic )
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
    unsigned pendingFocusMigrations = 0;
    unsigned slotHandlerAdds = 0;
    unsigned slotHandlerRevokes = 0;
    unsigned rootHandlerAdds = 0;
    unsigned rootHandlerRevokes = 0;
    wxWinUITransientRegistrySnapshot transients;
    wxWindow *wxCapture = nullptr;
    HWND nativeCapture = nullptr;
};

bool HasSingleHostOwner(wxWindow *slotWindow, wxWindow *expectedTLW)
{
    if ( !slotWindow || !expectedTLW )
        return false;

    wxWinUITopLevelHost * const expectedHost =
        wxWinUITopLevelHost::FindForTLW(expectedTLW);
    return expectedHost &&
           wxWinUITopLevelHost::FindSlotOwner(slotWindow) == expectedHost &&
           expectedHost->FindSlot(slotWindow);
}

bool HostDoesNotContain(wxWindow *slotWindow, wxWindow *tlw)
{
    if ( !slotWindow || !tlw )
        return false;

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindForTLW(tlw);
    return !host || !host->FindSlot(slotWindow);
}

wxBitmapBundle MakeTestBitmapBundle()
{
    wxBitmap normal;
    wxBitmap highDpi;
    if ( !normal.Create(16, 16, 32) || !highDpi.Create(32, 32, 32) )
        return wxBitmapBundle();
    return wxBitmapBundle::FromBitmaps(normal, highDpi);
}

class GdiProbePanel final : public wxPanel
{
public:
    explicit GdiProbePanel(wxWindow *parent)
        : wxPanel(parent, wxID_ANY)
    {
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        Bind(wxEVT_PAINT, &GdiProbePanel::OnPaint, this);
        SetMinSize(wxSize(80, 60));
    }

private:
    void OnPaint(wxPaintEvent&)
    {
        wxPaintDC dc(this);
        dc.SetBackground(*wxLIGHT_GREY_BRUSH);
        dc.Clear();
        dc.DrawLine(0, 0, GetClientSize().x, GetClientSize().y);
    }
};

class RecordingAuiManager final : public wxAuiManager
{
public:
    RecordingAuiManager(wxWindow *managedWindow, int offscreenOrdinal)
        : wxAuiManager(managedWindow),
          m_offscreenOrdinal(offscreenOrdinal)
    {
    }

    wxAuiFloatingFrame *CreateFloatingFrame(
        wxWindow *parent,
        const wxAuiPaneInfo& paneInfo) override
    {
        wxAuiFloatingFrame * const frame =
            wxAuiManager::CreateFloatingFrame(parent, paneInfo);
        if ( !frame )
            return nullptr;

        const HWND hwnd = static_cast<HWND>(frame->GetHWND());
        if ( hwnd )
        {
            ::SetWindowLongPtr(
                hwnd,
                GWL_EXSTYLE,
                ::GetWindowLongPtr(hwnd, GWL_EXSTYLE) |
                    WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW);
        }
        frame->Move(wxPoint(-30000 + m_offscreenOrdinal * 16, -29600));
        m_lastFloatingFrame = wxWeakRef<wxAuiFloatingFrame>(frame);
        ++m_floatingFrameCount;
        return frame;
    }

    wxAuiFloatingFrame *GetLastFloatingFrame() const
    {
        return m_lastFloatingFrame.get();
    }

    unsigned GetFloatingFrameCount() const
    {
        return m_floatingFrameCount;
    }

private:
    int m_offscreenOrdinal = 0;
    wxWeakRef<wxAuiFloatingFrame> m_lastFloatingFrame;
    unsigned m_floatingFrameCount = 0;
};

wxPanel *CreatePaneWithSentinel(wxWindow *parent,
                                const wxString& label,
                                wxWeakRef<wxButton> *weakSentinel)
{
    wxPanel * const pane = new wxPanel(parent, wxID_ANY);
    wxButton * const sentinel = new wxButton(pane, wxID_ANY, label);
    wxBoxSizer * const sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(sentinel, 1, wxEXPAND | wxALL, 4);
    pane->SetSizer(sizer);
    if ( weakSentinel )
        *weakSentinel = wxWeakRef<wxButton>(sentinel);
    return pane;
}

class ManagerFixture final
{
public:
    ~ManagerFixture()
    {
        Cleanup();
        DrainDispatch();
    }

    bool Create(int ordinal)
    {
        wxFrame * const frameA = new wxFrame(
            nullptr, wxID_ANY, "WinUI AUI manager A",
            wxDefaultPosition, wxSize(980, 700));
        wxFrame * const frameB = new wxFrame(
            nullptr, wxID_ANY, "WinUI AUI manager B",
            wxDefaultPosition, wxSize(780, 560));
        m_frameA = wxWeakRef<wxFrame>(frameA);
        m_frameB = wxWeakRef<wxFrame>(frameB);
        if ( !frameA->GetHWND() || !frameB->GetHWND() )
            return false;

        m_managerA =
            std::make_unique<RecordingAuiManager>(frameA, ordinal + 2);
        m_managerB =
            std::make_unique<RecordingAuiManager>(frameB, ordinal + 3);

        static const char * const names[] =
        {
            "left", "right", "top", "bottom"
        };
        for ( size_t i = 0; i < m_sidePanes.size(); ++i )
        {
            wxWeakRef<wxButton> weakSentinel;
            wxPanel * const pane = CreatePaneWithSentinel(
                frameA,
                wxString("WinUI AUI ") + wxString::FromUTF8(names[i]),
                &weakSentinel);
            m_sidePanes[i] = wxWeakRef<wxPanel>(pane);
            m_sideSentinels[i] = weakSentinel;

            wxAuiPaneInfo info;
            info.Name(wxString::FromUTF8(names[i]))
                .Caption(wxString::FromUTF8(names[i]))
                .BestSize(180, 120)
                .CloseButton()
                .Floatable()
                .Movable();
            switch ( i )
            {
                case 0: info.Left(); break;
                case 1: info.Right(); break;
                case 2: info.Top(); break;
                case 3: info.Bottom(); break;
            }
            if ( !m_managerA->AddPane(pane, info) )
                return false;
        }

        wxPanel * const nestedRoot = new wxPanel(frameA, wxID_ANY);
        m_nestedRoot = wxWeakRef<wxPanel>(nestedRoot);
        if ( !m_managerA->AddPane(
                nestedRoot,
                wxAuiPaneInfo().Name("center").CenterPane()) )
        {
            return false;
        }

        m_nestedManager = std::make_unique<wxAuiManager>(nestedRoot);
        GdiProbePanel * const gdi = new GdiProbePanel(nestedRoot);
        wxButton * const xaml = new wxButton(
            nestedRoot, wxID_ANY, "nested WinUI XAML sentinel");
        m_gdi = wxWeakRef<GdiProbePanel>(gdi);
        m_nestedSentinel = wxWeakRef<wxButton>(xaml);
        if ( !m_nestedManager->AddPane(
                 gdi,
                 wxAuiPaneInfo().Name("nested-gdi").Left().BestSize(160, 120)) ||
             !m_nestedManager->AddPane(
                 xaml,
                 wxAuiPaneInfo().Name("nested-xaml").CenterPane()) )
        {
            return false;
        }

        m_nestedManager->Update();
        m_managerA->Update();
        m_managerB->Update();
        if ( !ShowOffscreenWithoutActivation(frameA, ordinal) ||
             !ShowOffscreenWithoutActivation(frameB, ordinal + 1) )
        {
            return false;
        }

        return DrainUntil(
            [this]()
            {
                if ( !HasSingleHostOwner(m_nestedSentinel.get(),
                                         m_frameA.get()) )
                {
                    return false;
                }
                for ( const wxWeakRef<wxButton>& sentinel :
                      m_sideSentinels )
                {
                    if ( !HasSingleHostOwner(sentinel.get(), m_frameA.get()) )
                        return false;
                }
                return true;
            });
    }

    bool DestroyAndWait(const RuntimeSnapshot& before)
    {
        Cleanup();
        if ( !DrainUntil(
                [this, &before]()
                {
                    if ( m_frameA || m_frameB || m_nestedRoot || m_gdi ||
                         m_nestedSentinel )
                    {
                        return false;
                    }
                    for ( const wxWeakRef<wxPanel>& pane : m_sidePanes )
                    {
                        if ( pane )
                            return false;
                    }
                    for ( const wxWeakRef<wxButton>& sentinel :
                          m_sideSentinels )
                    {
                        if ( sentinel )
                            return false;
                    }
                    return before.IsRestored();
                }) )
        {
            return false;
        }
        return DrainToQuiescence() && before.IsRestored();
    }

    wxFrame *GetFrameA() const { return m_frameA.get(); }
    wxFrame *GetFrameB() const { return m_frameB.get(); }
    RecordingAuiManager *GetManagerA() const { return m_managerA.get(); }
    RecordingAuiManager *GetManagerB() const { return m_managerB.get(); }
    wxAuiManager *GetNestedManager() const { return m_nestedManager.get(); }
    wxPanel *GetSidePane(size_t index) const
    {
        return index < m_sidePanes.size() ? m_sidePanes[index].get()
                                          : nullptr;
    }
    wxButton *GetSideSentinel(size_t index) const
    {
        return index < m_sideSentinels.size()
                   ? m_sideSentinels[index].get()
                   : nullptr;
    }
    wxPanel *GetNestedRoot() const { return m_nestedRoot.get(); }
    GdiProbePanel *GetGdiProbe() const { return m_gdi.get(); }
    wxButton *GetNestedSentinel() const { return m_nestedSentinel.get(); }

private:
    void Cleanup()
    {
        if ( m_cleaned )
            return;
        m_cleaned = true;

        const auto redockFloating = [](RecordingAuiManager *manager)
        {
            if ( !manager || !manager->GetManagedWindow() ||
                 manager->GetManagedWindow()->IsBeingDeleted() )
            {
                return;
            }

            bool changed = false;
            for ( wxAuiPaneInfo& pane : manager->GetAllPanes() )
            {
                if ( pane.IsFloating() )
                {
                    pane.Dock();
                    changed = true;
                }
            }
            if ( changed )
            {
                manager->Update();
                DrainUntil(
                    [manager]()
                    {
                        for ( const wxAuiPaneInfo& pane :
                              manager->GetAllPanes() )
                        {
                            if ( pane.frame )
                                return false;
                        }
                        return true;
                    });
            }
        };

        redockFloating(m_managerA.get());
        redockFloating(m_managerB.get());

        if ( m_nestedManager )
            m_nestedManager->UnInit();
        m_nestedManager.reset();
        if ( m_managerA )
            m_managerA->UnInit();
        if ( m_managerB )
            m_managerB->UnInit();
        m_managerA.reset();
        m_managerB.reset();

        if ( wxFrame * const frame = m_frameA.get() )
        {
            if ( !frame->IsBeingDeleted() )
                frame->Destroy();
        }
        if ( wxFrame * const frame = m_frameB.get() )
        {
            if ( !frame->IsBeingDeleted() )
                frame->Destroy();
        }
    }

    bool m_cleaned = false;
    wxWeakRef<wxFrame> m_frameA;
    wxWeakRef<wxFrame> m_frameB;
    std::array<wxWeakRef<wxPanel>, 4> m_sidePanes;
    std::array<wxWeakRef<wxButton>, 4> m_sideSentinels;
    wxWeakRef<wxPanel> m_nestedRoot;
    wxWeakRef<GdiProbePanel> m_gdi;
    wxWeakRef<wxButton> m_nestedSentinel;
    std::unique_ptr<RecordingAuiManager> m_managerA;
    std::unique_ptr<RecordingAuiManager> m_managerB;
    std::unique_ptr<wxAuiManager> m_nestedManager;
};

class ManagerCloseObserver final
{
public:
    ManagerCloseObserver(wxFrame *frame, wxAuiManager *manager)
        : m_frame(frame), m_manager(manager)
    {
        frame->Bind(wxEVT_AUI_PANE_CLOSE,
                    &ManagerCloseObserver::OnClose, this);
    }

    ~ManagerCloseObserver()
    {
        if ( wxFrame * const frame = m_frame.get() )
        {
            frame->Unbind(wxEVT_AUI_PANE_CLOSE,
                          &ManagerCloseObserver::OnClose, this);
        }
    }

    void SetTarget(wxWindow *target) { m_target = target; }
    void SetVeto(bool veto) { m_veto = veto; }
    void SetDestroyDuringCallback(bool destroy)
    {
        m_destroyDuringCallback = destroy;
    }
    unsigned GetCallCount() const { return m_callCount; }

private:
    void OnClose(wxAuiManagerEvent& event)
    {
        if ( !event.GetPane() || event.GetPane()->window != m_target )
        {
            event.Skip();
            return;
        }

        ++m_callCount;
        if ( m_destroyDuringCallback )
        {
            wxWindow * const target = m_target;
            m_target = nullptr;
            m_manager->DetachPane(target);
            target->Destroy();
            m_manager->Update();
            return;
        }

        if ( m_veto )
            event.Veto();
        else
            event.Skip();
    }

    wxWeakRef<wxFrame> m_frame;
    wxAuiManager *m_manager = nullptr;
    wxWindow *m_target = nullptr;
    bool m_veto = false;
    bool m_destroyDuringCallback = false;
    unsigned m_callCount = 0;
};

void InvokePaneCloseButton(wxAuiManager& manager, wxAuiPaneInfo& pane)
{
    wxAuiManagerEvent event(wxEVT_AUI_PANE_BUTTON);
    event.SetManager(&manager);
    event.SetPane(&pane);
    event.SetButton(wxAUI_BUTTON_CLOSE);
    manager.OnPaneButton(event);
}

class PerspectiveRenderObserver final
{
public:
    PerspectiveRenderObserver(wxFrame *frame,
                              wxAuiManager *manager,
                              int expectedDirection,
                              bool expectedRightShown)
        : m_frame(frame),
          m_manager(manager),
          m_expectedDirection(expectedDirection),
          m_expectedRightShown(expectedRightShown)
    {
        frame->Bind(wxEVT_AUI_RENDER,
                    &PerspectiveRenderObserver::OnRender, this);
    }

    ~PerspectiveRenderObserver()
    {
        if ( wxFrame * const frame = m_frame.get() )
        {
            frame->Unbind(wxEVT_AUI_RENDER,
                          &PerspectiveRenderObserver::OnRender, this);
        }
    }

    unsigned GetCallCount() const { return m_callCount; }
    bool SawCommittedState() const { return m_sawCommittedState; }

private:
    void OnRender(wxAuiManagerEvent& event)
    {
        ++m_callCount;
        const wxAuiPaneInfo& left = m_manager->GetPane("left");
        const wxAuiPaneInfo& right = m_manager->GetPane("right");
        m_sawCommittedState =
            left.IsOk() && right.IsOk() &&
            left.dock_direction == m_expectedDirection &&
            right.IsShown() == m_expectedRightShown;

        // Re-enter a read-only public serializer from the paint callback. It
        // must observe the committed candidate, never a half-loaded layout.
        m_observedPerspective = m_manager->SavePerspective();
        event.Skip();
    }

    wxWeakRef<wxFrame> m_frame;
    wxAuiManager *m_manager = nullptr;
    int m_expectedDirection = wxAUI_DOCK_NONE;
    bool m_expectedRightShown = false;
    unsigned m_callCount = 0;
    bool m_sawCommittedState = false;
    wxString m_observedPerspective;
};

class OffscreenFramePair final
{
public:
    ~OffscreenFramePair()
    {
        Cleanup();
        DrainDispatch();
    }

    bool Create(const wxString& label, int ordinal)
    {
        wxFrame * const frameA = new wxFrame(
            nullptr, wxID_ANY, label + " A",
            wxDefaultPosition, wxSize(900, 640));
        wxFrame * const frameB = new wxFrame(
            nullptr, wxID_ANY, label + " B",
            wxDefaultPosition, wxSize(760, 540));
        m_frameA = wxWeakRef<wxFrame>(frameA);
        m_frameB = wxWeakRef<wxFrame>(frameB);
        return frameA->GetHWND() && frameB->GetHWND() &&
               ShowOffscreenWithoutActivation(frameA, ordinal) &&
               ShowOffscreenWithoutActivation(frameB, ordinal + 1);
    }

    template <typename Predicate>
    bool DestroyAndWait(const RuntimeSnapshot& before, Predicate predicate)
    {
        Cleanup();
        return DrainUntil(
                   [this, &before, &predicate]()
                   {
                       return !m_frameA && !m_frameB && predicate() &&
                              before.IsRestored();
                   }) &&
               DrainToQuiescence() && before.IsRestored();
    }

    wxFrame *GetFrameA() const { return m_frameA.get(); }
    wxFrame *GetFrameB() const { return m_frameB.get(); }

private:
    void Cleanup()
    {
        if ( m_cleaned )
            return;
        m_cleaned = true;

        if ( wxFrame * const frame = m_frameA.get() )
        {
            if ( !frame->IsBeingDeleted() )
                frame->Destroy();
        }
        if ( wxFrame * const frame = m_frameB.get() )
        {
            if ( !frame->IsBeingDeleted() )
                frame->Destroy();
        }
    }

    bool m_cleaned = false;
    wxWeakRef<wxFrame> m_frameA;
    wxWeakRef<wxFrame> m_frameB;
};

class ToolBarFixture final
{
public:
    ~ToolBarFixture()
    {
        Cleanup();
    }

    bool Create(int ordinal)
    {
        if ( !m_frames.Create("WinUI AUI toolbar", ordinal) )
            return false;

        m_managerA = std::make_unique<RecordingAuiManager>(
            m_frames.GetFrameA(), ordinal + 2);
        m_managerB = std::make_unique<RecordingAuiManager>(
            m_frames.GetFrameB(), ordinal + 3);

        wxAuiToolBar * const toolbar = new wxAuiToolBar(
            m_frames.GetFrameA(),
            wxID_ANY,
            wxPoint(0, 0),
            wxDefaultSize,
            wxAUI_TB_TEXT | wxAUI_TB_GRIPPER | wxAUI_TB_OVERFLOW);
        m_toolbar = wxWeakRef<wxAuiToolBar>(toolbar);
        if ( !m_managerA->AddPane(
                toolbar,
                wxAuiPaneInfo()
                    .Name("aui-toolbar")
                    .ToolbarPane()
                    .Top()
                    .Floatable()
                    .Movable()) )
        {
            return false;
        }
        m_managerA->Update();
        return true;
    }

    bool DestroyAndWait(const RuntimeSnapshot& before)
    {
        Cleanup();
        return m_frames.DestroyAndWait(
            before,
            [this]()
            {
                return !m_toolbar;
            });
    }

    wxFrame *GetFrameA() const { return m_frames.GetFrameA(); }
    wxFrame *GetFrameB() const { return m_frames.GetFrameB(); }
    wxAuiToolBar *GetToolBar() const { return m_toolbar.get(); }
    RecordingAuiManager *GetManagerA() const { return m_managerA.get(); }
    RecordingAuiManager *GetManagerB() const { return m_managerB.get(); }

private:
    static void Redock(RecordingAuiManager *manager)
    {
        if ( !manager || !manager->GetManagedWindow() ||
             manager->GetManagedWindow()->IsBeingDeleted() )
        {
            return;
        }

        bool changed = false;
        for ( wxAuiPaneInfo& pane : manager->GetAllPanes() )
        {
            if ( pane.IsFloating() )
            {
                pane.Dock();
                changed = true;
            }
        }
        if ( changed )
        {
            manager->Update();
            DrainUntil(
                [manager]()
                {
                    for ( const wxAuiPaneInfo& pane :
                          manager->GetAllPanes() )
                    {
                        if ( pane.frame )
                            return false;
                    }
                    return true;
                });
        }
    }

    void Cleanup()
    {
        if ( m_cleaned )
            return;
        m_cleaned = true;

        Redock(m_managerA.get());
        Redock(m_managerB.get());
        if ( m_managerA )
            m_managerA->UnInit();
        if ( m_managerB )
            m_managerB->UnInit();
        m_managerA.reset();
        m_managerB.reset();
    }

    bool m_cleaned = false;
    OffscreenFramePair m_frames;
    wxWeakRef<wxAuiToolBar> m_toolbar;
    std::unique_ptr<RecordingAuiManager> m_managerA;
    std::unique_ptr<RecordingAuiManager> m_managerB;
};

#if wxUSE_MDI
class MdiFixture final
{
public:
    ~MdiFixture()
    {
        Cleanup();
        DrainDispatch();
    }

    bool Create(int ordinal)
    {
        wxAuiMDIParentFrame * const parentA = new wxAuiMDIParentFrame(
            nullptr, wxID_ANY, "WinUI AUI MDI A",
            wxDefaultPosition, wxSize(900, 640));
        wxAuiMDIParentFrame * const parentB = new wxAuiMDIParentFrame(
            nullptr, wxID_ANY, "WinUI AUI MDI B",
            wxDefaultPosition, wxSize(760, 540));
        m_parentA = wxWeakRef<wxAuiMDIParentFrame>(parentA);
        m_parentB = wxWeakRef<wxAuiMDIParentFrame>(parentB);

        return parentA->GetClientWindow() && parentB->GetClientWindow() &&
               ShowOffscreenWithoutActivation(parentA, ordinal) &&
               ShowOffscreenWithoutActivation(parentB, ordinal + 1);
    }

    bool DestroyAndWait(const RuntimeSnapshot& before)
    {
        Cleanup();
        return DrainUntil(
                   [this, &before]()
                   {
                       return !m_parentA && !m_parentB &&
                              before.IsRestored();
                   }) &&
               DrainToQuiescence() && before.IsRestored();
    }

    wxAuiMDIParentFrame *GetParentA() const { return m_parentA.get(); }
    wxAuiMDIParentFrame *GetParentB() const { return m_parentB.get(); }

private:
    void Cleanup()
    {
        if ( m_cleaned )
            return;
        m_cleaned = true;
        if ( wxAuiMDIParentFrame * const parent = m_parentA.get() )
        {
            if ( !parent->IsBeingDeleted() )
                parent->Destroy();
        }
        if ( wxAuiMDIParentFrame * const parent = m_parentB.get() )
        {
            if ( !parent->IsBeingDeleted() )
                parent->Destroy();
        }
    }

    bool m_cleaned = false;
    wxWeakRef<wxAuiMDIParentFrame> m_parentA;
    wxWeakRef<wxAuiMDIParentFrame> m_parentB;
};
#endif // wxUSE_MDI

class ManagerSelfDestroyFixture final
{
public:
    ~ManagerSelfDestroyFixture()
    {
        m_manager.reset();
        if ( wxFrame * const frame = m_frame.get() )
        {
            frame->Unbind(wxEVT_AUI_PANE_CLOSE,
                          &ManagerSelfDestroyFixture::OnClose, this);
            if ( !frame->IsBeingDeleted() )
                frame->Destroy();
        }
        DrainUntil([this]() { return !m_frame; }, 200);
    }

    bool Create()
    {
        wxFrame * const frame = new wxFrame(
            nullptr, wxID_ANY, "AUI manager self-destroy",
            wxDefaultPosition, wxSize(360, 220),
            wxDEFAULT_FRAME_STYLE | wxFRAME_NO_TASKBAR |
                wxFRAME_TOOL_WINDOW);
        m_frame = wxWeakRef<wxFrame>(frame);
        if ( !ShowOffscreenWithoutActivation(frame, 96) )
            return false;

        m_manager = std::make_unique<wxAuiManager>(frame);
        m_pane = new wxPanel(frame, wxID_ANY);
        if ( !m_manager->AddPane(
                 m_pane,
                 wxAuiPaneInfo().Name("self-destroy").CenterPane()) )
        {
            return false;
        }
        m_manager->Update();

        frame->Bind(wxEVT_AUI_PANE_CLOSE,
                    &ManagerSelfDestroyFixture::OnClose, this);
        return true;
    }

    wxWeakRef<wxAuiManager> GetManagerWeak() const
    {
        return wxWeakRef<wxAuiManager>(m_manager.get());
    }

    bool InvokeClose()
    {
        if ( !m_manager || !m_pane )
            return false;

        wxAuiPaneInfo& pane = m_manager->GetPane(m_pane);
        if ( !pane.IsOk() )
            return false;

        InvokePaneCloseButton(*m_manager, pane);
        return true;
    }

    unsigned GetCloseCallbacks() const { return m_closeCallbacks; }

private:
    void OnClose(wxAuiManagerEvent& event)
    {
        ++m_closeCallbacks;
        m_manager.reset();
        event.Skip();
    }

    wxWeakRef<wxFrame> m_frame;
    std::unique_ptr<wxAuiManager> m_manager;
    wxPanel *m_pane = nullptr;
    unsigned m_closeCallbacks = 0;
};

} // anonymous namespace

TEST_CASE("WinUI AUI pane callback may destroy its manager",
          "[aui][winui][advanced][lifetime][WinUIAUI]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();

    {
        ManagerSelfDestroyFixture fixture;
        REQUIRE(fixture.Create());
        const wxWeakRef<wxAuiManager> weakManager = fixture.GetManagerWeak();
        REQUIRE(weakManager);
        REQUIRE(fixture.InvokeClose());
        CHECK(fixture.GetCloseCallbacks() == 1);
        CHECK_FALSE(weakManager);
    }

    REQUIRE(DrainUntil([&before]() { return before.IsRestored(); }, 200));
    // This fixture intentionally exercises only the manager callback
    // transaction: it creates no WinUI peer, so balanced zero handler traffic
    // is the expected result here.
    before.CheckRestored(false);
}

TEST_CASE("WinUI AUI manager preserves host and perspective transactions",
          "[aui][winui][advanced][WinUIAUI]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();

    ManagerFixture fixture;
    REQUIRE(fixture.Create(720));
    RecordingAuiManager * const manager = fixture.GetManagerA();
    REQUIRE(manager);

    static const int expectedDirections[] =
    {
        wxAUI_DOCK_LEFT,
        wxAUI_DOCK_RIGHT,
        wxAUI_DOCK_TOP,
        wxAUI_DOCK_BOTTOM
    };
    for ( size_t i = 0; i < WXSIZEOF(expectedDirections); ++i )
    {
        wxWindow * const pane = fixture.GetSidePane(i);
        REQUIRE(pane);
        const wxAuiPaneInfo& info = manager->GetPane(pane);
        CHECK(info.IsOk());
        CHECK(info.IsDocked());
        CHECK(info.IsShown());
        CHECK(info.dock_direction == expectedDirections[i]);
        CHECK(info.rect.GetWidth() > 0);
        CHECK(info.rect.GetHeight() > 0);
    }

    const wxAuiPaneInfo& centre = manager->GetPane("center");
    CHECK(centre.IsOk());
    CHECK(centre.dock_direction == wxAUI_DOCK_CENTER);
    CHECK(wxAuiManager::GetManager(fixture.GetGdiProbe()) ==
          fixture.GetNestedManager());
    CHECK(wxAuiManager::GetManager(fixture.GetNestedSentinel()) ==
          fixture.GetNestedManager());

    // A pane is identified by the window passed to AddPane(), not by the
    // normally empty PaneInfo::window field.
    const size_t paneCount = manager->GetAllPanes().size();
    CHECK_FALSE(manager->AddPane(
        fixture.GetSidePane(0), wxAuiPaneInfo().Name("duplicate-left")));
    CHECK(manager->GetAllPanes().size() == paneCount);

    const wxString stable = manager->SavePerspective();
    wxAuiPaneInfo alteredLeft = manager->GetPane("left");
    alteredLeft.Right().Row(2).Position(3);
    const wxString invalidMidStream =
        "layout3|" + manager->SavePaneInfo(alteredLeft) +
        "|name=broken;dir=not-a-number;|";
    CHECK_FALSE(manager->LoadPerspective(invalidMidStream, false));
    CHECK(manager->SavePerspective() == stable);

    const wxString invalidSuffix =
        "layout3|" + manager->SavePaneInfo(alteredLeft) + "||garbage";
    CHECK_FALSE(manager->LoadPerspective(invalidSuffix, false));
    CHECK(manager->SavePerspective() == stable);

    manager->GetPane("left").Bottom().Row(1);
    manager->GetPane("right").Hide();
    manager->Update();
    CHECK(manager->SavePerspective() != stable);

    PerspectiveRenderObserver renderObserver(
        fixture.GetFrameA(), manager, wxAUI_DOCK_LEFT, true);
    REQUIRE(manager->LoadPerspective(stable, true));
    CHECK(manager->GetPane("left").dock_direction == wxAUI_DOCK_LEFT);
    CHECK(manager->GetPane("right").IsShown());
    CHECK(renderObserver.GetCallCount() > 0);
    CHECK(renderObserver.SawCommittedState());
    CHECK(manager->SavePerspective() == stable);

    wxPanel * const leftPane = fixture.GetSidePane(0);
    wxButton * const leftSentinel = fixture.GetSideSentinel(0);
    REQUIRE(leftPane);
    REQUIRE(leftSentinel);
    manager->GetPane(leftPane)
        .Float()
        .FloatingPosition(-29400, -29400)
        .FloatingSize(360, 240);
    manager->Update();
    REQUIRE(DrainUntil(
        [manager, leftPane, leftSentinel, &fixture]()
        {
            wxAuiFloatingFrame * const floating =
                manager->GetLastFloatingFrame();
            return floating && manager->GetPane(leftPane).IsFloating() &&
                   leftPane->GetParent() == floating &&
                   HasSingleHostOwner(leftSentinel, floating) &&
                   HostDoesNotContain(leftSentinel, fixture.GetFrameA()) &&
                   HostDoesNotContain(leftSentinel, fixture.GetFrameB());
        }));
    wxWeakRef<wxAuiFloatingFrame> weakFloating(
        manager->GetLastFloatingFrame());

    manager->GetPane(leftPane).Dock().Left();
    manager->Update();
    REQUIRE(DrainUntil(
        [leftPane, leftSentinel, &fixture, &weakFloating]()
        {
            return !weakFloating &&
                   leftPane->GetParent() == fixture.GetFrameA() &&
                   HasSingleHostOwner(leftSentinel, fixture.GetFrameA()) &&
                   HostDoesNotContain(leftSentinel, fixture.GetFrameB());
        }));

    manager->GetPane(leftPane).Hide();
    manager->Update();
    CHECK_FALSE(manager->GetPane(leftPane).IsShown());
    manager->GetPane(leftPane).Show();
    manager->Update();
    CHECK(manager->GetPane(leftPane).IsShown());

    wxPanel * const rightPane = fixture.GetSidePane(1);
    wxButton * const rightSentinel = fixture.GetSideSentinel(1);
    REQUIRE(rightPane);
    REQUIRE(rightSentinel);
    REQUIRE(manager->DetachPane(rightPane));
    manager->Update();
    REQUIRE(rightPane->Reparent(fixture.GetFrameB()));
    REQUIRE(fixture.GetManagerB()->AddPane(
        rightPane,
        wxAuiPaneInfo().Name("right-b").Right().BestSize(180, 120)));
    fixture.GetManagerB()->Update();
    REQUIRE(DrainUntil(
        [rightSentinel, &fixture]()
        {
            return HasSingleHostOwner(rightSentinel, fixture.GetFrameB()) &&
                   HostDoesNotContain(rightSentinel, fixture.GetFrameA());
        }));

    REQUIRE(fixture.GetManagerB()->DetachPane(rightPane));
    fixture.GetManagerB()->Update();
    REQUIRE(rightPane->Reparent(fixture.GetFrameA()));
    REQUIRE(manager->AddPane(
        rightPane,
        wxAuiPaneInfo().Name("right").Right().BestSize(180, 120)));
    manager->Update();
    REQUIRE(DrainUntil(
        [rightSentinel, &fixture]()
        {
            return HasSingleHostOwner(rightSentinel, fixture.GetFrameA()) &&
                   HostDoesNotContain(rightSentinel, fixture.GetFrameB());
        }));

    ManagerCloseObserver closeObserver(fixture.GetFrameA(), manager);
    closeObserver.SetTarget(rightPane);
    closeObserver.SetVeto(true);
    InvokePaneCloseButton(*manager, manager->GetPane(rightPane));
    CHECK(closeObserver.GetCallCount() == 1);
    CHECK(manager->GetPane(rightPane).IsShown());

    closeObserver.SetVeto(false);
    InvokePaneCloseButton(*manager, manager->GetPane(rightPane));
    CHECK(closeObserver.GetCallCount() == 2);
    CHECK_FALSE(manager->GetPane(rightPane).IsShown());
    manager->GetPane(rightPane).Show();
    manager->Update();

    wxWeakRef<wxPanel> weakBottom(fixture.GetSidePane(3));
    wxWeakRef<wxButton> weakBottomSentinel(fixture.GetSideSentinel(3));
    closeObserver.SetTarget(fixture.GetSidePane(3));
    closeObserver.SetDestroyDuringCallback(true);
    InvokePaneCloseButton(
        *manager, manager->GetPane(fixture.GetSidePane(3)));
    REQUIRE(DrainUntil(
        [&weakBottom, &weakBottomSentinel]()
        {
            return !weakBottom && !weakBottomSentinel;
        }));
    CHECK_FALSE(manager->GetPane("bottom").IsOk());

    REQUIRE(fixture.DestroyAndWait(before));
    before.CheckRestored();
}

TEST_CASE("WinUI AUI manager model stress remains generation-clean",
          "[aui][winui][advanced][stress][WinUIAUI]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();

    ManagerFixture fixture;
    REQUIRE(fixture.Create(740));
    RecordingAuiManager * const manager = fixture.GetManagerA();
    wxPanel * const pane = fixture.GetSidePane(0);
    wxButton * const sentinel = fixture.GetSideSentinel(0);
    REQUIRE(manager);
    REQUIRE(pane);
    REQUIRE(sentinel);

    static const int directions[] =
    {
        wxAUI_DOCK_LEFT,
        wxAUI_DOCK_TOP,
        wxAUI_DOCK_RIGHT,
        wxAUI_DOCK_BOTTOM
    };
    for ( unsigned cycle = 0; cycle < 100; ++cycle )
    {
        wxAuiPaneInfo& info = manager->GetPane(pane);
        info.Dock().Direction(directions[cycle % WXSIZEOF(directions)]);
        info.Show((cycle % 3) != 0);
        manager->Update();
        CHECK(info.dock_direction ==
              directions[cycle % WXSIZEOF(directions)]);
        CHECK(info.IsShown() == ((cycle % 3) != 0));

        info.Show();
        manager->Update();
        REQUIRE(DrainUntil(
            [sentinel, &fixture]()
            {
                return HasSingleHostOwner(sentinel, fixture.GetFrameA()) &&
                       HostDoesNotContain(sentinel, fixture.GetFrameB());
            }));
    }

    REQUIRE(fixture.DestroyAndWait(before));
    before.CheckRestored();
}

TEST_CASE("WinUI AUI notebook preserves model focus and host ownership",
          "[aui][winui][advanced][notebook][WinUIAUI]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();

    OffscreenFramePair frames;
    REQUIRE(frames.Create("WinUI AUI notebook", 760));
    const long style = wxAUI_NB_DEFAULT_STYLE |
                       wxAUI_NB_TAB_EXTERNAL_MOVE |
                       wxAUI_NB_WINDOWLIST_BUTTON |
                       wxAUI_NB_PIN_ON_ACTIVE_TAB;
    wxAuiNotebook * const book = new wxAuiNotebook(
        frames.GetFrameA(), wxID_ANY, wxPoint(0, 0), wxSize(700, 480), style);
    wxWeakRef<wxAuiNotebook> weakBook(book);

    const wxBitmapBundle pageBitmap = MakeTestBitmapBundle();
    REQUIRE(pageBitmap.IsOk());

    std::vector<wxWeakRef<wxPanel>> weakPages;
    std::vector<wxWeakRef<wxButton>> weakSentinels;
    const auto makePage = [&weakPages, &weakSentinels](
                              wxWindow *parent, const wxString& label)
    {
        wxPanel * const page = new wxPanel(parent, wxID_ANY);
        wxButton * const sentinel = new wxButton(page, wxID_ANY, label);
        wxBoxSizer * const sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(sentinel, 1, wxEXPAND | wxALL, 4);
        page->SetSizer(sizer);
        weakPages.emplace_back(page);
        weakSentinels.emplace_back(sentinel);
        return page;
    };

    wxPanel * const page0 = makePage(book, "notebook page zero");
    wxPanel * const page2 = makePage(book, "notebook page two");
    wxPanel * const page1 = makePage(book, "notebook page one");
    REQUIRE(book->AddPage(page0, "Zero", true, pageBitmap));
    REQUIRE(book->AddPage(page2, "Two", false, pageBitmap));
    REQUIRE(book->InsertPage(1, page1, "One", false, pageBitmap));
    REQUIRE(book->GetPageCount() == 3);
    CHECK(book->GetPage(0) == page0);
    CHECK(book->GetPage(1) == page1);
    CHECK(book->GetPage(2) == page2);
    CHECK(book->GetPageBitmap(2).IsOk());
    wxBitmap replacementBitmap(20, 20, 32);
    REQUIRE(replacementBitmap.IsOk());
    REQUIRE(book->SetPageBitmap(
        2, wxBitmapBundle::FromBitmap(replacementBitmap)));
    CHECK(book->GetPageBitmap(2).GetSize() == wxSize(20, 20));
    bool vetoSelection = true;
    unsigned changingCount = 0;
    unsigned changedCount = 0;
    book->Bind(
        wxEVT_AUINOTEBOOK_PAGE_CHANGING,
        [&vetoSelection, &changingCount](wxAuiNotebookEvent& event)
        {
            ++changingCount;
            if ( vetoSelection )
                event.Veto();
            else
                event.Skip();
        });
    book->Bind(
        wxEVT_AUINOTEBOOK_PAGE_CHANGED,
        [&changedCount](wxAuiNotebookEvent& event)
        {
            ++changedCount;
            event.Skip();
        });

    const int initialSelection = book->GetSelection();
    book->SetSelection(2);
    CHECK(book->GetSelection() == initialSelection);
    CHECK(changingCount == 1);
    CHECK(changedCount == 0);
    vetoSelection = false;
    book->SetSelection(2);
    CHECK(book->GetSelection() == 2);
    CHECK(changingCount == 2);
    CHECK(changedCount == 1);

    wxAuiTabCtrl * const mainTabs = book->GetMainTabCtrl();
    REQUIRE(mainTabs);
    REQUIRE(mainTabs->MovePage(size_t(2), size_t(0)));
    const std::vector<size_t> movedOrder{2, 0, 1};
    CHECK(book->GetPagesInDisplayOrder(mainTabs) == movedOrder);
    REQUIRE(book->SetPageKind(0, wxAuiTabKind::Pinned));
    CHECK(book->GetPageKind(0) == wxAuiTabKind::Pinned);

    book->Split(0, wxLEFT);
    REQUIRE(book->GetAllTabCtrls().size() == 2);
    const wxAuiNotebookPosition splitPosition = book->GetPagePosition(0);
    CHECK(static_cast<bool>(splitPosition));
    CHECK(splitPosition.tabCtrl != mainTabs);
    book->UnsplitAll();
    CHECK(book->GetAllTabCtrls().size() == 1);

    bool vetoClose = true;
    unsigned closeCount = 0;
    book->Bind(
        wxEVT_AUINOTEBOOK_PAGE_CLOSE,
        [&vetoClose, &closeCount](wxAuiNotebookEvent& event)
        {
            ++closeCount;
            if ( vetoClose )
                event.Veto();
            else
                event.Skip();
        });
    wxAuiNotebookEvent closeEvent(
        wxEVT_AUINOTEBOOK_PAGE_CLOSE, book->GetId());
    closeEvent.SetEventObject(book);
    closeEvent.SetSelection(1);
    book->ProcessWindowEvent(closeEvent);
    CHECK(closeCount == 1);
    CHECK_FALSE(closeEvent.IsAllowed());
    CHECK(book->GetPageCount() == 3);

    wxWeakRef<wxPanel> deletedPage(page1);
    REQUIRE(book->DeletePage(1));
    REQUIRE(DrainUntil([&deletedPage]() { return !deletedPage; }));
    CHECK(book->GetPageCount() == 2);

    for ( unsigned i = 0; i < 32; ++i )
    {
        wxPanel * const page = makePage(
            book, wxString::Format("overflow sentinel %u", i));
        REQUIRE(book->AddPage(
            page,
            wxString::Format("Long overflow page number %02u", i),
            false));
    }
    book->SetSize(wxRect(wxPoint(0, 0), wxSize(330, 300)));
    book->Layout();
    DrainDispatch(4);
    wxAuiTabCtrl * const overflowTabs = book->GetMainTabCtrl();
    REQUIRE(overflowTabs);
    REQUIRE(overflowTabs->GetPageCount() > 20);
    overflowTabs->MakeTabVisible(
        static_cast<int>(overflowTabs->GetPageCount() - 1), overflowTabs);
    CHECK(overflowTabs->GetTabOffset() > 0);
    wxInfoDC tabDC(overflowTabs);
    CHECK(overflowTabs->IsTabVisible(
        static_cast<int>(overflowTabs->GetPageCount() - 1),
        static_cast<int>(overflowTabs->GetTabOffset()),
        &tabDC,
        overflowTabs));

    REQUIRE(book->Reparent(frames.GetFrameB()));
    book->SetSize(wxRect(wxPoint(0, 0), wxSize(650, 440)));
    book->Layout();
    REQUIRE(DrainUntil(
        [&weakSentinels, &frames]()
        {
            for ( const wxWeakRef<wxButton>& sentinel : weakSentinels )
            {
                if ( sentinel &&
                     (!HasSingleHostOwner(sentinel.get(), frames.GetFrameB()) ||
                      !HostDoesNotContain(sentinel.get(), frames.GetFrameA())) )
                {
                    return false;
                }
            }
            return true;
        }));

    REQUIRE(book->Reparent(frames.GetFrameA()));
    book->SetSize(wxRect(wxPoint(0, 0), wxSize(700, 480)));
    book->Layout();
    REQUIRE(DrainUntil(
        [&weakSentinels, &frames]()
        {
            for ( const wxWeakRef<wxButton>& sentinel : weakSentinels )
            {
                if ( sentinel &&
                     (!HasSingleHostOwner(sentinel.get(), frames.GetFrameA()) ||
                      !HostDoesNotContain(sentinel.get(), frames.GetFrameB())) )
                {
                    return false;
                }
            }
            return true;
        }));

    REQUIRE(book->GetPageCount() > 0);
    book->ChangeSelection(0);
    wxButton * const focusSentinel = weakSentinels.front().get();
    REQUIRE(focusSentinel);
    focusSentinel->SetFocus();
    REQUIRE(DrainUntil(
        [focusSentinel]()
        {
            return wxWindow::FindFocus() == focusSentinel;
        }));

    REQUIRE(book->DeleteAllPages());
    REQUIRE(DrainUntil(
        [&weakPages, &weakSentinels]()
        {
            for ( const wxWeakRef<wxPanel>& page : weakPages )
            {
                if ( page )
                    return false;
            }
            for ( const wxWeakRef<wxButton>& sentinel : weakSentinels )
            {
                if ( sentinel )
                    return false;
            }
            return true;
        }));
    book->Destroy();

    REQUIRE(frames.DestroyAndWait(
        before,
        [&weakBook]()
        {
            return !weakBook;
        }));
    before.CheckRestored();
}

TEST_CASE("WinUI AUI toolbar preserves tools orientation and host migration",
          "[aui][winui][advanced][toolbar][WinUIAUI]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();

    ToolBarFixture fixture;
    REQUIRE(fixture.Create(780));
    wxAuiToolBar * const toolbar = fixture.GetToolBar();
    RecordingAuiManager * const manager = fixture.GetManagerA();
    REQUIRE(toolbar);
    REQUIRE(manager);

    const wxBitmapBundle bundle = MakeTestBitmapBundle();
    REQUIRE(bundle.IsOk());
    REQUIRE(toolbar->AddTool(
        ID_AUI_TOOL_NORMAL, "Normal", bundle, "normal help", wxITEM_NORMAL));
    REQUIRE(toolbar->AddTool(
        ID_AUI_TOOL_CHECK, "Check", bundle, "check help", wxITEM_CHECK));
    REQUIRE(toolbar->AddTool(
        ID_AUI_TOOL_RADIO_A, "Radio A", bundle, "radio A", wxITEM_RADIO));
    REQUIRE(toolbar->AddTool(
        ID_AUI_TOOL_RADIO_B, "Radio B", bundle, "radio B", wxITEM_RADIO));
    REQUIRE(toolbar->AddTool(
        ID_AUI_TOOL_DROPDOWN,
        "Dropdown",
        bundle,
        "dropdown help",
        wxITEM_NORMAL));
    toolbar->SetToolDropDown(ID_AUI_TOOL_DROPDOWN, true);

    wxTextCtrl * const temporary = new wxTextCtrl(
        toolbar, ID_AUI_TOOL_TEMPORARY, "temporary control");
    wxWeakRef<wxTextCtrl> weakTemporary(temporary);
    REQUIRE(toolbar->AddControl(temporary, "Temporary"));
    wxButton * const sentinel = new wxButton(
        toolbar, ID_AUI_TOOL_CONTROL, "WinUI toolbar sentinel");
    wxWeakRef<wxButton> weakSentinel(sentinel);
    REQUIRE(toolbar->AddControl(sentinel, "Sentinel"));
    REQUIRE(toolbar->AddSeparator());
    REQUIRE(toolbar->AddSpacer(9));
    REQUIRE(toolbar->AddStretchSpacer(1));
    REQUIRE(toolbar->Realize());

    REQUIRE(toolbar->FindTool(ID_AUI_TOOL_NORMAL));
    CHECK(toolbar->FindTool(ID_AUI_TOOL_NORMAL)->GetKind() == wxITEM_NORMAL);
    CHECK(toolbar->FindTool(ID_AUI_TOOL_CHECK)->GetKind() == wxITEM_CHECK);
    CHECK(toolbar->FindTool(ID_AUI_TOOL_RADIO_A)->GetKind() == wxITEM_RADIO);
    CHECK(toolbar->FindTool(ID_AUI_TOOL_NORMAL)
              ->GetBitmapBundle()
              .IsOk());
    CHECK(toolbar->GetToolDropDown(ID_AUI_TOOL_DROPDOWN));
    CHECK(toolbar->GetOverflowVisible());

    toolbar->EnableTool(ID_AUI_TOOL_NORMAL, false);
    CHECK_FALSE(toolbar->GetToolEnabled(ID_AUI_TOOL_NORMAL));
    toolbar->EnableTool(ID_AUI_TOOL_NORMAL, true);
    CHECK(toolbar->GetToolEnabled(ID_AUI_TOOL_NORMAL));
    toolbar->ToggleTool(ID_AUI_TOOL_CHECK, true);
    CHECK(toolbar->GetToolToggled(ID_AUI_TOOL_CHECK));
    toolbar->ToggleTool(ID_AUI_TOOL_RADIO_A, true);
    toolbar->ToggleTool(ID_AUI_TOOL_RADIO_B, true);
    CHECK_FALSE(toolbar->GetToolToggled(ID_AUI_TOOL_RADIO_A));
    CHECK(toolbar->GetToolToggled(ID_AUI_TOOL_RADIO_B));

    const size_t countBeforeDelete = toolbar->GetToolCount();
    REQUIRE(toolbar->DeleteTool(ID_AUI_TOOL_NORMAL));
    CHECK(toolbar->GetToolCount() == countBeforeDelete - 1);
    REQUIRE(toolbar->AddTool(
        ID_AUI_TOOL_NORMAL, "Normal restored", bundle,
        "normal help", wxITEM_NORMAL));
    REQUIRE(toolbar->Realize());
    REQUIRE(toolbar->DestroyTool(ID_AUI_TOOL_TEMPORARY));
    REQUIRE(DrainUntil([&weakTemporary]() { return !weakTemporary; }));
    CHECK(toolbar->FindControl(ID_AUI_TOOL_TEMPORARY) == nullptr);

    unsigned auiEventCount = 0;
    const auto dispatchAuiEvent =
        [toolbar, &auiEventCount](
            const wxEventTypeTag<wxAuiToolBarEvent>& type)
    {
        toolbar->Bind(
            type,
            [&auiEventCount](wxAuiToolBarEvent& event)
            {
                ++auiEventCount;
                CHECK(event.GetToolId() == ID_AUI_TOOL_DROPDOWN);
            });
        wxAuiToolBarEvent event(type, toolbar->GetId());
        event.SetEventObject(toolbar);
        event.SetToolId(ID_AUI_TOOL_DROPDOWN);
        event.SetItemRect(toolbar->GetToolRect(ID_AUI_TOOL_DROPDOWN));
        toolbar->ProcessWindowEvent(event);
    };
    dispatchAuiEvent(wxEVT_AUITOOLBAR_TOOL_DROPDOWN);
    dispatchAuiEvent(wxEVT_AUITOOLBAR_OVERFLOW_CLICK);
    dispatchAuiEvent(wxEVT_AUITOOLBAR_RIGHT_CLICK);
    dispatchAuiEvent(wxEVT_AUITOOLBAR_MIDDLE_CLICK);
    dispatchAuiEvent(wxEVT_AUITOOLBAR_BEGIN_DRAG);
    CHECK(auiEventCount == 5);

    unsigned menuEventCount = 0;
    toolbar->Bind(
        wxEVT_MENU,
        [&menuEventCount](wxCommandEvent& event)
        {
            ++menuEventCount;
            CHECK(event.GetId() == ID_AUI_TOOL_NORMAL);
        },
        ID_AUI_TOOL_NORMAL);
    wxCommandEvent menuEvent(wxEVT_MENU, ID_AUI_TOOL_NORMAL);
    menuEvent.SetEventObject(toolbar);
    toolbar->ProcessWindowEvent(menuEvent);
    CHECK(menuEventCount == 1);

    wxAuiPaneInfo& toolbarPane = manager->GetPane(toolbar);
    toolbarPane.Direction(wxAUI_DOCK_LEFT);
    manager->Update();
    REQUIRE(DrainUntil(
        [toolbar]()
        {
            return (toolbar->GetArtProvider()->GetFlags() &
                    wxAUI_TB_VERTICAL) != 0;
        }));
    const wxSize verticalHint = toolbar->GetHintSize(wxAUI_DOCK_LEFT);
    CHECK(verticalHint.x > 0);
    CHECK(verticalHint.y > 0);

    toolbarPane.Direction(wxAUI_DOCK_TOP);
    manager->Update();
    REQUIRE(DrainUntil(
        [toolbar]()
        {
            return (toolbar->GetArtProvider()->GetFlags() &
                    wxAUI_TB_VERTICAL) == 0;
        }));
    const wxSize horizontalHint = toolbar->GetHintSize(wxAUI_DOCK_TOP);
    CHECK(horizontalHint.x > 0);
    CHECK(horizontalHint.y > 0);

    toolbarPane
        .Float()
        .FloatingPosition(-29200, -29200)
        .FloatingSize(520, 180);
    manager->Update();
    REQUIRE(DrainUntil(
        [toolbar, sentinel, manager, &fixture]()
        {
            wxAuiFloatingFrame * const floating =
                manager->GetLastFloatingFrame();
            return floating && toolbar->GetParent() == floating &&
                   HasSingleHostOwner(sentinel, floating) &&
                   HostDoesNotContain(sentinel, fixture.GetFrameA()) &&
                   HostDoesNotContain(sentinel, fixture.GetFrameB());
        }));
    wxWeakRef<wxAuiFloatingFrame> weakFloating(
        manager->GetLastFloatingFrame());
    manager->GetPane(toolbar).Dock().Top();
    manager->Update();
    REQUIRE(DrainUntil(
        [sentinel, &weakFloating, &fixture]()
        {
            return !weakFloating &&
                   HasSingleHostOwner(sentinel, fixture.GetFrameA()) &&
                   HostDoesNotContain(sentinel, fixture.GetFrameB());
        }));

    REQUIRE(manager->DetachPane(toolbar));
    manager->Update();
    REQUIRE(toolbar->Reparent(fixture.GetFrameB()));
    REQUIRE(fixture.GetManagerB()->AddPane(
        toolbar,
        wxAuiPaneInfo().Name("aui-toolbar-b").ToolbarPane().Top()));
    fixture.GetManagerB()->Update();
    REQUIRE(DrainUntil(
        [sentinel, &fixture]()
        {
            return HasSingleHostOwner(sentinel, fixture.GetFrameB()) &&
                   HostDoesNotContain(sentinel, fixture.GetFrameA());
        }));

    REQUIRE(fixture.GetManagerB()->DetachPane(toolbar));
    fixture.GetManagerB()->Update();
    REQUIRE(toolbar->Reparent(fixture.GetFrameA()));
    REQUIRE(manager->AddPane(
        toolbar,
        wxAuiPaneInfo().Name("aui-toolbar").ToolbarPane().Top()));
    manager->Update();
    REQUIRE(DrainUntil(
        [sentinel, &fixture]()
        {
            return HasSingleHostOwner(sentinel, fixture.GetFrameA()) &&
                   HostDoesNotContain(sentinel, fixture.GetFrameB());
        }));

    REQUIRE(fixture.DestroyAndWait(before));
    CHECK_FALSE(weakSentinel);
    before.CheckRestored();
}

#if wxUSE_MDI
TEST_CASE("WinUI AUI MDI exercises the stable GDI model lifecycle",
          "[aui][winui][advanced][mdi][WinUIAUI]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();

    MdiFixture fixture;
    REQUIRE(fixture.Create(800));
    wxAuiMDIParentFrame * const parentA = fixture.GetParentA();
    wxAuiMDIParentFrame * const parentB = fixture.GetParentB();
    REQUIRE(parentA);
    REQUIRE(parentB);
    REQUIRE(parentA->GetNotebook());
    REQUIRE(parentB->GetNotebook());

    wxAuiMDIChildFrame * const childOne = new wxAuiMDIChildFrame(
        parentA, wxID_ANY, "Document One");
    wxAuiMDIChildFrame * const childTwo = new wxAuiMDIChildFrame(
        parentA, wxID_ANY, "Document Two");
    wxWeakRef<wxAuiMDIChildFrame> weakChildOne(childOne);
    wxWeakRef<wxAuiMDIChildFrame> weakChildTwo(childTwo);
    GdiProbePanel * const probeOne = new GdiProbePanel(childOne);
    GdiProbePanel * const probeTwo = new GdiProbePanel(childTwo);
    wxWeakRef<GdiProbePanel> weakProbeOne(probeOne);
    wxWeakRef<GdiProbePanel> weakProbeTwo(probeTwo);
    REQUIRE(probeOne->GetParent() == childOne);
    REQUIRE(probeTwo->GetParent() == childTwo);

    unsigned activatedOne = 0;
    unsigned deactivatedOne = 0;
    childOne->Bind(
        wxEVT_ACTIVATE,
        [&activatedOne, &deactivatedOne](wxActivateEvent& event)
        {
            if ( event.GetActive() )
                ++activatedOne;
            else
                ++deactivatedOne;
            event.Skip();
        });

    childOne->Activate();
    CHECK(parentA->GetActiveChild() == childOne);
    childTwo->Activate();
    CHECK(parentA->GetActiveChild() == childTwo);
    parentA->ActivatePrevious();
    CHECK(parentA->GetActiveChild() == childOne);
    parentA->ActivateNext();
    CHECK(parentA->GetActiveChild() == childTwo);
    CHECK(activatedOne > 0);
    CHECK(deactivatedOne > 0);

#if wxUSE_MENUS
    unsigned routedToOne = 0;
    unsigned routedToTwo = 0;
    childOne->Bind(
        wxEVT_MENU,
        [&routedToOne](wxCommandEvent&) { ++routedToOne; },
        ID_AUI_MDI_COMMAND);
    childTwo->Bind(
        wxEVT_MENU,
        [&routedToTwo](wxCommandEvent&) { ++routedToTwo; },
        ID_AUI_MDI_COMMAND);
    childOne->Activate();
    wxCommandEvent commandOne(wxEVT_MENU, ID_AUI_MDI_COMMAND);
    commandOne.SetEventObject(parentA);
    parentA->ProcessWindowEvent(commandOne);
    CHECK(routedToOne == 1);
    CHECK(routedToTwo == 0);
    childTwo->Activate();
    wxCommandEvent commandTwo(wxEVT_MENU, ID_AUI_MDI_COMMAND);
    commandTwo.SetEventObject(parentA);
    parentA->ProcessWindowEvent(commandTwo);
    CHECK(routedToOne == 1);
    CHECK(routedToTwo == 1);
#endif

    bool vetoChildClose = true;
    childOne->Bind(
        wxEVT_CLOSE_WINDOW,
        [&vetoChildClose](wxCloseEvent& event)
        {
            if ( vetoChildClose )
                event.Veto();
            else
                event.Skip();
        });
    childOne->Activate();
    CHECK_FALSE(parentA->Close());
    CHECK(weakChildOne);
    CHECK(weakChildTwo);
    vetoChildClose = false;
    REQUIRE(childOne->Close());
    REQUIRE(DrainUntil(
        [&weakChildOne, &weakProbeOne]()
        {
            return !weakChildOne && !weakProbeOne;
        }));

    const unsigned steadySlots =
        wxWinUITopLevelHost::GetLiveSlotCount();
    for ( unsigned cycle = 0; cycle < 100; ++cycle )
    {
        wxAuiMDIChildFrame * const child = new wxAuiMDIChildFrame(
            parentB,
            wxID_ANY,
            wxString::Format("Stress document %u", cycle),
            wxDefaultPosition,
            wxDefaultSize,
            wxMINIMIZE);
        wxWeakRef<wxAuiMDIChildFrame> weakChild(child);
        GdiProbePanel * const probe = new GdiProbePanel(child);
        wxWeakRef<GdiProbePanel> weakProbe(probe);
        REQUIRE(probe->GetParent() == child);
        CHECK(parentB->GetNotebook()->GetPageIndex(child) != wxNOT_FOUND);
        REQUIRE(child->Destroy());
        REQUIRE(DrainUntil(
            [&weakChild, &weakProbe, steadySlots]()
            {
                return !weakChild && !weakProbe &&
                       wxWinUITopLevelHost::GetLiveSlotCount() == steadySlots;
            }));
    }

    // AUI MDI exposes no atomic inter-parent transfer. Two independent TLWs
    // are qualified above; cross-parent reparent remains an explicit STOP in
    // the plan document instead of being simulated with SetMDIParentFrame().
    wxAuiMDIChildFrame * const childB = new wxAuiMDIChildFrame(
        parentB, wxID_ANY, "Independent document B");
    wxWeakRef<wxAuiMDIChildFrame> weakChildB(childB);
    GdiProbePanel * const probeB = new GdiProbePanel(childB);
    wxWeakRef<GdiProbePanel> weakProbeB(probeB);
    REQUIRE(probeB->GetParent() == childB);
    CHECK(parentA->GetNotebook()->GetPageIndex(childB) == wxNOT_FOUND);
    CHECK(parentB->GetNotebook()->GetPageIndex(childB) != wxNOT_FOUND);

    // The generic parent destructor deletes its notebook before all MDI child
    // destructors can safely call back into that notebook.  Keep the stable
    // automatic model gate honest by closing every child explicitly; the
    // parent-with-live-children reproducer is documented as blocked below.
    REQUIRE(childTwo->Destroy());
    REQUIRE(childB->Destroy());
    REQUIRE(DrainUntil(
        [&weakChildTwo, &weakProbeTwo, &weakChildB, &weakProbeB]()
        {
            return !weakChildTwo && !weakProbeTwo &&
                   !weakChildB && !weakProbeB;
        }));

    REQUIRE(fixture.DestroyAndWait(before));
    before.CheckRestored(false);
}
#endif // wxUSE_MDI

// ----------------------------------------------------------------------------
// The Fluent tool bar art provider
// ----------------------------------------------------------------------------

TEST_CASE("WinUI AUI toolbar uses the Fluent art provider",
          "[aui][winui][toolbar][art][WinUIAUI]")
{
    REQUIRE(DrainToQuiescence());
    const RuntimeSnapshot before = RuntimeSnapshot::Capture();

    ToolBarFixture fixture;
    REQUIRE(fixture.Create(420));
    wxAuiToolBar * const toolbar = fixture.GetToolBar();
    REQUIRE(toolbar);

    // A tool bar under this port must not fall back to the uxtheme drawing of
    // wxAuiMSWToolBarArt: that is Windows 7 chrome inside a WinUI window.
    wxAuiToolBarArt * const art = toolbar->GetArtProvider();
    REQUIRE(art);
    CHECK(dynamic_cast<wxAuiWinUIToolBarArt *>(art) != nullptr);

    // Cloning is how a tool bar hands its art to a floating pane, so the clone
    // has to stay Fluent too.
    std::unique_ptr<wxAuiToolBarArt> clone(art->Clone());
    REQUIRE(clone);
    CHECK(dynamic_cast<wxAuiWinUIToolBarArt *>(clone.get()) != nullptr);

    SECTION("a tool is at least a touch target")
    {
        wxAuiToolBarItem item;
        item.SetKind(wxITEM_NORMAL);
        item.SetLabel("Tool");

        wxClientDC dc(toolbar);
        const wxSize size = art->GetToolSize(dc, toolbar, item);
        const wxSize minimum = toolbar->FromDIP(wxSize(32, 32));
        CHECK(size.x >= minimum.x);
        CHECK(size.y >= minimum.y);
    }

    SECTION("the rest state paints nothing over the bar")
    {
        // The defining property of a command bar: a tool which is neither
        // hovered nor pressed nor checked leaves the bar showing through,
        // while a hovered one does not.
        const wxSize size(60, 40);
        wxBitmap canvas(size);
        wxMemoryDC dc(canvas);
        dc.SetBackground(*wxRED);
        dc.Clear();

        wxAuiToolBarItem item;
        item.SetKind(wxITEM_NORMAL);

        const wxRect rect(0, 0, size.x, size.y);

        // Read back through a device context rather than raw access: the
        // bitmap depth here is whatever the display uses, which the raw
        // accessors are picky about, and one pixel is all this needs.
        const auto sampleLeftEdge = [&size](wxBitmap& bitmap)
        {
            wxMemoryDC reader(bitmap);
            wxColour sampled;
            reader.GetPixel(3, size.y / 2, &sampled);
            reader.SelectObject(wxNullBitmap);
            return sampled;
        };

        art->DrawButton(dc, toolbar, item, rect);
        dc.SelectObject(wxNullBitmap);
        CHECK(sampleLeftEdge(canvas) == *wxRED);

        {
            wxMemoryDC hoverDC(canvas);
            item.SetState(wxAUI_BUTTON_STATE_HOVER);
            art->DrawButton(hoverDC, toolbar, item, rect);
            hoverDC.SelectObject(wxNullBitmap);
        }
        CHECK(sampleLeftEdge(canvas) != *wxRED);
    }

    REQUIRE(fixture.DestroyAndWait(before));
    before.CheckRestored(false);
}

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_AUI
