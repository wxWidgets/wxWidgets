///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuiadvancedharness.cpp
// Purpose:     shared WinUI hybrid reparenting harness for advanced controls
// Author:      wxWidgets development team
// Created:     2026-08-08
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_AUI

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/button.h"
    #include "wx/frame.h"
    #include "wx/panel.h"
    #include "wx/sizer.h"
    #include "wx/textctrl.h"
    #include "wx/utils.h"
#endif // WX_PRECOMP

#include "wx/aui/floatpane.h"
#include "wx/aui/framemanager.h"
#include "wx/msw/wrapwin.h"
#include "wx/weakref.h"
#include "wx/winui/private/tlwhost.h"

#if wxUSE_TREECTRL
    #include "wx/treectrl.h"
#endif
#if wxUSE_GRID
    #include "wx/grid.h"
#endif
#if wxUSE_RICHTEXT
    #include "wx/richtext/richtextctrl.h"
#endif
#if wxUSE_STC
    #include "wx/stc/stc.h"
#endif
#if wxUSE_HTML
    #include "wx/html/htmlwin.h"
#endif

#include <memory>
#include <vector>

namespace
{

// Keep every native TLW used by this automatic harness outside the virtual
// desktop and out of Alt-Tab. Physical pointer, IME and rendering checks are
// deliberately left to the manual part of plan 014.
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

struct HostSnapshot
{
    static HostSnapshot Capture()
    {
        HostSnapshot snapshot;
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
                       rootHandlerRevokes;
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
};

enum class PayloadKind
{
    Text,
#if wxUSE_TREECTRL
    Tree,
#endif
#if wxUSE_GRID
    Grid,
#endif
#if wxUSE_RICHTEXT
    RichText,
#endif
#if wxUSE_STC
    StyledText,
#endif
#if wxUSE_HTML
    Html,
#endif
};

const char *GetPayloadName(PayloadKind kind)
{
    switch ( kind )
    {
        case PayloadKind::Text:
            return "TextCtrl";
#if wxUSE_TREECTRL
        case PayloadKind::Tree:
            return "TreeCtrl";
#endif
#if wxUSE_GRID
        case PayloadKind::Grid:
            return "Grid";
#endif
#if wxUSE_RICHTEXT
        case PayloadKind::RichText:
            return "RichTextCtrl";
#endif
#if wxUSE_STC
        case PayloadKind::StyledText:
            return "StyledTextCtrl";
#endif
#if wxUSE_HTML
        case PayloadKind::Html:
            return "HtmlWindow";
#endif
    }

    return "unknown";
}

std::vector<PayloadKind> GetPayloadKinds()
{
    std::vector<PayloadKind> kinds{PayloadKind::Text};
#if wxUSE_TREECTRL
    kinds.push_back(PayloadKind::Tree);
#endif
#if wxUSE_GRID
    kinds.push_back(PayloadKind::Grid);
#endif
#if wxUSE_RICHTEXT
    kinds.push_back(PayloadKind::RichText);
#endif
#if wxUSE_STC
    kinds.push_back(PayloadKind::StyledText);
#endif
#if wxUSE_HTML
    kinds.push_back(PayloadKind::Html);
#endif
    return kinds;
}

wxWindow *CreatePayloadWindow(wxWindow *parent, PayloadKind kind)
{
    switch ( kind )
    {
        case PayloadKind::Text:
            return new wxTextCtrl(
                parent, wxID_ANY, "stable hybrid payload",
                wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);

#if wxUSE_TREECTRL
        case PayloadKind::Tree:
        {
            wxTreeCtrl * const tree = new wxTreeCtrl(parent, wxID_ANY);
            const wxTreeItemId root = tree->AddRoot("root");
            tree->AppendItem(root, "child");
            tree->Expand(root);
            return tree;
        }
#endif

#if wxUSE_GRID
        case PayloadKind::Grid:
        {
            wxGrid * const grid = new wxGrid(parent, wxID_ANY);
            grid->CreateGrid(3, 3);
            grid->SetCellValue(0, 0, "hybrid");
            return grid;
        }
#endif

#if wxUSE_RICHTEXT
        case PayloadKind::RichText:
            return new wxRichTextCtrl(
                parent, wxID_ANY, "rich hybrid payload",
                wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
#endif

#if wxUSE_STC
        case PayloadKind::StyledText:
        {
            wxStyledTextCtrl * const stc =
                new wxStyledTextCtrl(parent, wxID_ANY);
            stc->SetText("styled hybrid payload\n");
            return stc;
        }
#endif

#if wxUSE_HTML
        case PayloadKind::Html:
        {
            wxHtmlWindow * const html = new wxHtmlWindow(parent, wxID_ANY);
            html->SetPage(
                "<html><body><p>local hybrid payload</p></body></html>");
            return html;
        }
#endif
    }

    return nullptr;
}

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

        // wxAuiManager shows the frame after this callback. Apply the native
        // no-activation policy now, without creating another host or island.
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
    const int m_offscreenOrdinal;
    wxWeakRef<wxAuiFloatingFrame> m_lastFloatingFrame;
    unsigned m_floatingFrameCount = 0;
};

wxAuiPaneInfo MakePaneInfo(const wxString& name)
{
    return wxAuiPaneInfo()
        .Name(name)
        .Caption(name)
        .CenterPane()
        .CaptionVisible()
        .Floatable()
        .Movable()
        .CloseButton(false);
}

class HybridHarnessFixture final
{
public:
    HybridHarnessFixture() = default;

    ~HybridHarnessFixture()
    {
        Cleanup();
        DrainDispatch();
    }

    bool Create(PayloadKind kind, int ordinal)
    {
        wxFrame * const frameA = new wxFrame(
            nullptr, wxID_ANY, "WinUI advanced harness A",
            wxDefaultPosition, wxSize(520, 360));
        wxFrame * const frameB = new wxFrame(
            nullptr, wxID_ANY, "WinUI advanced harness B",
            wxDefaultPosition, wxSize(520, 360));
        m_frameA = wxWeakRef<wxFrame>(frameA);
        m_frameB = wxWeakRef<wxFrame>(frameB);

        if ( !frameA->GetHWND() || !frameB->GetHWND() )
            return false;

        m_managerA = std::make_unique<RecordingAuiManager>(frameA,
                                                           ordinal + 2);
        m_managerB = std::make_unique<RecordingAuiManager>(frameB,
                                                           ordinal + 3);

        wxPanel * const pane = new wxPanel(frameA, wxID_ANY);
        wxWindow * const content = CreatePayloadWindow(pane, kind);
        if ( !content )
            return false;

        wxButton * const sentinel = new wxButton(
            pane, wxID_ANY,
            "WinUI slot sentinel " + wxString::FromUTF8(GetPayloadName(kind)));
        wxBoxSizer * const sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(content, 1, wxEXPAND | wxALL, 4);
        sizer->Add(sentinel, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 4);
        pane->SetSizer(sizer);

        m_pane = wxWeakRef<wxPanel>(pane);
        m_content = wxWeakRef<wxWindow>(content);
        m_sentinel = wxWeakRef<wxButton>(sentinel);

        return ShowOffscreenWithoutActivation(frameA, ordinal) &&
               ShowOffscreenWithoutActivation(frameB, ordinal + 1);
    }

    bool AddInitiallyToA()
    {
        wxPanel * const pane = GetPane();
        if ( !pane || !m_managerA ||
             !m_managerA->AddPane(pane, MakePaneInfo("hybrid-A")) )
        {
            return false;
        }

        m_managerA->Update();
        return WaitForSingleOwner(GetFrameA());
    }

    bool FloatFromA()
    {
        wxPanel * const pane = GetPane();
        if ( !pane || !m_managerA )
            return false;

        wxAuiPaneInfo& paneInfo = m_managerA->GetPane(pane);
        if ( !paneInfo.IsOk() )
            return false;

        paneInfo.Float()
            .FloatingPosition(-31200, -31600)
            .FloatingSize(380, 260);
        m_managerA->Update();

        return DrainUntil(
            [this, pane]()
            {
                wxAuiFloatingFrame * const floating =
                    m_managerA->GetLastFloatingFrame();
                return floating && pane->GetParent() == floating &&
                       HasSingleOwner(floating);
            });
    }

    bool RedockInA()
    {
        wxPanel * const pane = GetPane();
        if ( !pane || !m_managerA )
            return false;

        wxAuiPaneInfo& paneInfo = m_managerA->GetPane(pane);
        if ( !paneInfo.IsOk() )
            return false;

        paneInfo.Dock().Center();
        m_managerA->Update();

        return DrainUntil(
            [this, pane]()
            {
                return !m_managerA->GetLastFloatingFrame() &&
                       pane->GetParent() == GetFrameA() &&
                       HasSingleOwner(GetFrameA());
            });
    }

    bool MoveAToB()
    {
        return MovePane(*m_managerA, *m_managerB,
                        GetFrameB(), "hybrid-B");
    }

    bool MoveBToA()
    {
        return MovePane(*m_managerB, *m_managerA,
                        GetFrameA(), "hybrid-A");
    }

    bool HasSingleOwner(wxWindow *expectedTLW) const
    {
        wxButton * const sentinel = GetSentinel();
        if ( !sentinel || !expectedTLW )
            return false;

        wxWinUITopLevelHost * const expectedHost =
            wxWinUITopLevelHost::FindForTLW(expectedTLW);
        if ( !expectedHost ||
             wxWinUITopLevelHost::FindSlotOwner(sentinel) != expectedHost ||
             !expectedHost->FindSlot(sentinel) )
        {
            return false;
        }

        wxWindow * const roots[] =
        {
            GetFrameA(),
            GetFrameB(),
            m_managerA ? m_managerA->GetLastFloatingFrame() : nullptr
        };
        for ( wxWindow * const root : roots )
        {
            if ( !root || root == expectedTLW )
                continue;
            wxWinUITopLevelHost * const otherHost =
                wxWinUITopLevelHost::FindForTLW(root);
            if ( otherHost && otherHost->FindSlot(sentinel) )
                return false;
        }

        return true;
    }

    bool DestroyAndWait(const HostSnapshot& before)
    {
        Cleanup();
        if ( !DrainUntil(
                [this, &before]()
                {
                    return !m_frameA && !m_frameB && !m_pane &&
                           !m_content && !m_sentinel && before.IsRestored();
                }) )
        {
            return false;
        }

        // A stale coalesced flush is allowed to observe the dead generation,
        // but it must drain before this fixture lends its baseline to the next
        // payload section.
        return DrainToQuiescence() && before.IsRestored();
    }

    wxFrame *GetFrameA() const { return m_frameA.get(); }
    wxFrame *GetFrameB() const { return m_frameB.get(); }
    wxPanel *GetPane() const { return m_pane.get(); }
    wxWindow *GetContent() const { return m_content.get(); }
    wxButton *GetSentinel() const { return m_sentinel.get(); }
    RecordingAuiManager *GetManagerA() const { return m_managerA.get(); }
    RecordingAuiManager *GetManagerB() const { return m_managerB.get(); }

private:
    bool WaitForSingleOwner(wxWindow *expectedTLW)
    {
        return DrainUntil(
            [this, expectedTLW]() { return HasSingleOwner(expectedTLW); });
    }

    bool MovePane(RecordingAuiManager& source,
                  RecordingAuiManager& destination,
                  wxFrame *destinationFrame,
                  const wxString& paneName)
    {
        wxPanel * const pane = GetPane();
        if ( !pane || !destinationFrame || !source.DetachPane(pane) )
            return false;

        source.Update();
        if ( !pane->Reparent(destinationFrame) ||
             !destination.AddPane(pane, MakePaneInfo(paneName)) )
        {
            return false;
        }

        destination.Update();
        return WaitForSingleOwner(destinationFrame);
    }

    void Cleanup()
    {
        if ( m_cleaned )
            return;
        m_cleaned = true;

        if ( m_managerA )
        {
            if ( wxAuiFloatingFrame * const floating =
                    m_managerA->GetLastFloatingFrame() )
            {
                if ( !floating->IsBeingDeleted() )
                    floating->Destroy();
            }
            m_managerA->UnInit();
        }
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
    wxWeakRef<wxPanel> m_pane;
    wxWeakRef<wxWindow> m_content;
    wxWeakRef<wxButton> m_sentinel;
    std::unique_ptr<RecordingAuiManager> m_managerA;
    std::unique_ptr<RecordingAuiManager> m_managerB;
};

} // anonymous namespace

TEST_CASE("WinUIAdvancedHarness::PayloadTransitions",
          "[winui-advanced][winui-aui]")
{
    const std::vector<PayloadKind> kinds = GetPayloadKinds();
    for ( std::size_t i = 0; i < kinds.size(); ++i )
    {
        const PayloadKind kind = kinds[i];
        DYNAMIC_SECTION(GetPayloadName(kind))
        {
            REQUIRE(DrainToQuiescence());
            const HostSnapshot before = HostSnapshot::Capture();

            HybridHarnessFixture fixture;
            REQUIRE(fixture.Create(kind, static_cast<int>(i) * 4));
            REQUIRE(fixture.GetPane());
            REQUIRE(fixture.GetContent());
            REQUIRE(fixture.GetSentinel());

            REQUIRE(fixture.AddInitiallyToA());
            CHECK(fixture.GetManagerA()->GetPane(fixture.GetPane()).IsDocked());
            CHECK(fixture.HasSingleOwner(fixture.GetFrameA()));

            REQUIRE(fixture.FloatFromA());
            REQUIRE(fixture.GetManagerA()->GetLastFloatingFrame());
            CHECK(fixture.GetManagerA()->GetFloatingFrameCount() == 1);
            CHECK(fixture.GetManagerA()->GetPane(fixture.GetPane()).IsFloating());
            CHECK(fixture.HasSingleOwner(
                fixture.GetManagerA()->GetLastFloatingFrame()));

            REQUIRE(fixture.RedockInA());
            CHECK(fixture.GetManagerA()->GetPane(fixture.GetPane()).IsDocked());
            CHECK(fixture.HasSingleOwner(fixture.GetFrameA()));

            REQUIRE(fixture.MoveAToB());
            CHECK_FALSE(
                fixture.GetManagerA()->GetPane(fixture.GetPane()).IsOk());
            CHECK(fixture.GetManagerB()->GetPane(fixture.GetPane()).IsOk());
            CHECK(fixture.HasSingleOwner(fixture.GetFrameB()));

            REQUIRE(fixture.MoveBToA());
            CHECK(fixture.GetManagerA()->GetPane(fixture.GetPane()).IsOk());
            CHECK_FALSE(
                fixture.GetManagerB()->GetPane(fixture.GetPane()).IsOk());
            CHECK(fixture.HasSingleOwner(fixture.GetFrameA()));

            REQUIRE(fixture.DestroyAndWait(before));
            before.CheckRestored();
        }
    }
}

TEST_CASE("WinUIAdvancedHarness::CrossManagerStress",
          "[winui-advanced][winui-aui][stress]")
{
    REQUIRE(DrainToQuiescence());
    const HostSnapshot before = HostSnapshot::Capture();

    HybridHarnessFixture fixture;
    REQUIRE(fixture.Create(PayloadKind::Text, 40));
    REQUIRE(fixture.AddInitiallyToA());

    // Keep the expensive float construction in the matrix test above. This
    // loop isolates 100 complete A -> B -> A manager/host transactions, which
    // is the reusable stress primitive needed by the richer payloads.
    for ( unsigned cycle = 0; cycle < 100; ++cycle )
    {
        CAPTURE(cycle);
        REQUIRE(fixture.MoveAToB());
        fixture.GetSentinel()->SetLabel(
            wxString::Format("hybrid cycle %u B", cycle));
        CHECK(fixture.HasSingleOwner(fixture.GetFrameB()));

        REQUIRE(fixture.MoveBToA());
        fixture.GetSentinel()->SetLabel(
            wxString::Format("hybrid cycle %u A", cycle));
        CHECK(fixture.HasSingleOwner(fixture.GetFrameA()));
    }

    REQUIRE(fixture.DestroyAndWait(before));
    before.CheckRestored();
}

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_AUI
