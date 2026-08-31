///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuiribbon.cpp
// Purpose:     WinUI generic ribbon public-contract and host-lifecycle tests
// Author:      wxWidgets development team
// Created:     2026-08-08
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_RIBBON

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/bitmap.h"
    #include "wx/button.h"
    #include "wx/frame.h"
    #include "wx/sizer.h"
    #include "wx/utils.h"
#endif // WX_PRECOMP

#include "wx/msw/wrapwin.h"
#include "wx/ribbon/art.h"
#include "wx/ribbon/bar.h"
#include "wx/ribbon/buttonbar.h"
#include "wx/ribbon/gallery.h"
#include "wx/ribbon/page.h"
#include "wx/ribbon/panel.h"
#include "wx/ribbon/toolbar.h"
#include "wx/weakref.h"
#include "wx/winui/private/tlwhost.h"
#include "wx/winui/winui.h"

#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>

#if wxUSE_XRC
    #include "wx/sstream.h"
    #include "wx/xml/xml.h"
    #include "wx/xrc/xh_ribbon.h"
    #include "wx/xrc/xmlres.h"
#endif // wxUSE_XRC

#include <algorithm>
#include <array>
#include <cstdint>
#include <iterator>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace
{

enum
{
    ID_RIBBON_BUTTON_NORMAL = wxID_HIGHEST + 4100,
    ID_RIBBON_BUTTON_DROPDOWN,
    ID_RIBBON_BUTTON_HYBRID,
    ID_RIBBON_BUTTON_TOGGLE,
    ID_RIBBON_BUTTON_TEMPORARY,
    ID_RIBBON_TOOL_NORMAL,
    ID_RIBBON_TOOL_DROPDOWN,
    ID_RIBBON_TOOL_HYBRID,
    ID_RIBBON_TOOL_TOGGLE,
    ID_RIBBON_GALLERY_FIRST,
    ID_RIBBON_GALLERY_SECOND,
    ID_RIBBON_GALLERY_THIRD,
    ID_RIBBON_GALLERY_REPLACEMENT
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

wxRibbonPage *CreatePage(wxRibbonBar *bar,
                         const wxString& label,
                         bool useDefaultCreate)
{
    if ( !useDefaultCreate )
        return new wxRibbonPage(bar, wxID_ANY, label);

    wxRibbonPage * const page = new wxRibbonPage;
    if ( page->Create(bar, wxID_ANY, label) )
        return page;

    delete page;
    return nullptr;
}

wxRibbonPanel *CreatePanel(wxRibbonPage *page,
                           const wxString& label,
                           bool useDefaultCreate,
                           long style = wxRIBBON_PANEL_DEFAULT_STYLE)
{
    if ( !useDefaultCreate )
    {
        return new wxRibbonPanel(page, wxID_ANY, label, wxBitmapBundle(),
                                 wxDefaultPosition, wxDefaultSize, style);
    }

    wxRibbonPanel * const panel = new wxRibbonPanel;
    if ( panel->Create(page, wxID_ANY, label, wxBitmapBundle(),
                       wxDefaultPosition, wxDefaultSize, style) )
        return panel;

    delete panel;
    return nullptr;
}

wxRibbonButtonBar *CreateButtonBar(wxRibbonPanel *panel,
                                   bool useDefaultCreate)
{
    if ( !useDefaultCreate )
        return new wxRibbonButtonBar(panel, wxID_ANY);

    wxRibbonButtonBar * const bar = new wxRibbonButtonBar;
    if ( bar->Create(panel, wxID_ANY) )
        return bar;

    delete bar;
    return nullptr;
}

wxRibbonToolBar *CreateToolBar(wxRibbonPanel *panel,
                               bool useDefaultCreate)
{
    if ( !useDefaultCreate )
        return new wxRibbonToolBar(panel, wxID_ANY);

    wxRibbonToolBar * const bar = new wxRibbonToolBar;
    if ( bar->Create(panel, wxID_ANY) )
        return bar;

    delete bar;
    return nullptr;
}

wxRibbonGallery *CreateGallery(wxRibbonPanel *panel,
                               bool useDefaultCreate)
{
    if ( !useDefaultCreate )
        return new wxRibbonGallery(panel, wxID_ANY);

    wxRibbonGallery * const gallery = new wxRibbonGallery;
    if ( gallery->Create(panel, wxID_ANY) )
        return gallery;

    delete gallery;
    return nullptr;
}

class RibbonFixture final
{
public:
    RibbonFixture() = default;

    ~RibbonFixture()
    {
        Cleanup();
        DrainDispatch();
    }

    bool Create(bool useDefaultCreate, int ordinal)
    {
        wxFrame * const frameA = new wxFrame(
            nullptr, wxID_ANY, "WinUI ribbon harness A",
            wxDefaultPosition, wxSize(1200, 340));
        wxFrame * const frameB = new wxFrame(
            nullptr, wxID_ANY, "WinUI ribbon harness B",
            wxDefaultPosition, wxSize(1200, 340));
        m_frameA = wxWeakRef<wxFrame>(frameA);
        m_frameB = wxWeakRef<wxFrame>(frameB);

        if ( !frameA->GetHWND() || !frameB->GetHWND() )
            return false;

        wxRibbonBar *bar = nullptr;
        if ( useDefaultCreate )
        {
            bar = new wxRibbonBar;
            if ( !bar->Create(frameA, wxID_ANY) )
            {
                delete bar;
                return false;
            }
        }
        else
        {
            bar = new wxRibbonBar(frameA, wxID_ANY);
        }
        m_bar = wxWeakRef<wxRibbonBar>(bar);

        wxRibbonPage * const home =
            CreatePage(bar, "Home", useDefaultCreate);
        wxRibbonPage * const second =
            CreatePage(bar, "Second", useDefaultCreate);
        wxRibbonPage * const third =
            CreatePage(bar, "Third", useDefaultCreate);
        if ( !home || !second || !third )
            return false;
        m_home = wxWeakRef<wxRibbonPage>(home);
        m_second = wxWeakRef<wxRibbonPage>(second);
        m_third = wxWeakRef<wxRibbonPage>(third);

        wxRibbonPanel * const buttonPanel =
            CreatePanel(home, "Buttons", useDefaultCreate,
                        wxRIBBON_PANEL_EXT_BUTTON);
        wxRibbonPanel * const toolPanel =
            CreatePanel(home, "Tools", useDefaultCreate);
        wxRibbonPanel * const galleryPanel =
            CreatePanel(home, "Gallery", useDefaultCreate);
        wxRibbonPanel * const sentinelPanel =
            CreatePanel(home, "WinUI sentinel", useDefaultCreate);
        if ( !buttonPanel || !toolPanel || !galleryPanel || !sentinelPanel )
            return false;
        m_buttonPanel = wxWeakRef<wxRibbonPanel>(buttonPanel);
        m_toolPanel = wxWeakRef<wxRibbonPanel>(toolPanel);
        m_galleryPanel = wxWeakRef<wxRibbonPanel>(galleryPanel);
        m_sentinelPanel = wxWeakRef<wxRibbonPanel>(sentinelPanel);

        wxRibbonButtonBar * const buttonBar =
            CreateButtonBar(buttonPanel, useDefaultCreate);
        wxRibbonToolBar * const toolBar =
            CreateToolBar(toolPanel, useDefaultCreate);
        wxRibbonGallery * const gallery =
            CreateGallery(galleryPanel, useDefaultCreate);
        if ( !buttonBar || !toolBar || !gallery )
            return false;
        m_buttonBar = wxWeakRef<wxRibbonButtonBar>(buttonBar);
        m_toolBar = wxWeakRef<wxRibbonToolBar>(toolBar);
        m_gallery = wxWeakRef<wxRibbonGallery>(gallery);

        wxBitmap bitmap;
        if ( !bitmap.Create(32, 32, 32) )
            return false;
        const wxBitmapBundle icon = wxBitmapBundle::FromBitmap(bitmap);
        if ( !icon.IsOk() )
            return false;

        if ( !buttonBar->AddButton(ID_RIBBON_BUTTON_NORMAL,
                                   "Normal", icon, "normal help") ||
             !buttonBar->AddDropdownButton(ID_RIBBON_BUTTON_DROPDOWN,
                                           "Dropdown", icon,
                                           "dropdown help") ||
             !buttonBar->AddHybridButton(ID_RIBBON_BUTTON_HYBRID,
                                         "Hybrid", icon, "hybrid help") ||
             !buttonBar->AddToggleButton(ID_RIBBON_BUTTON_TOGGLE,
                                         "Toggle", icon, "toggle help") )
        {
            return false;
        }

        if ( !toolBar->AddTool(ID_RIBBON_TOOL_NORMAL,
                               icon, "normal tool") ||
             !toolBar->AddDropdownTool(ID_RIBBON_TOOL_DROPDOWN,
                                       icon, "dropdown tool") ||
             !toolBar->AddHybridTool(ID_RIBBON_TOOL_HYBRID,
                                     icon, "hybrid tool") ||
             !toolBar->AddToggleTool(ID_RIBBON_TOOL_TOGGLE,
                                     icon, "toggle tool") ||
             !toolBar->AddSeparator() )
        {
            return false;
        }
        toolBar->SetRows(1, 2);

        m_galleryFirst = gallery->Append(icon, ID_RIBBON_GALLERY_FIRST);
        m_gallerySecond = gallery->Append(icon, ID_RIBBON_GALLERY_SECOND);
        m_galleryThird = gallery->Append(icon, ID_RIBBON_GALLERY_THIRD);
        if ( !m_galleryFirst || !m_gallerySecond || !m_galleryThird )
            return false;
        gallery->SetSelection(m_gallerySecond);

        wxButton * const sentinel = new wxButton(
            sentinelPanel, wxID_ANY, "WinUI ribbon slot sentinel");
        sentinel->SetMinSize(wxSize(260, 60));
        wxBoxSizer * const sentinelSizer = new wxBoxSizer(wxVERTICAL);
        sentinelSizer->Add(sentinel, 1, wxEXPAND | wxALL, 3);
        sentinelPanel->SetSizer(sentinelSizer);
        m_sentinel = wxWeakRef<wxButton>(sentinel);

        frameA->SetClientSize(wxSize(1200, 320));
        frameB->SetClientSize(wxSize(1200, 320));
        bar->SetSize(frameA->GetClientRect());
        if ( !bar->Realize() )
            return false;

        if ( !ShowOffscreenWithoutActivation(frameA, ordinal) ||
             !ShowOffscreenWithoutActivation(frameB, ordinal + 1) )
        {
            return false;
        }

        return WaitForSingleOwner(frameA);
    }

    bool Resize(int width, int height = 320)
    {
        wxRibbonBar * const bar = GetBar();
        wxFrame * const parent = wxDynamicCast(bar ? bar->GetParent() : nullptr,
                                               wxFrame);
        if ( !bar || !parent )
            return false;

        parent->SetClientSize(wxSize(width, height));
        // A tall offscreen TLW can be clamped to the monitor work area by
        // USER32. The Ribbon layout gate needs the requested logical test
        // viewport, not the incidental native outer-frame constraint.
        parent->Layout();
        bar->SetSize(wxRect(wxPoint(0, 0), wxSize(width, height)));
        const bool realized = bar->Realize();
        DrainDispatch(4);
        return realized && bar->GetSize() == wxSize(width, height);
    }

    bool ReparentTo(wxFrame *destination)
    {
        wxRibbonBar * const bar = GetBar();
        if ( !bar || !destination || !bar->Reparent(destination) )
            return false;

        bar->SetSize(destination->GetClientRect());
        if ( !bar->Realize() )
            return false;
        destination->Layout();
        return WaitForSingleOwner(destination);
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

        wxWindow * const roots[] = { GetFrameA(), GetFrameB() };
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
                    return !m_frameA && !m_frameB && !m_bar && !m_home &&
                           !m_second && !m_third && !m_buttonPanel &&
                           !m_toolPanel && !m_galleryPanel &&
                           !m_sentinelPanel && !m_buttonBar && !m_toolBar &&
                           !m_gallery && !m_sentinel &&
                           before.IsRestored();
                }) )
        {
            return false;
        }

        return DrainToQuiescence() && before.IsRestored();
    }

    wxFrame *GetFrameA() const { return m_frameA.get(); }
    wxFrame *GetFrameB() const { return m_frameB.get(); }
    wxRibbonBar *GetBar() const { return m_bar.get(); }
    wxRibbonPage *GetHome() const { return m_home.get(); }
    wxRibbonPage *GetSecond() const { return m_second.get(); }
    wxRibbonPage *GetThird() const { return m_third.get(); }
    wxRibbonPanel *GetButtonPanel() const { return m_buttonPanel.get(); }
    wxRibbonPanel *GetToolPanel() const { return m_toolPanel.get(); }
    wxRibbonPanel *GetGalleryPanel() const { return m_galleryPanel.get(); }
    wxRibbonPanel *GetSentinelPanel() const { return m_sentinelPanel.get(); }
    wxRibbonButtonBar *GetButtonBar() const { return m_buttonBar.get(); }
    wxRibbonToolBar *GetToolBar() const { return m_toolBar.get(); }
    wxRibbonGallery *GetGallery() const { return m_gallery.get(); }
    wxButton *GetSentinel() const { return m_sentinel.get(); }
    wxRibbonGalleryItem *GetGalleryFirst() const { return m_galleryFirst; }
    wxRibbonGalleryItem *GetGallerySecond() const { return m_gallerySecond; }
    wxRibbonGalleryItem *GetGalleryThird() const { return m_galleryThird; }

private:
    bool WaitForSingleOwner(wxWindow *expectedTLW)
    {
        return DrainUntil(
            [this, expectedTLW]() { return HasSingleOwner(expectedTLW); });
    }

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
    wxWeakRef<wxRibbonBar> m_bar;
    wxWeakRef<wxRibbonPage> m_home;
    wxWeakRef<wxRibbonPage> m_second;
    wxWeakRef<wxRibbonPage> m_third;
    wxWeakRef<wxRibbonPanel> m_buttonPanel;
    wxWeakRef<wxRibbonPanel> m_toolPanel;
    wxWeakRef<wxRibbonPanel> m_galleryPanel;
    wxWeakRef<wxRibbonPanel> m_sentinelPanel;
    wxWeakRef<wxRibbonButtonBar> m_buttonBar;
    wxWeakRef<wxRibbonToolBar> m_toolBar;
    wxWeakRef<wxRibbonGallery> m_gallery;
    wxWeakRef<wxButton> m_sentinel;
    wxRibbonGalleryItem *m_galleryFirst = nullptr;
    wxRibbonGalleryItem *m_gallerySecond = nullptr;
    wxRibbonGalleryItem *m_galleryThird = nullptr;
};

void CheckStablePublicModel(const RibbonFixture& fixture)
{
    wxRibbonBar * const bar = fixture.GetBar();
    wxRibbonPage * const home = fixture.GetHome();
    wxRibbonButtonBar * const buttons = fixture.GetButtonBar();
    wxRibbonToolBar * const tools = fixture.GetToolBar();
    wxRibbonGallery * const gallery = fixture.GetGallery();
    REQUIRE(bar);
    REQUIRE(home);
    REQUIRE(buttons);
    REQUIRE(tools);
    REQUIRE(gallery);

    CHECK(bar->GetPageCount() == 3);
    CHECK(home->GetPanelCount() == 4);
    CHECK(buttons->GetButtonCount() == 4);
    CHECK(tools->GetToolCount() == 5);
    CHECK(tools->GetToolKind(ID_RIBBON_TOOL_NORMAL) ==
          wxRIBBON_BUTTON_NORMAL);
    CHECK(tools->GetToolKind(ID_RIBBON_TOOL_DROPDOWN) ==
          wxRIBBON_BUTTON_DROPDOWN);
    CHECK(tools->GetToolKind(ID_RIBBON_TOOL_HYBRID) ==
          wxRIBBON_BUTTON_HYBRID);
    CHECK(tools->GetToolKind(ID_RIBBON_TOOL_TOGGLE) ==
          wxRIBBON_BUTTON_TOGGLE);
    CHECK(gallery->GetCount() == 3);
    CHECK(gallery->GetSelection() == fixture.GetGallerySecond());
}

using PanelMinimisedState = std::array<bool, 4>;
using PanelRectState = std::array<wxRect, 4>;

PanelMinimisedState CapturePanelMinimisedState(const RibbonFixture& fixture)
{
    const wxRibbonPanel * const panels[] =
    {
        fixture.GetButtonPanel(),
        fixture.GetToolPanel(),
        fixture.GetGalleryPanel(),
        fixture.GetSentinelPanel()
    };

    PanelMinimisedState state{};
    for ( size_t i = 0; i < state.size(); ++i )
    {
        REQUIRE(panels[i]);
        state[i] = panels[i]->IsMinimised();
    }
    return state;
}

PanelRectState CapturePanelRectState(const RibbonFixture& fixture)
{
    const wxRibbonPanel * const panels[] =
    {
        fixture.GetButtonPanel(),
        fixture.GetToolPanel(),
        fixture.GetGalleryPanel(),
        fixture.GetSentinelPanel()
    };

    PanelRectState state{};
    for ( size_t i = 0; i < state.size(); ++i )
    {
        REQUIRE(panels[i]);
        state[i] = panels[i]->GetRect();
    }
    return state;
}

unsigned CountMinimised(const PanelMinimisedState& state)
{
    unsigned count = 0;
    for ( const bool minimised : state )
    {
        if ( minimised )
            ++count;
    }
    return count;
}

unsigned CountRibbonBarAuxiliaryChildren(wxRibbonBar *bar)
{
    REQUIRE(bar);
    unsigned count = 0;
    for ( wxWindowList::compatibility_iterator node =
              bar->GetChildren().GetFirst();
          node;
          node = node->GetNext() )
    {
        if ( !wxDynamicCast(node->GetData(), wxRibbonPage) )
            ++count;
    }
    return count;
}

void CheckPanelOrder(const RibbonFixture& fixture, wxOrientation orientation)
{
    wxRibbonPage * const page = fixture.GetHome();
    REQUIRE(page);

    int previous = -1;
    for ( int i = 0; i < static_cast<int>(page->GetPanelCount()); ++i )
    {
        wxRibbonPanel * const panel = page->GetPanel(i);
        REQUIRE(panel);
        CHECK(panel->GetSize().x > 0);
        CHECK(panel->GetSize().y > 0);
        const int position = orientation == wxVERTICAL
                                 ? panel->GetPosition().y
                                 : panel->GetPosition().x;
        CHECK(position >= previous);
        previous = position;
    }
}

// ---------------------------------------------------------------------------
// R2 bounded native spike
//
// This is intentionally test-only.  It answers the architectural question
// left open by the managed-only Toolkit packages: can a wx-owned model be
// projected with first-party WinUI controls into the EXISTING shared host,
// while retaining exact mutation and callback lifetime semantics?  It is not
// a partial production backend and therefore cannot silently replace R1.
// ---------------------------------------------------------------------------

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXAP = winrt::Microsoft::UI::Xaml::Automation::Peers;
namespace MUXAPR = winrt::Microsoft::UI::Xaml::Automation::Provider;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXCP = winrt::Microsoft::UI::Xaml::Controls::Primitives;

enum class NativeRibbonCommandKind
{
    Normal,
    Toggle,
    Dropdown
};

struct NativeRibbonCommandModel
{
    int id = 0;
    std::wstring label;
    NativeRibbonCommandKind kind = NativeRibbonCommandKind::Normal;
    bool enabled = true;
    bool checked = false;
};

struct NativeRibbonGroupModel
{
    std::wstring label;
    double expandedWidth = 0;
    unsigned collapsePriority = 0;
    bool collapsed = false;
    std::vector<NativeRibbonCommandModel> commands;
};

struct NativeRibbonPageModel
{
    std::wstring label;
    std::vector<NativeRibbonGroupModel> groups;
};

// No XAML object appears in this type: this is the authority projected by the
// peer, never a mirror reconstructed from TabView/CommandBar state.
struct NativeRibbonSpikeModel
{
    static NativeRibbonSpikeModel Create()
    {
        NativeRibbonSpikeModel model;
        model.pages =
        {
            {
                L"Home",
                {
                    { L"Clipboard", 300, 0, false,
                      { { 6101, L"Paste", NativeRibbonCommandKind::Normal },
                        { 6102, L"Pin", NativeRibbonCommandKind::Toggle } } },
                    { L"Editing", 280, 1, false,
                      { { 6201, L"Find", NativeRibbonCommandKind::Normal },
                        { 6202, L"Replace", NativeRibbonCommandKind::Dropdown } } },
                    { L"View", 260, 2, false,
                      { { 6301, L"Zoom", NativeRibbonCommandKind::Normal },
                        { 6302, L"Grid", NativeRibbonCommandKind::Toggle } } }
                }
            },
            {
                L"Insert",
                {
                    { L"Media", 320, 0, false,
                      { { 7101, L"Image", NativeRibbonCommandKind::Normal },
                        { 7102, L"Video", NativeRibbonCommandKind::Dropdown } } },
                    { L"Links", 280, 1, false,
                      { { 7201, L"Link", NativeRibbonCommandKind::Normal } } }
                }
            },
            {
                L"Layout",
                {
                    { L"Arrange", 360, 0, false,
                      { { 8101, L"Align", NativeRibbonCommandKind::Dropdown },
                        { 8102, L"Group", NativeRibbonCommandKind::Normal } } }
                }
            }
        };
        return model;
    }

    NativeRibbonCommandModel *FindCommand(int id)
    {
        for ( NativeRibbonPageModel& page : pages )
        {
            for ( NativeRibbonGroupModel& group : page.groups )
            {
                for ( NativeRibbonCommandModel& command : group.commands )
                {
                    if ( command.id == id )
                        return &command;
                }
            }
        }
        return nullptr;
    }

    const NativeRibbonCommandModel *FindCommand(int id) const
    {
        return const_cast<NativeRibbonSpikeModel *>(this)->FindCommand(id);
    }

    bool ReplaceCommand(int oldId, NativeRibbonCommandModel replacement)
    {
        for ( NativeRibbonPageModel& page : pages )
        {
            for ( NativeRibbonGroupModel& group : page.groups )
            {
                for ( NativeRibbonCommandModel& command : group.commands )
                {
                    if ( command.id == oldId )
                    {
                        command = std::move(replacement);
                        return true;
                    }
                }
            }
        }
        return false;
    }

    size_t GetCommandCount() const
    {
        size_t count = 0;
        for ( const NativeRibbonPageModel& page : pages )
        {
            for ( const NativeRibbonGroupModel& group : page.groups )
                count += group.commands.size();
        }
        return count;
    }

    std::vector<NativeRibbonPageModel> pages;
    size_t activePage = 0;
};

unsigned gs_nativeRibbonSpikeStates = 0;
unsigned gs_nativeRibbonSpikePeers = 0;
unsigned gs_nativeRibbonSpikeHandlerAdds = 0;
unsigned gs_nativeRibbonSpikeHandlerRevokes = 0;

struct NativeRibbonSpikeState
{
    struct ProgrammaticSelectionTicket
    {
        unsigned long long generation = 0;
        unsigned long long serial = 0;
        int target = -1;
    };

    NativeRibbonSpikeState()
    {
        ++gs_nativeRibbonSpikeStates;
    }

    ~NativeRibbonSpikeState()
    {
        --gs_nativeRibbonSpikeStates;
    }

    NativeRibbonSpikeModel model = NativeRibbonSpikeModel::Create();
    std::map<int, unsigned> commandEvents;
    unsigned selectionChangingEvents = 0;
    unsigned selectionChangedEvents = 0;
    unsigned selectionVetoes = 0;
    unsigned selectionFailures = 0;
    unsigned layoutEvents = 0;
    unsigned layoutFailures = 0;
    unsigned peerWriteDepth = 0;
    unsigned long long generation = 0;
    unsigned long long nextSelectionSerial = 0;
    double lastWidth = 1200;
    double lastActualWidth = 0;
    std::vector<ProgrammaticSelectionTicket> programmaticSelections;
    unsigned long long userSelectionGeneration = 0;
    int userSelectionTarget = -1;
    bool userSelectionPending = false;
    bool vetoNextSelection = false;
    bool alive = true;
};

class NativeRibbonPeerWriteScope final
{
public:
    explicit NativeRibbonPeerWriteScope(NativeRibbonSpikeState& state)
        : m_state(state)
    {
        ++m_state.peerWriteDepth;
    }

    ~NativeRibbonPeerWriteScope() noexcept
    {
        --m_state.peerWriteDepth;
    }

    NativeRibbonPeerWriteScope(const NativeRibbonPeerWriteScope&) = delete;
    NativeRibbonPeerWriteScope& operator=(
        const NativeRibbonPeerWriteScope&) = delete;

private:
    NativeRibbonSpikeState& m_state;
};

class NativeRibbonSpikePeer final
    : public std::enable_shared_from_this<NativeRibbonSpikePeer>
{
public:
    static std::shared_ptr<NativeRibbonSpikePeer> Create(
        const std::shared_ptr<NativeRibbonSpikeState>& state)
    {
        // Construct the shared owner before Rebuild() installs any callback.
        // If a WinRT call throws during initialization, the local shared_ptr
        // still runs the destructor and restores the peer counter.
        std::shared_ptr<NativeRibbonSpikePeer> peer(
            new NativeRibbonSpikePeer(state));
        if ( !peer->Rebuild() )
            return {};
        return peer;
    }

    ~NativeRibbonSpikePeer() noexcept
    {
        Disconnect();
        --gs_nativeRibbonSpikePeers;
    }

    MUXC::TabView Root() const { return m_tabs; }

    bool Rebuild()
    {
        const std::shared_ptr<NativeRibbonSpikeState> state = m_state;
        if ( !state || !state->alive || !m_tabs )
            return false;

        const std::weak_ptr<NativeRibbonSpikePeer> weakPeer =
            shared_from_this();
        RevokeHandlers();
        ++state->generation;
        const unsigned long long generation = state->generation;
        state->programmaticSelections.clear();
        state->userSelectionPending = false;
        state->vetoNextSelection = false;

        {
            NativeRibbonPeerWriteScope write(*state);
            m_tabs.TabItems().Clear();
        }
        m_groups.clear();
        m_commands.clear();

        for ( size_t pageIndex = 0;
              pageIndex < state->model.pages.size();
              ++pageIndex )
        {
            NativeRibbonPageModel& pageModel =
                state->model.pages[pageIndex];
            MUXC::StackPanel pagePanel;
            pagePanel.Orientation(MUXC::Orientation::Horizontal);

            for ( size_t groupIndex = 0;
                  groupIndex < pageModel.groups.size();
                  ++groupIndex )
            {
                NativeRibbonGroupModel& groupModel =
                    pageModel.groups[groupIndex];
                groupModel.collapsed = false;

                GroupPeer group;
                group.pageIndex = pageIndex;
                group.groupIndex = groupIndex;
                group.root = MUXC::Grid();
                group.expandedBar = MUXC::CommandBar();
                group.collapsedBar = MUXC::CommandBar();
                group.collapsedButton = MUXC::DropDownButton();
                group.collapsedFlyout = MUXC::Flyout();

                MUXC::StackPanel expandedContent;
                MUXC::TextBlock label;
                label.Text(winrt::hstring(groupModel.label));
                expandedContent.Children().Append(label);
                expandedContent.Children().Append(group.expandedBar);

                MUXC::Border expandedBorder;
                expandedBorder.Width(groupModel.expandedWidth);
                expandedBorder.Child(expandedContent);
                group.expandedRoot = expandedBorder;

                group.collapsedButton.Content(
                    winrt::box_value(winrt::hstring(groupModel.label)));
                group.collapsedButton.Visibility(MUX::Visibility::Collapsed);
                group.collapsedFlyout.Content(group.collapsedBar);
                group.collapsedButton.Flyout(group.collapsedFlyout);

                group.root.Children().Append(group.expandedRoot);
                group.root.Children().Append(group.collapsedButton);
                pagePanel.Children().Append(group.root);

                for ( const NativeRibbonCommandModel& commandModel :
                      groupModel.commands )
                {
                    CommandPeer command;
                    command.id = commandModel.id;
                    command.pageIndex = pageIndex;
                    command.groupIndex = groupIndex;

                    if ( commandModel.kind ==
                         NativeRibbonCommandKind::Toggle )
                    {
                        MUXC::AppBarToggleButton button;
                        button.Label(winrt::hstring(commandModel.label));
                        button.IsChecked(commandModel.checked);
                        command.button = button;
                        command.element =
                            button.as<MUXC::ICommandBarElement>();
                    }
                    else
                    {
                        MUXC::AppBarButton button;
                        button.Label(winrt::hstring(commandModel.label));
                        if ( commandModel.kind ==
                             NativeRibbonCommandKind::Dropdown )
                        {
                            MUXC::MenuFlyout menu;
                            MUXC::MenuFlyoutItem item;
                            item.Text(L"Bounded R2 probe item");
                            menu.Items().Append(item);
                            button.Flyout(menu);
                        }
                        command.button = button;
                        command.element =
                            button.as<MUXC::ICommandBarElement>();
                    }

                    command.button.IsEnabled(commandModel.enabled);
                    const std::weak_ptr<NativeRibbonSpikeState> weakState =
                        state;
                    command.clickToken = command.button.Click(
                        [weakState, generation, id = commandModel.id](
                            const winrt::Windows::Foundation::IInspectable& sender,
                            const MUX::RoutedEventArgs&)
                        {
                            DispatchCommand(
                                weakState, generation, id, sender);
                        });
                    ++gs_nativeRibbonSpikeHandlerAdds;

                    group.expandedBar.PrimaryCommands().Append(
                        command.element);
                    m_commands.push_back(std::move(command));
                }

                m_groups.push_back(std::move(group));
            }

            MUXC::TabViewItem tab;
            tab.Header(winrt::box_value(winrt::hstring(pageModel.label)));
            tab.IsClosable(false);
            tab.Content(pagePanel);
            m_tabs.TabItems().Append(tab);
        }

        const std::weak_ptr<NativeRibbonSpikeState> weakState = state;
        m_selectionToken = m_tabs.SelectionChanged(
            [weakState, generation](
                const winrt::Windows::Foundation::IInspectable& sender,
                const MUXC::SelectionChangedEventArgs& event)
            {
                const std::shared_ptr<NativeRibbonSpikeState> live =
                    weakState.lock();
                if ( !live || !live->alive ||
                     live->generation != generation )
                {
                    return;
                }

                const MUXC::TabView tabs = sender.try_as<MUXC::TabView>();
                if ( !tabs )
                    return;

                const int requested = ResolveSelectionIndex(tabs, event);
                if ( requested < 0 ||
                     static_cast<size_t>(requested) >=
                         live->model.pages.size() )
                {
                    return;
                }

                const bool isExplicitUserSelection =
                    live->userSelectionPending &&
                    live->userSelectionGeneration == generation &&
                    live->userSelectionTarget == requested;
                if ( isExplicitUserSelection )
                {
                    live->userSelectionPending = false;
                    live->programmaticSelections.clear();
                }
                else if ( ConsumeProgrammaticSelection(
                              *live, generation, requested) )
                {
                    return;
                }
                else if ( live->peerWriteDepth != 0 ||
                          static_cast<size_t>(requested) ==
                              live->model.activePage )
                {
                    // Covers a delayed bootstrap echo from TabItems.Append()
                    // and duplicate notifications for an already authoritative
                    // page. Neither represents a user-visible transition.
                    live->programmaticSelections.clear();
                    return;
                }
                else
                {
                    // Event ordering guarantees that an unmatched user change
                    // supersedes any older programmatic writes coalesced away
                    // by WinUI. Retiring them prevents a later real user event
                    // from being mistaken for a stale echo.
                    live->programmaticSelections.clear();
                    live->userSelectionPending = false;
                }

                ++live->selectionChangingEvents;
                if ( live->vetoNextSelection )
                {
                    live->vetoNextSelection = false;
                    ++live->selectionVetoes;
                    const int active =
                        static_cast<int>(live->model.activePage);
                    const unsigned long long ticket =
                        ArmProgrammaticSelection(
                            *live, generation, active);
                    try
                    {
                        NativeRibbonPeerWriteScope write(*live);
                        tabs.SelectedIndex(active);
                    }
                    catch ( ... )
                    {
                        CancelProgrammaticSelection(*live, ticket);
                        ++live->selectionFailures;
                    }
                    return;
                }

                live->model.activePage =
                    static_cast<size_t>(requested);
                ++live->selectionChangedEvents;
            });
        m_hasSelectionHandler = true;
        ++gs_nativeRibbonSpikeHandlerAdds;

        m_sizeToken = m_tabs.SizeChanged(
            [weakState, weakPeer, generation](
                const winrt::Windows::Foundation::IInspectable&,
                const MUX::SizeChangedEventArgs& event)
            {
                const std::shared_ptr<NativeRibbonSpikeState> live =
                    weakState.lock();
                const std::shared_ptr<NativeRibbonSpikePeer> peer =
                    weakPeer.lock();
                if ( !live || !peer || !live->alive ||
                     live->generation != generation )
                {
                    return;
                }

                const double width = event.NewSize().Width;
                if ( width <= 0 )
                    return;

                ++live->layoutEvents;
                live->lastActualWidth = width;
                try
                {
                    if ( !peer->ApplyWidth(width) )
                        ++live->layoutFailures;
                }
                catch ( ... )
                {
                    // Routed callbacks must never unwind through WinUI. The
                    // deterministic failure counter makes this visible.
                    ++live->layoutFailures;
                }
            });
        m_hasSizeHandler = true;
        ++gs_nativeRibbonSpikeHandlerAdds;

        {
            const int active = static_cast<int>(state->model.activePage);
            if ( m_tabs.SelectedIndex() != active )
            {
                ArmProgrammaticSelection(*state, generation, active);
                NativeRibbonPeerWriteScope write(*state);
                m_tabs.SelectedIndex(active);
            }
        }

        const double actualWidth = m_tabs.ActualWidth();
        ApplyWidth(actualWidth > 0 ? actualWidth : state->lastWidth);
        return true;
    }

private:
    explicit NativeRibbonSpikePeer(
        const std::shared_ptr<NativeRibbonSpikeState>& state)
        : m_state(state),
          m_tabs(MUXC::TabView())
    {
        m_tabs.IsAddTabButtonVisible(false);
        ++gs_nativeRibbonSpikePeers;
    }

public:
    void Disconnect() noexcept
    {
        const std::shared_ptr<NativeRibbonSpikeState> state = m_state;
        if ( !state )
            return;

        state->alive = false;
        ++state->generation;
        try
        {
            RevokeHandlers();
        }
        catch ( ... )
        {
            // The state and generation were retired before touching WinRT.
            // Even if a revoke fails during exception unwinding, any callback
            // still owned by the visual tree can only lock dead state.
        }
        for ( GroupPeer& group : m_groups )
        {
            try
            {
                if ( group.collapsedFlyout &&
                     group.collapsedFlyout.IsOpen() )
                {
                    group.collapsedFlyout.Hide();
                }
            }
            catch ( ... )
            {
                // State/generation are already retired. Releasing the visual
                // tree below is the final fallback for a flyout that rejects
                // Hide() during teardown.
            }
        }
        if ( m_tabs )
        {
            try
            {
                NativeRibbonPeerWriteScope write(*state);
                m_tabs.TabItems().Clear();
            }
            catch ( ... )
            {
                // A destructor/failure unwind must still retire every weak
                // callback and release the model.
            }
        }
        m_groups.clear();
        m_commands.clear();
        m_state.reset();
    }

    bool SetActivePageProgrammatically(size_t page)
    {
        const std::shared_ptr<NativeRibbonSpikeState> state = m_state;
        if ( !state || page >= state->model.pages.size() )
            return false;

        state->model.activePage = page;
        state->userSelectionPending = false;
        state->vetoNextSelection = false;
        const int target = static_cast<int>(page);
        if ( m_tabs.SelectedIndex() == target )
            return true;

        const unsigned long long ticket = ArmProgrammaticSelection(
            *state, state->generation, target);
        NativeRibbonPeerWriteScope write(*state);
        try
        {
            m_tabs.SelectedIndex(target);
        }
        catch ( ... )
        {
            CancelProgrammaticSelection(*state, ticket);
            throw;
        }
        return true;
    }

    bool SelectPageAsUser(size_t page, bool veto)
    {
        const std::shared_ptr<NativeRibbonSpikeState> state = m_state;
        if ( !state || page >= state->model.pages.size() )
            return false;

        const int target = static_cast<int>(page);
        state->vetoNextSelection = veto;
        if ( m_tabs.SelectedIndex() == target )
        {
            // No SelectionChanged event will consume the one-shot verdict.
            state->vetoNextSelection = false;
            state->userSelectionPending = false;
            return true;
        }

        state->userSelectionGeneration = state->generation;
        state->userSelectionTarget = target;
        state->userSelectionPending = true;
        try
        {
            m_tabs.SelectedIndex(target);
        }
        catch ( ... )
        {
            state->userSelectionPending = false;
            state->vetoNextSelection = false;
            throw;
        }
        return true;
    }

    bool ApplyWidth(double width)
    {
        const std::shared_ptr<NativeRibbonSpikeState> state = m_state;
        if ( !state || width <= 0 )
            return false;

        state->lastWidth = width;
        const double available = std::max(180.0, width - 32.0);

        for ( size_t pageIndex = 0;
              pageIndex < state->model.pages.size();
              ++pageIndex )
        {
            NativeRibbonPageModel& page = state->model.pages[pageIndex];
            double required = 0;
            for ( const NativeRibbonGroupModel& group : page.groups )
                required += group.expandedWidth;

            std::vector<size_t> collapseOrder(page.groups.size());
            for ( size_t i = 0; i < collapseOrder.size(); ++i )
                collapseOrder[i] = i;
            std::stable_sort(
                collapseOrder.begin(), collapseOrder.end(),
                [&page](size_t lhs, size_t rhs)
                {
                    return page.groups[lhs].collapsePriority <
                           page.groups[rhs].collapsePriority;
                });

            std::vector<bool> collapsed(page.groups.size(), false);
            for ( const size_t groupIndex : collapseOrder )
            {
                if ( required <= available )
                    break;
                collapsed[groupIndex] = true;
                required -= page.groups[groupIndex].expandedWidth -
                            CollapsedGroupWidth;
            }

            for ( size_t groupIndex = 0;
                  groupIndex < page.groups.size();
                  ++groupIndex )
            {
                GroupPeer * const peer =
                    FindGroup(pageIndex, groupIndex);
                if ( !peer )
                    return false;
                ApplyGroupState(*peer, collapsed[groupIndex]);
                page.groups[groupIndex].collapsed = collapsed[groupIndex];
            }
        }
        return true;
    }

    unsigned GetCollapsedCount(size_t page) const
    {
        const std::shared_ptr<NativeRibbonSpikeState> state = m_state;
        if ( !state || page >= state->model.pages.size() )
            return 0;

        unsigned count = 0;
        for ( const NativeRibbonGroupModel& group :
              state->model.pages[page].groups )
        {
            if ( group.collapsed )
                ++count;
        }
        return count;
    }

    bool EveryCommandHasOneCollection() const
    {
        for ( const CommandPeer& command : m_commands )
        {
            unsigned occurrences = 0;
            for ( const GroupPeer& group : m_groups )
            {
                occurrences += CountIdentity(
                    group.expandedBar.PrimaryCommands(), command.element);
                occurrences += CountIdentity(
                    group.collapsedBar.PrimaryCommands(), command.element);
            }
            if ( occurrences != 1 )
                return false;
        }
        return true;
    }

    uintptr_t GetCommandIdentity(int id) const
    {
        const CommandPeer * const command = FindCommandPeer(id);
        return command
                   ? reinterpret_cast<uintptr_t>(
                         winrt::get_abi(command->button))
                   : 0;
    }

    bool GetToggleState(int id, bool *checked) const
    {
        if ( !checked )
            return false;
        const CommandPeer * const command = FindCommandPeer(id);
        if ( !command )
            return false;
        const MUXC::AppBarToggleButton toggle =
            command->button.try_as<MUXC::AppBarToggleButton>();
        if ( !toggle )
            return false;
        const auto value = toggle.IsChecked();
        *checked = value && value.Value();
        return true;
    }

    bool SetToggleState(int id, bool checked)
    {
        const std::shared_ptr<NativeRibbonSpikeState> state = m_state;
        NativeRibbonCommandModel * const model =
            state ? state->model.FindCommand(id) : nullptr;
        CommandPeer * const command = FindCommandPeer(id);
        if ( !model || !command )
            return false;
        const MUXC::AppBarToggleButton toggle =
            command->button.try_as<MUXC::AppBarToggleButton>();
        if ( !toggle )
            return false;
        model->checked = checked;
        toggle.IsChecked(checked);
        return true;
    }

    bool InvokeCommand(int id) const
    {
        const CommandPeer * const command = FindCommandPeer(id);
        if ( !command )
            return false;
        const MUXC::AppBarButton button =
            command->button.try_as<MUXC::AppBarButton>();
        if ( !button )
            return false;

        const MUXAP::AppBarButtonAutomationPeer peer(button);
        const MUXAPR::IInvokeProvider invoke =
            peer.GetPattern(MUXAP::PatternInterface::Invoke)
                .try_as<MUXAPR::IInvokeProvider>();
        if ( !invoke )
            return false;
        invoke.Invoke();
        return true;
    }

    bool IsCommandInCollapsedCollection(int id) const
    {
        const CommandPeer * const command = FindCommandPeer(id);
        if ( !command )
            return false;
        const GroupPeer * const group =
            FindGroup(command->pageIndex, command->groupIndex);
        return group && group->collapsed &&
               CountIdentity(group->collapsedBar.PrimaryCommands(),
                             command->element) == 1;
    }

    bool ShowCollapsedGroup(size_t pageIndex, size_t groupIndex)
    {
        GroupPeer * const group = FindGroup(pageIndex, groupIndex);
        if ( !group || !group->collapsed ||
             group->collapsedButton.Visibility() != MUX::Visibility::Visible )
        {
            return false;
        }

        group->collapsedFlyout.ShowAt(group->collapsedButton);
        return true;
    }

    bool IsGroupFlyoutOpen(size_t pageIndex, size_t groupIndex) const
    {
        const GroupPeer * const group = FindGroup(pageIndex, groupIndex);
        return group && group->collapsedFlyout &&
               group->collapsedFlyout.IsOpen();
    }

    bool DispatchRetiredGenerationForTest(
        unsigned long long generation, int id) const
    {
        return DispatchCommand(
            std::weak_ptr<NativeRibbonSpikeState>(m_state),
            generation,
            id,
            winrt::Windows::Foundation::IInspectable{ nullptr });
    }

private:
    struct CommandPeer
    {
        int id = 0;
        size_t pageIndex = 0;
        size_t groupIndex = 0;
        MUXCP::ButtonBase button{ nullptr };
        MUXC::ICommandBarElement element{ nullptr };
        winrt::event_token clickToken{};
    };

    struct GroupPeer
    {
        size_t pageIndex = 0;
        size_t groupIndex = 0;
        bool collapsed = false;
        MUXC::Grid root{ nullptr };
        MUX::UIElement expandedRoot{ nullptr };
        MUXC::CommandBar expandedBar{ nullptr };
        MUXC::CommandBar collapsedBar{ nullptr };
        MUXC::DropDownButton collapsedButton{ nullptr };
        MUXC::Flyout collapsedFlyout{ nullptr };
    };

    static constexpr double CollapsedGroupWidth = 84.0;

    static int ResolveSelectionIndex(
        const MUXC::TabView& tabs,
        const MUXC::SelectionChangedEventArgs& event)
    {
        const auto added = event.AddedItems();
        if ( added && added.Size() != 0 )
        {
            const auto selected = added.GetAt(added.Size() - 1);
            const void * const selectedIdentity = winrt::get_abi(selected);
            const auto items = tabs.TabItems();
            for ( uint32_t index = 0; index < items.Size(); ++index )
            {
                if ( winrt::get_abi(items.GetAt(index)) ==
                     selectedIdentity )
                {
                    return static_cast<int>(index);
                }
            }
        }

        return tabs.SelectedIndex();
    }

    static unsigned long long ArmProgrammaticSelection(
        NativeRibbonSpikeState& state,
        unsigned long long generation,
        int target)
    {
        const unsigned long long serial = ++state.nextSelectionSerial;
        state.programmaticSelections.push_back(
            { generation, serial, target });
        return serial;
    }

    static void CancelProgrammaticSelection(
        NativeRibbonSpikeState& state,
        unsigned long long serial)
    {
        const auto found = std::find_if(
            state.programmaticSelections.begin(),
            state.programmaticSelections.end(),
            [serial](const NativeRibbonSpikeState::
                         ProgrammaticSelectionTicket& ticket)
            {
                return ticket.serial == serial;
            });
        if ( found != state.programmaticSelections.end() )
            state.programmaticSelections.erase(found);
    }

    static bool ConsumeProgrammaticSelection(
        NativeRibbonSpikeState& state,
        unsigned long long generation,
        int target)
    {
        auto& tickets = state.programmaticSelections;
        tickets.erase(
            std::remove_if(
                tickets.begin(), tickets.end(),
                [generation](const NativeRibbonSpikeState::
                                 ProgrammaticSelectionTicket& ticket)
                {
                    return ticket.generation != generation;
                }),
            tickets.end());

        const auto found = std::find_if(
            tickets.begin(), tickets.end(),
            [target](const NativeRibbonSpikeState::
                         ProgrammaticSelectionTicket& ticket)
            {
                return ticket.target == target;
            });
        if ( found == tickets.end() )
            return false;

        // Event delivery is ordered, but WinUI may coalesce successive peer
        // writes. Dropping all tickets through the matching one accounts for
        // both cases without leaving an old target able to mask a later user
        // transition.
        tickets.erase(tickets.begin(), std::next(found));
        return true;
    }

    static bool DispatchCommand(
        const std::weak_ptr<NativeRibbonSpikeState>& weakState,
        unsigned long long generation,
        int id,
        const winrt::Windows::Foundation::IInspectable& sender)
    {
        const std::shared_ptr<NativeRibbonSpikeState> state =
            weakState.lock();
        if ( !state || !state->alive || state->generation != generation )
            return false;

        NativeRibbonCommandModel * const command =
            state->model.FindCommand(id);
        if ( !command || !command->enabled )
            return false;

        if ( command->kind == NativeRibbonCommandKind::Toggle && sender )
        {
            const MUXC::AppBarToggleButton toggle =
                sender.try_as<MUXC::AppBarToggleButton>();
            if ( toggle )
            {
                const auto checked = toggle.IsChecked();
                command->checked = checked && checked.Value();
            }
        }

        ++state->commandEvents[id];
        return true;
    }

    void RevokeHandlers()
    {
        if ( m_tabs && m_hasSizeHandler )
        {
            m_tabs.SizeChanged(m_sizeToken);
            m_hasSizeHandler = false;
            ++gs_nativeRibbonSpikeHandlerRevokes;
        }
        if ( m_tabs && m_hasSelectionHandler )
        {
            m_tabs.SelectionChanged(m_selectionToken);
            m_hasSelectionHandler = false;
            ++gs_nativeRibbonSpikeHandlerRevokes;
        }
        for ( CommandPeer& command : m_commands )
        {
            if ( command.button )
            {
                command.button.Click(command.clickToken);
                ++gs_nativeRibbonSpikeHandlerRevokes;
            }
        }
    }

    GroupPeer *FindGroup(size_t pageIndex, size_t groupIndex)
    {
        for ( GroupPeer& group : m_groups )
        {
            if ( group.pageIndex == pageIndex &&
                 group.groupIndex == groupIndex )
            {
                return &group;
            }
        }
        return nullptr;
    }

    const GroupPeer *FindGroup(size_t pageIndex, size_t groupIndex) const
    {
        return const_cast<NativeRibbonSpikePeer *>(this)->FindGroup(
            pageIndex, groupIndex);
    }

    CommandPeer *FindCommandPeer(int id)
    {
        return const_cast<CommandPeer *>(
            static_cast<const NativeRibbonSpikePeer *>(this)->
                FindCommandPeer(id));
    }

    const CommandPeer *FindCommandPeer(int id) const
    {
        const auto found = std::find_if(
            m_commands.begin(), m_commands.end(),
            [id](const CommandPeer& command) { return command.id == id; });
        return found == m_commands.end() ? nullptr : &*found;
    }

    static unsigned CountIdentity(
        const winrt::Windows::Foundation::Collections::
            IObservableVector<MUXC::ICommandBarElement>& collection,
        const MUXC::ICommandBarElement& element)
    {
        unsigned count = 0;
        const void * const identity = winrt::get_abi(element);
        for ( const MUXC::ICommandBarElement& candidate : collection )
        {
            if ( winrt::get_abi(candidate) == identity )
                ++count;
        }
        return count;
    }

    void ApplyGroupState(GroupPeer& group, bool collapsed)
    {
        if ( group.collapsed == collapsed )
            return;

        MUXC::CommandBar& source =
            collapsed ? group.expandedBar : group.collapsedBar;
        MUXC::CommandBar& target =
            collapsed ? group.collapsedBar : group.expandedBar;
        if ( group.collapsedFlyout.IsOpen() )
            group.collapsedFlyout.Hide();

        std::vector<MUXC::ICommandBarElement> elements;
        for ( const MUXC::ICommandBarElement& element :
              source.PrimaryCommands() )
        {
            elements.push_back(element);
        }
        source.PrimaryCommands().Clear();
        for ( const MUXC::ICommandBarElement& element : elements )
            target.PrimaryCommands().Append(element);

        group.expandedRoot.Visibility(
            collapsed ? MUX::Visibility::Collapsed :
                        MUX::Visibility::Visible);
        group.collapsedButton.Visibility(
            collapsed ? MUX::Visibility::Visible :
                        MUX::Visibility::Collapsed);
        group.collapsed = collapsed;
    }

    std::shared_ptr<NativeRibbonSpikeState> m_state;
    MUXC::TabView m_tabs{ nullptr };
    std::vector<GroupPeer> m_groups;
    std::vector<CommandPeer> m_commands;
    winrt::event_token m_sizeToken{};
    winrt::event_token m_selectionToken{};
    bool m_hasSizeHandler = false;
    bool m_hasSelectionHandler = false;
};

class NativeRibbonSpikeFixture final
{
public:
    ~NativeRibbonSpikeFixture() noexcept
    {
        CleanupNoThrow();
    }

    bool Create()
    {
        if ( m_frame )
            return false;

        m_before = HostSnapshot::Capture();
        m_hasBefore = true;

        wxFrame * const frame = new wxFrame(
            nullptr, wxID_ANY, "WinUI native Ribbon R2 bounded spike",
            wxDefaultPosition, wxSize(1240, 320));
        m_frame = wxWeakRef<wxFrame>(frame);

        wxWindow * const shell = new wxWindow(
            frame, wxID_ANY, wxPoint(0, 0), wxSize(1200, 240),
            wxTAB_TRAVERSAL | wxBORDER_NONE);
        m_shell = wxWeakRef<wxWindow>(shell);

        m_state = std::make_shared<NativeRibbonSpikeState>();
        m_peer = NativeRibbonSpikePeer::Create(m_state);
        if ( !m_peer || !m_peer->Root() )
            return false;

        m_host = wxWinUITopLevelHost::ForWindow(shell, true);
        if ( !m_host )
            return false;

        m_registeredRoot = m_peer->Root();
        m_slot = m_host->RegisterSlot(shell, m_registeredRoot);
        if ( !m_slot )
            return false;
        m_host->SetSlotPreferredFocus(shell, m_registeredRoot);

        if ( !ShowOffscreenWithoutActivation(frame, 84) )
            return false;
        m_host->MarkDirty(shell);
        m_host->FlushSync();
        return true;
    }

    NativeRibbonSpikePeer *GetPeer() const { return m_peer.get(); }
    std::shared_ptr<NativeRibbonSpikeState> GetState() const
        { return m_state; }
    wxFrame *GetFrame() const { return m_frame.get(); }
    wxWindow *GetShell() const { return m_shell.get(); }
    wxWinUITopLevelHost *GetHost() const { return m_host; }

    bool DestroyAndWait(const HostSnapshot& before)
    {
        const wxWeakRef<wxFrame> weakFrame = m_frame;
        const wxWeakRef<wxWindow> weakShell = m_shell;
        const std::weak_ptr<NativeRibbonSpikeState> weakState = m_state;
        CleanupNoThrow();
        return DrainUntil(
            [weakFrame, weakShell, weakState, &before]()
            {
                return !weakFrame && !weakShell && weakState.expired() &&
                       before.IsRestored();
            },
            300);
    }

private:
    void CleanupNoThrow() noexcept
    {
        if ( m_cleaned )
            return;
        m_cleaned = true;

        const wxWeakRef<wxFrame> weakFrame = m_frame;
        const wxWeakRef<wxWindow> weakShell = m_shell;
        const std::weak_ptr<NativeRibbonSpikeState> weakState = m_state;

        // Retire component callbacks/generation first. Even if the XAML tree
        // synchronously calls back while the host detaches it, every ticket is
        // already dead and no raw wx pointer was ever captured.
        if ( m_peer )
            m_peer->Disconnect();

        wxWindow * const shell = m_shell.get();
        wxFrame * const frame = m_frame.get();
        if ( shell && frame && m_host && m_slot )
        {
            try
            {
                // Never unregister a recycled shell or a slot migrated to a
                // different owner/content generation while an assertion was
                // unwinding.
                if ( wxWinUITopLevelHost::FindSlotOwner(shell) == m_host &&
                     m_host->FindSlot(shell) == m_slot &&
                     m_slot->GetContent() == m_registeredRoot )
                {
                    m_host->UnregisterSlot(shell);
                }
            }
            catch ( ... )
            {
                // Destroying the owning TLW below is the terminal host
                // transaction and will retire any still-owned slot.
            }
        }
        m_slot = nullptr;
        m_registeredRoot = nullptr;

        m_peer.reset();
        m_state.reset();

        if ( frame && !frame->IsBeingDeleted() )
        {
            try
            {
                frame->Destroy();
            }
            catch ( ... )
            {
                // wx destruction is normally non-throwing. The weak refs keep
                // this fallback from touching an already retired object.
            }
        }
        m_host = nullptr;

        try
        {
            // Fatal REQUIRE/exception paths do not reach DestroyAndWait().
            // Give the asynchronous wx TLW destruction the same bounded
            // opportunity to restore host/slot/callback baselines.
            DrainUntil(
                [this, weakFrame, weakShell, weakState]()
                {
                    return !weakFrame && !weakShell && weakState.expired() &&
                           (!m_hasBefore || m_before.IsRestored());
                },
                300);
        }
        catch ( ... )
        {
            // Never let cleanup mask the original test failure.
        }
    }

    bool m_cleaned = false;
    bool m_hasBefore = false;
    HostSnapshot m_before;
    wxWeakRef<wxFrame> m_frame;
    wxWeakRef<wxWindow> m_shell;
    std::shared_ptr<NativeRibbonSpikeState> m_state;
    std::shared_ptr<NativeRibbonSpikePeer> m_peer;
    wxWinUITopLevelHost *m_host = nullptr;
    wxWinUISlot *m_slot = nullptr;
    MUX::UIElement m_registeredRoot{ nullptr };
};

} // anonymous namespace

TEST_CASE("WinUIRibbon::GenericFallbackPublicSmoke",
          "[winui-v0-supported][winui-ribbon][winui-ribbon-fallback]")
{
    // This qualifies the public generic wxRibbon fallback hosted by wxWinUI;
    // it is deliberately not a claim that Ribbon has a native WinUI peer.
    struct ThemeRestorer
    {
        wxWinUIAppTheme saved = wxWinUIGetAppTheme();
        bool active = true;

        ~ThemeRestorer()
        {
            if ( active )
                wxWinUISetAppTheme(saved);
        }

        void Restore()
        {
            wxWinUISetAppTheme(saved);
            active = false;
        }
    } restoreTheme;

    wxWinUISetAppTheme(wxWinUIAppTheme::Light);
    REQUIRE(DrainToQuiescence());
    const HostSnapshot before = HostSnapshot::Capture();

    RibbonFixture fixture;
    REQUIRE(fixture.Create(false, 58));
    CheckStablePublicModel(fixture);

    wxRibbonBar * const bar = fixture.GetBar();
    wxRibbonArtProvider * const provider = bar->GetArtProvider();
    REQUIRE(provider);
    CHECK(bar->GetPage(0) == fixture.GetHome());
    CHECK(fixture.GetHome()->GetPanel(0) == fixture.GetButtonPanel());
    CHECK(fixture.GetButtonBar()->GetButtonCount() == 4);
    CHECK(fixture.GetToolBar()->GetToolCount() == 5);
    CHECK(fixture.GetGallery()->GetCount() == 3);
    CHECK(fixture.HasSingleOwner(fixture.GetFrameA()));

    const auto getScheme = [](wxRibbonArtProvider *art)
    {
        std::array<wxColour, 3> scheme;
        art->GetColourScheme(&scheme[0], &scheme[1], &scheme[2]);
        return scheme;
    };

    const std::array<wxColour, 3> lightScheme = getScheme(provider);
    provider->SetColourScheme(wxColour(1, 2, 3),
                              wxColour(4, 5, 6),
                              wxColour(7, 8, 9));
    CHECK(getScheme(provider) != lightScheme);

    int systemColourNotifications = 0;
    bar->Bind(wxEVT_SYS_COLOUR_CHANGED,
              [&systemColourNotifications](wxSysColourChangedEvent& event)
              {
                  ++systemColourNotifications;
                  event.Skip();
              });
    ::SendMessage(static_cast<HWND>(fixture.GetFrameA()->GetHWND()),
                  WM_SYSCOLORCHANGE, 0, 0);
    CHECK(systemColourNotifications > 0);
    CHECK(getScheme(provider) == lightScheme);

    const int lightNotifications = systemColourNotifications;
    wxWinUISetAppTheme(wxWinUIAppTheme::Dark);
    CHECK(systemColourNotifications > lightNotifications);
    const std::array<wxColour, 3> darkScheme = getScheme(provider);
    CHECK(darkScheme != lightScheme);

    const int darkNotifications = systemColourNotifications;
    wxWinUISetAppTheme(wxWinUIAppTheme::Light);
    CHECK(systemColourNotifications > darkNotifications);
    CHECK(getScheme(provider) == lightScheme);

    REQUIRE(fixture.DestroyAndWait(before));
    before.CheckRestored();
    restoreTheme.Restore();
    REQUIRE(DrainToQuiescence());
}

TEST_CASE("WinUIRibbon::DirectAndDefaultCreate",
          "[winui-advanced][winui-ribbon]")
{
    for ( int mode = 0; mode != 2; ++mode )
    {
        DYNAMIC_SECTION("construction mode: " <<
                        (mode == 0 ? "direct constructors" :
                                     "default constructors plus Create"))
        {
            REQUIRE(DrainToQuiescence());
            const HostSnapshot before = HostSnapshot::Capture();

            RibbonFixture fixture;
            REQUIRE(fixture.Create(mode != 0, 60 + mode * 2));
            CheckStablePublicModel(fixture);

            CHECK(fixture.GetBar()->GetParent() == fixture.GetFrameA());
            CHECK(fixture.GetHome()->GetParent() == fixture.GetBar());
            CHECK(fixture.GetButtonPanel()->GetParent() == fixture.GetHome());
            CHECK(fixture.GetButtonBar()->GetParent() ==
                  fixture.GetButtonPanel());
            CHECK(fixture.GetToolBar()->GetParent() == fixture.GetToolPanel());
            CHECK(fixture.GetGallery()->GetParent() ==
                  fixture.GetGalleryPanel());
            CHECK(fixture.GetSentinel()->GetParent() ==
                  fixture.GetSentinelPanel());
            CHECK(fixture.GetBar()->GetPage(0) == fixture.GetHome());
            CHECK(fixture.GetHome()->GetPanel(0) ==
                  fixture.GetButtonPanel());
            CHECK(fixture.HasSingleOwner(fixture.GetFrameA()));

            REQUIRE(fixture.DestroyAndWait(before));
            before.CheckRestored();
        }
    }
}

TEST_CASE("WinUIRibbon::PageModelAndDisplayModes",
          "[winui-advanced][winui-ribbon]")
{
    REQUIRE(DrainToQuiescence());
    const HostSnapshot before = HostSnapshot::Capture();

    RibbonFixture fixture;
    REQUIRE(fixture.Create(false, 64));
    wxRibbonBar * const bar = fixture.GetBar();
    REQUIRE(bar);
    CHECK(bar->GetActivePage() == 0);

    int changing = 0;
    int changed = 0;
    bar->Bind(wxEVT_RIBBONBAR_PAGE_CHANGING,
              [&changing](wxRibbonBarEvent&) { ++changing; });
    bar->Bind(wxEVT_RIBBONBAR_PAGE_CHANGED,
              [&changed](wxRibbonBarEvent&) { ++changed; });

    REQUIRE(bar->SetActivePage(static_cast<size_t>(1)));
    CHECK(bar->GetActivePage() == 1);
    REQUIRE(bar->SetActivePage(fixture.GetThird()));
    CHECK(bar->GetActivePage() == 2);
    CHECK_FALSE(bar->SetActivePage(static_cast<size_t>(99)));
    // Programmatic selection changes are state operations and intentionally
    // do not synthesize the user tab-changing/tab-changed notifications.
    CHECK(changing == 0);
    CHECK(changed == 0);

    bar->HidePage(1);
    CHECK_FALSE(bar->IsPageShown(1));
    bar->ShowPage(1);
    CHECK(bar->IsPageShown(1));
    bar->AddPageHighlight(1);
    CHECK(bar->IsPageHighlighted(1));
    bar->RemovePageHighlight(1);
    CHECK_FALSE(bar->IsPageHighlighted(1));

    bar->ShowPanels(wxRIBBON_BAR_MINIMIZED);
    CHECK(bar->GetDisplayMode() == wxRIBBON_BAR_MINIMIZED);
    CHECK_FALSE(bar->ArePanelsShown());
    bar->ShowPanels(wxRIBBON_BAR_EXPANDED);
    CHECK(bar->GetDisplayMode() == wxRIBBON_BAR_EXPANDED);
    CHECK(bar->ArePanelsShown());
    bar->ShowPanels(wxRIBBON_BAR_PINNED);
    CHECK(bar->GetDisplayMode() == wxRIBBON_BAR_PINNED);
    CHECK(bar->ArePanelsShown());

    // Delete the active middle page: the public contract selects the page
    // which shifted into the same index, without synthesizing user tab events.
    REQUIRE(bar->SetActivePage(static_cast<size_t>(1)));
    CHECK(bar->GetActivePage() == 1);
    const wxWeakRef<wxRibbonPage> weakSecond(fixture.GetSecond());
    bar->DeletePage(1);
    CHECK(bar->GetPageCount() == 2);
    CHECK(bar->GetActivePage() == 1);
    CHECK(bar->GetPage(1) == fixture.GetThird());
    REQUIRE(DrainUntil([weakSecond]() { return !weakSecond; }));
    CHECK(changing == 0);
    CHECK(changed == 0);

    const wxWeakRef<wxRibbonPage> weakHome(fixture.GetHome());
    const wxWeakRef<wxRibbonPage> weakThird(fixture.GetThird());
    const wxWeakRef<wxButton> weakSentinel(fixture.GetSentinel());
    bar->ClearPages();
    CHECK(bar->GetPageCount() == 0);
    CHECK(bar->GetActivePage() == wxNOT_FOUND);
    REQUIRE(DrainUntil(
        [weakHome, weakThird, weakSentinel]()
        {
            return !weakHome && !weakThird && !weakSentinel;
        }));

    REQUIRE(fixture.DestroyAndWait(before));
    before.CheckRestored();
}

TEST_CASE("WinUIRibbon::CommandsGalleryAndPublicEvents",
          "[winui-advanced][winui-ribbon]")
{
    REQUIRE(DrainToQuiescence());
    const HostSnapshot before = HostSnapshot::Capture();

    RibbonFixture fixture;
    REQUIRE(fixture.Create(false, 66));
    wxRibbonButtonBar * const buttons = fixture.GetButtonBar();
    wxRibbonToolBar * const tools = fixture.GetToolBar();
    wxRibbonGallery * const gallery = fixture.GetGallery();
    REQUIRE(buttons);
    REQUIRE(tools);
    REQUIRE(gallery);

    CHECK(buttons->GetItemId(buttons->GetItem(0)) ==
          ID_RIBBON_BUTTON_NORMAL);
    CHECK(buttons->GetItemById(ID_RIBBON_BUTTON_DROPDOWN) ==
          buttons->GetItem(1));
    buttons->EnableButton(ID_RIBBON_BUTTON_NORMAL, false);
    buttons->EnableButton(ID_RIBBON_BUTTON_NORMAL, true);
    buttons->ToggleButton(ID_RIBBON_BUTTON_TOGGLE, true);
    buttons->ToggleButton(ID_RIBBON_BUTTON_TOGGLE, false);
    buttons->SetButtonText(ID_RIBBON_BUTTON_NORMAL, "Normal updated");

    wxBitmap temporaryBitmap;
    REQUIRE(temporaryBitmap.Create(32, 32, 32));
    const wxBitmapBundle temporaryIcon =
        wxBitmapBundle::FromBitmap(temporaryBitmap);
    REQUIRE(buttons->InsertButton(1, ID_RIBBON_BUTTON_TEMPORARY,
                                  "Temporary", temporaryIcon,
                                  "temporary help"));
    CHECK(buttons->GetButtonCount() == 5);
    CHECK(buttons->DeleteButton(ID_RIBBON_BUTTON_TEMPORARY));
    CHECK(buttons->GetButtonCount() == 4);

    CHECK(tools->GetToolKind(ID_RIBBON_TOOL_NORMAL) ==
          wxRIBBON_BUTTON_NORMAL);
    CHECK(tools->GetToolKind(ID_RIBBON_TOOL_DROPDOWN) ==
          wxRIBBON_BUTTON_DROPDOWN);
    CHECK(tools->GetToolKind(ID_RIBBON_TOOL_HYBRID) ==
          wxRIBBON_BUTTON_HYBRID);
    CHECK(tools->GetToolKind(ID_RIBBON_TOOL_TOGGLE) ==
          wxRIBBON_BUTTON_TOGGLE);
    tools->EnableTool(ID_RIBBON_TOOL_NORMAL, false);
    CHECK_FALSE(tools->GetToolEnabled(ID_RIBBON_TOOL_NORMAL));
    tools->EnableTool(ID_RIBBON_TOOL_NORMAL);
    CHECK(tools->GetToolEnabled(ID_RIBBON_TOOL_NORMAL));
    tools->ToggleTool(ID_RIBBON_TOOL_TOGGLE, true);
    CHECK(tools->GetToolState(ID_RIBBON_TOOL_TOGGLE));
    tools->ToggleTool(ID_RIBBON_TOOL_TOGGLE, false);
    CHECK_FALSE(tools->GetToolState(ID_RIBBON_TOOL_TOGGLE));

    int buttonClicks = 0;
    int buttonDropdowns = 0;
    buttons->Bind(wxEVT_RIBBONBUTTONBAR_CLICKED,
                  [&buttonClicks](wxRibbonButtonBarEvent& event)
                  {
                      CHECK(event.GetBar());
                      CHECK(event.GetButton());
                      ++buttonClicks;
                  });
    buttons->Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED,
                  [&buttonDropdowns](wxRibbonButtonBarEvent& event)
                  {
                      CHECK(event.GetBar());
                      CHECK(event.GetButton());
                      ++buttonDropdowns;
                  });
    wxRibbonButtonBarEvent buttonClick(
        wxEVT_RIBBONBUTTONBAR_CLICKED, ID_RIBBON_BUTTON_NORMAL,
        buttons, buttons->GetItemById(ID_RIBBON_BUTTON_NORMAL));
    buttonClick.SetEventObject(buttons);
    CHECK(buttons->ProcessWindowEvent(buttonClick));
    wxRibbonButtonBarEvent buttonDropdown(
        wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED,
        ID_RIBBON_BUTTON_DROPDOWN, buttons,
        buttons->GetItemById(ID_RIBBON_BUTTON_DROPDOWN));
    buttonDropdown.SetEventObject(buttons);
    CHECK(buttons->ProcessWindowEvent(buttonDropdown));
    CHECK(buttonClicks == 1);
    CHECK(buttonDropdowns == 1);

    int toolClicks = 0;
    int toolDropdowns = 0;
    tools->Bind(wxEVT_RIBBONTOOLBAR_CLICKED,
                [&toolClicks](wxRibbonToolBarEvent& event)
                {
                    CHECK(event.GetBar());
                    ++toolClicks;
                });
    tools->Bind(wxEVT_RIBBONTOOLBAR_DROPDOWN_CLICKED,
                [&toolDropdowns](wxRibbonToolBarEvent& event)
                {
                    CHECK(event.GetBar());
                    ++toolDropdowns;
                });
    wxRibbonToolBarEvent toolClick(
        wxEVT_RIBBONTOOLBAR_CLICKED, ID_RIBBON_TOOL_NORMAL, tools);
    toolClick.SetEventObject(tools);
    CHECK(tools->ProcessWindowEvent(toolClick));
    wxRibbonToolBarEvent toolDropdown(
        wxEVT_RIBBONTOOLBAR_DROPDOWN_CLICKED,
        ID_RIBBON_TOOL_DROPDOWN, tools);
    toolDropdown.SetEventObject(tools);
    CHECK(tools->ProcessWindowEvent(toolDropdown));
    CHECK(toolClicks == 1);
    CHECK(toolDropdowns == 1);

    int firstClientToken = 1;
    int replacementClientToken = 2;
    gallery->SetItemClientData(fixture.GetGalleryFirst(), &firstClientToken);
    CHECK(gallery->GetItemClientData(fixture.GetGalleryFirst()) ==
          &firstClientToken);
    gallery->SetSelection(fixture.GetGalleryThird());
    CHECK(gallery->GetSelection() == fixture.GetGalleryThird());
    gallery->EnsureVisible(fixture.GetGalleryThird());

    int hoverEvents = 0;
    int selectedEvents = 0;
    int clickedEvents = 0;
    wxRibbonGalleryItem *replacement = nullptr;
    gallery->Bind(wxEVT_RIBBONGALLERY_HOVER_CHANGED,
                  [&hoverEvents](wxRibbonGalleryEvent& event)
                  {
                      CHECK(event.GetGallery());
                      CHECK(event.GetGalleryItem());
                      ++hoverEvents;
                  });
    gallery->Bind(wxEVT_RIBBONGALLERY_SELECTED,
                  [&selectedEvents](wxRibbonGalleryEvent& event)
                  {
                      CHECK(event.GetGallery());
                      CHECK(event.GetGalleryItem());
                      ++selectedEvents;
                  });
    gallery->Bind(wxEVT_RIBBONGALLERY_CLICKED,
                  [gallery, temporaryIcon, &clickedEvents, &replacement,
                   &replacementClientToken](wxRibbonGalleryEvent& event)
                  {
                      CHECK(event.GetGallery() == gallery);
                      CHECK(event.GetGalleryItem());
                      event.StopPropagation();
                      gallery->Clear();
                      replacement = gallery->Append(
                          temporaryIcon, ID_RIBBON_GALLERY_REPLACEMENT,
                          &replacementClientToken);
                      gallery->SetSelection(replacement);
                      ++clickedEvents;
                  });

    wxRibbonGalleryEvent hover(
        wxEVT_RIBBONGALLERY_HOVER_CHANGED, gallery->GetId(), gallery,
        fixture.GetGalleryFirst());
    hover.SetEventObject(gallery);
    CHECK(gallery->ProcessWindowEvent(hover));
    wxRibbonGalleryEvent selected(
        wxEVT_RIBBONGALLERY_SELECTED, gallery->GetId(), gallery,
        fixture.GetGallerySecond());
    selected.SetEventObject(gallery);
    CHECK(gallery->ProcessWindowEvent(selected));
    wxRibbonGalleryEvent clicked(
        wxEVT_RIBBONGALLERY_CLICKED, gallery->GetId(), gallery,
        fixture.GetGalleryThird());
    clicked.SetEventObject(gallery);
    CHECK(gallery->ProcessWindowEvent(clicked));

    CHECK(hoverEvents == 1);
    CHECK(selectedEvents == 1);
    CHECK(clickedEvents == 1);
    REQUIRE(replacement);
    CHECK(gallery->GetCount() == 1);
    CHECK(gallery->GetSelection() == replacement);
    CHECK(gallery->GetItemClientData(replacement) ==
          &replacementClientToken);

    REQUIRE(fixture.DestroyAndWait(before));
    before.CheckRestored();
}

TEST_CASE("WinUIRibbon::AdaptiveLayoutAndCrossTLWReparent",
          "[winui-advanced][winui-ribbon]")
{
    REQUIRE(DrainToQuiescence());
    const HostSnapshot before = HostSnapshot::Capture();

    RibbonFixture fixture;
    REQUIRE(fixture.Create(false, 68));
    CheckStablePublicModel(fixture);

    REQUIRE(fixture.Resize(1200));
    CheckStablePublicModel(fixture);
    REQUIRE(fixture.Resize(450));
    CheckStablePublicModel(fixture);
    REQUIRE(fixture.Resize(1200));
    CheckStablePublicModel(fixture);

    REQUIRE(fixture.ReparentTo(fixture.GetFrameB()));
    CHECK(fixture.GetBar()->GetParent() == fixture.GetFrameB());
    CHECK(fixture.HasSingleOwner(fixture.GetFrameB()));
    CheckStablePublicModel(fixture);

    REQUIRE(fixture.ReparentTo(fixture.GetFrameA()));
    CHECK(fixture.GetBar()->GetParent() == fixture.GetFrameA());
    CHECK(fixture.HasSingleOwner(fixture.GetFrameA()));
    CheckStablePublicModel(fixture);

    REQUIRE(fixture.DestroyAndWait(before));
    before.CheckRestored();
}

TEST_CASE("WinUIRibbon::ArtProvidersOrientationAndAdaptiveModel",
          "[winui-advanced][winui-ribbon]")
{
    REQUIRE(DrainToQuiescence());
    const HostSnapshot before = HostSnapshot::Capture();

    RibbonFixture fixture;
    REQUIRE(fixture.Create(false, 74));
    wxRibbonBar * const bar = fixture.GetBar();
    REQUIRE(bar);

    const long horizontalStyle =
        wxRIBBON_BAR_DEFAULT_STYLE |
        wxRIBBON_BAR_ALWAYS_SHOW_TABS |
        wxRIBBON_BAR_SHOW_PAGE_ICONS;
    bar->SetWindowStyleFlag(horizontalStyle);
    fixture.GetHome()->SetLabel(
        wxString::FromUTF8("Commandes localis\xC3\xA9" "es "
                           "\xE2\x80\x94 texte long"));

    for ( int providerIndex = 0; providerIndex < 3; ++providerIndex )
    {
        CAPTURE(providerIndex);
        wxRibbonArtProvider *provider = nullptr;
        switch ( providerIndex )
        {
            case 0:
                provider = new wxRibbonMSWArtProvider;
                break;
            case 1:
                provider = new wxRibbonMSWFlatArtProvider;
                break;
            case 2:
                provider = new wxRibbonAUIArtProvider;
                break;
        }
        REQUIRE(provider);

        // SetArtProvider() transfers ownership and synchronously propagates the
        // same provider through the generic Ribbon hierarchy.
        bar->SetArtProvider(provider);
        CHECK(bar->GetArtProvider() == provider);
        CHECK(fixture.GetHome()->GetArtProvider() == provider);
        CHECK(fixture.GetSecond()->GetArtProvider() == provider);
        CHECK(fixture.GetThird()->GetArtProvider() == provider);
        CHECK(fixture.GetButtonPanel()->GetArtProvider() == provider);
        CHECK(fixture.GetToolPanel()->GetArtProvider() == provider);
        CHECK(fixture.GetGalleryPanel()->GetArtProvider() == provider);
        CHECK(fixture.GetSentinelPanel()->GetArtProvider() == provider);
        CHECK(fixture.GetButtonBar()->GetArtProvider() == provider);
        CHECK(fixture.GetToolBar()->GetArtProvider() == provider);
        CHECK(fixture.GetGallery()->GetArtProvider() == provider);
        CHECK(provider->GetFlags() == bar->GetWindowStyleFlag());

        const int metric =
            provider->GetMetric(wxRIBBON_ART_TAB_SEPARATION_SIZE);
        provider->SetMetric(wxRIBBON_ART_TAB_SEPARATION_SIZE, metric + 1);
        CHECK(provider->GetMetric(wxRIBBON_ART_TAB_SEPARATION_SIZE) ==
              metric + 1);
        provider->SetMetric(wxRIBBON_ART_TAB_SEPARATION_SIZE, metric);

        const wxColour primary(31, 71, 113);
        const wxColour secondary(47, 89, 131);
        const wxColour tertiary(67, 109, 151);
        provider->SetColourScheme(primary, secondary, tertiary);
        wxColour actualPrimary;
        wxColour actualSecondary;
        wxColour actualTertiary;
        provider->GetColourScheme(&actualPrimary,
                                  &actualSecondary,
                                  &actualTertiary);
        CHECK(actualPrimary == primary);
        CHECK(actualSecondary == secondary);
        CHECK(actualTertiary == tertiary);

        std::unique_ptr<wxRibbonArtProvider> clone(provider->Clone());
        REQUIRE(clone);
        CHECK(clone.get() != provider);
        CHECK(clone->GetFlags() == provider->GetFlags());
        CHECK(clone->GetMetric(wxRIBBON_ART_TAB_SEPARATION_SIZE) ==
              provider->GetMetric(wxRIBBON_ART_TAB_SEPARATION_SIZE));
        wxColour clonePrimary;
        wxColour cloneSecondary;
        wxColour cloneTertiary;
        clone->GetColourScheme(&clonePrimary,
                               &cloneSecondary,
                               &cloneTertiary);
        CHECK(clonePrimary == primary);
        CHECK(cloneSecondary == secondary);
        CHECK(cloneTertiary == tertiary);

        REQUIRE(fixture.Resize(1200));
        CheckStablePublicModel(fixture);
        CheckPanelOrder(fixture, wxHORIZONTAL);
        const PanelMinimisedState wideMinimised =
            CapturePanelMinimisedState(fixture);
        const PanelRectState wideRects = CapturePanelRectState(fixture);
        const unsigned wideAuxiliaryChildren =
            CountRibbonBarAuxiliaryChildren(bar);

        REQUIRE(fixture.Resize(450));
        CheckStablePublicModel(fixture);
        CheckPanelOrder(fixture, wxHORIZONTAL);
        const PanelMinimisedState narrowMinimised =
            CapturePanelMinimisedState(fixture);
        const PanelRectState narrowRects = CapturePanelRectState(fixture);
        const unsigned narrowAuxiliaryChildren =
            CountRibbonBarAuxiliaryChildren(bar);

        // Art providers are free either to reduce/minimise panels or to keep
        // their public minimum and expose stable page-scroll auxiliaries.
        // Accept those public adaptive outcomes, but reject a layout which did
        // nothing at 450px. Do not call ScrollLines() here: its first call may
        // install the left button and immediately trigger a second collapse
        // transaction which legitimately resets the tentative scroll amount.
        CHECK((CountMinimised(narrowMinimised) >
                   CountMinimised(wideMinimised) ||
               narrowRects != wideRects ||
               narrowAuxiliaryChildren > wideAuxiliaryChildren));

        REQUIRE(fixture.Resize(1200));
        CheckStablePublicModel(fixture);
        CHECK(CapturePanelMinimisedState(fixture) == wideMinimised);
        CHECK(CapturePanelRectState(fixture) == wideRects);
        REQUIRE(DrainUntil(
            [bar, wideAuxiliaryChildren]()
            {
                return CountRibbonBarAuxiliaryChildren(bar) ==
                       wideAuxiliaryChildren;
            }));
    }

    const PanelMinimisedState horizontalMinimised =
        CapturePanelMinimisedState(fixture);
    bar->SetWindowStyleFlag(horizontalStyle | wxRIBBON_BAR_FLOW_VERTICAL);
    REQUIRE(fixture.Resize(480, 900));
    CHECK((bar->GetWindowStyleFlag() & wxRIBBON_BAR_FLOW_VERTICAL) != 0);
    CHECK(bar->GetArtProvider()->GetFlags() ==
          bar->GetWindowStyleFlag());
    CHECK(fixture.GetHome()->GetMajorAxis() == wxVERTICAL);
    CheckPanelOrder(fixture, wxVERTICAL);
    CheckStablePublicModel(fixture);

    bar->SetWindowStyleFlag(horizontalStyle);
    REQUIRE(fixture.Resize(1200));
    CHECK((bar->GetWindowStyleFlag() & wxRIBBON_BAR_FLOW_VERTICAL) == 0);
    CHECK(fixture.GetHome()->GetMajorAxis() == wxHORIZONTAL);
    CheckPanelOrder(fixture, wxHORIZONTAL);
    CheckStablePublicModel(fixture);
    CHECK(CapturePanelMinimisedState(fixture) == horizontalMinimised);

    REQUIRE(fixture.DestroyAndWait(before));
    before.CheckRestored();
}

TEST_CASE("WinUIRibbon::BarAndPanelPublicEventMatrix",
          "[winui-advanced][winui-ribbon]")
{
    REQUIRE(DrainToQuiescence());
    const HostSnapshot before = HostSnapshot::Capture();

    RibbonFixture fixture;
    REQUIRE(fixture.Create(false, 76));
    wxRibbonBar * const bar = fixture.GetBar();
    wxRibbonPage * const page = fixture.GetSecond();
    wxRibbonPanel * const panel = fixture.GetButtonPanel();
    REQUIRE(bar);
    REQUIRE(page);
    REQUIRE(panel);

    int changingEvents = 0;
    bar->Bind(wxEVT_RIBBONBAR_PAGE_CHANGING,
              [&changingEvents, bar, page](wxRibbonBarEvent& event)
              {
                  CHECK(event.GetEventObject() == bar);
                  CHECK(event.GetPage() == page);
                  event.Veto();
                  ++changingEvents;
              });
    wxRibbonBarEvent changing(wxEVT_RIBBONBAR_PAGE_CHANGING,
                              bar->GetId(), page);
    changing.SetEventObject(bar);
    CHECK(bar->ProcessWindowEvent(changing));
    CHECK(changingEvents == 1);
    CHECK_FALSE(changing.IsAllowed());
    CHECK(bar->GetActivePage() == 0);

    int barEvents = 0;
    const auto handler = [&barEvents, bar, page](wxRibbonBarEvent& event)
    {
        CHECK(event.GetEventObject() == bar);
        CHECK(event.GetId() == bar->GetId());
        CHECK(event.GetPage() == page);
        ++barEvents;
    };
    bar->Bind(wxEVT_RIBBONBAR_PAGE_CHANGED, handler);
    bar->Bind(wxEVT_RIBBONBAR_TAB_MIDDLE_DOWN, handler);
    bar->Bind(wxEVT_RIBBONBAR_TAB_MIDDLE_UP, handler);
    bar->Bind(wxEVT_RIBBONBAR_TAB_RIGHT_DOWN, handler);
    bar->Bind(wxEVT_RIBBONBAR_TAB_RIGHT_UP, handler);
    bar->Bind(wxEVT_RIBBONBAR_TAB_LEFT_DCLICK, handler);
    bar->Bind(wxEVT_RIBBONBAR_TOGGLED, handler);
    bar->Bind(wxEVT_RIBBONBAR_HELP_CLICK, handler);

    const auto dispatch = [bar, page](wxEventType type)
    {
        wxRibbonBarEvent event(type, bar->GetId(), page);
        event.SetEventObject(bar);
        return bar->ProcessWindowEvent(event);
    };
    CHECK(dispatch(wxEVT_RIBBONBAR_PAGE_CHANGED));
    CHECK(dispatch(wxEVT_RIBBONBAR_TAB_MIDDLE_DOWN));
    CHECK(dispatch(wxEVT_RIBBONBAR_TAB_MIDDLE_UP));
    CHECK(dispatch(wxEVT_RIBBONBAR_TAB_RIGHT_DOWN));
    CHECK(dispatch(wxEVT_RIBBONBAR_TAB_RIGHT_UP));
    CHECK(dispatch(wxEVT_RIBBONBAR_TAB_LEFT_DCLICK));
    CHECK(dispatch(wxEVT_RIBBONBAR_TOGGLED));
    CHECK(dispatch(wxEVT_RIBBONBAR_HELP_CLICK));
    CHECK(barEvents == 8);
    CHECK(bar->GetActivePage() == 0);

    REQUIRE(panel->HasExtButton());
    int panelEvents = 0;
    panel->Bind(wxEVT_RIBBONPANEL_EXTBUTTON_ACTIVATED,
                [&panelEvents, panel](wxRibbonPanelEvent& event)
                {
                    CHECK(event.GetEventObject() == panel);
                    CHECK(event.GetPanel() == panel);
                    ++panelEvents;
                });
    wxRibbonPanelEvent panelEvent(
        wxEVT_RIBBONPANEL_EXTBUTTON_ACTIVATED, panel->GetId(), panel);
    panelEvent.SetEventObject(panel);
    CHECK(panel->ProcessWindowEvent(panelEvent));
    CHECK(panelEvents == 1);

    REQUIRE(fixture.DestroyAndWait(before));
    before.CheckRestored();
}

static void RunPageOverflowPublicScrolling(int providerIndex)
{
    REQUIRE(DrainToQuiescence());
    const HostSnapshot before = HostSnapshot::Capture();

    RibbonFixture fixture;
    REQUIRE(fixture.Create(false, 78 + providerIndex * 4));
    wxRibbonBar * const bar = fixture.GetBar();
    wxRibbonPage * const home = fixture.GetHome();
    REQUIRE(bar);
    REQUIRE(home);

    wxRibbonArtProvider *provider = nullptr;
    switch ( providerIndex )
    {
        case 0:
            provider = new wxRibbonMSWArtProvider;
            break;
        case 1:
            provider = new wxRibbonMSWFlatArtProvider;
            break;
        case 2:
            provider = new wxRibbonAUIArtProvider;
            break;
    }
    REQUIRE(provider);
    bar->SetArtProvider(provider);
    CHECK(bar->GetArtProvider() == provider);

    wxRibbonPanel * const overflowPanelA = CreatePanel(
        home, "Non-minimising commands A", false,
        wxRIBBON_PANEL_NO_AUTO_MINIMISE);
    wxRibbonPanel * const overflowPanelB = CreatePanel(
        home, "Non-minimising commands B", false,
        wxRIBBON_PANEL_NO_AUTO_MINIMISE);
    REQUIRE(overflowPanelA);
    REQUIRE(overflowPanelB);

    wxButton * const overflowButtonA = new wxButton(
        overflowPanelA, wxID_ANY, "A deterministic wide command group");
    wxButton * const overflowButtonB = new wxButton(
        overflowPanelB, wxID_ANY, "A second deterministic wide command group");
    overflowButtonA->SetMinSize(wxSize(340, 60));
    overflowButtonB->SetMinSize(wxSize(340, 60));

    wxBoxSizer * const sizerA = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer * const sizerB = new wxBoxSizer(wxVERTICAL);
    sizerA->Add(overflowButtonA, 1, wxEXPAND | wxALL, 3);
    sizerB->Add(overflowButtonB, 1, wxEXPAND | wxALL, 3);
    overflowPanelA->SetSizer(sizerA);
    overflowPanelB->SetSizer(sizerB);

    const wxWeakRef<wxRibbonPanel> weakOverflowPanelA(overflowPanelA);
    const wxWeakRef<wxRibbonPanel> weakOverflowPanelB(overflowPanelB);
    const wxWeakRef<wxButton> weakOverflowButtonA(overflowButtonA);
    const wxWeakRef<wxButton> weakOverflowButtonB(overflowButtonB);

    REQUIRE(fixture.Resize(1200));
    std::vector<wxWeakRef<wxWindow>> barChildrenBeforeOverflow;
    for ( wxWindowList::compatibility_iterator node =
              bar->GetChildren().GetFirst();
          node;
          node = node->GetNext() )
    {
        barChildrenBeforeOverflow.emplace_back(node->GetData());
    }
    // The two 340px NO_AUTO_MINIMISE panels cannot both fit this viewport for
    // any provider. Unlike the adaptive 450px gate, this is deliberately a
    // stable overflow oracle, not a transient scroll-button/collapse race.
    REQUIRE(fixture.Resize(360));
    CHECK_FALSE(overflowPanelA->IsMinimised());
    CHECK_FALSE(overflowPanelB->IsMinimised());

    const wxPoint initialPosition = overflowPanelA->GetPosition();
    REQUIRE(home->ScrollLines(1));
    unsigned forwardLines = 1;

    // Page scroll buttons are private implementation classes. Track only the
    // new wxWindow children appearing after overflow, so this remains valid if
    // the bar gains another permanent auxiliary child in the future.
    std::vector<wxWeakRef<wxWindow>> scrollButtonWindows;
    for ( wxWindowList::compatibility_iterator node =
              bar->GetChildren().GetFirst();
          node;
          node = node->GetNext() )
    {
        wxWindow * const child = node->GetData();
        bool existedBeforeOverflow = false;
        for ( const wxWeakRef<wxWindow>& weak : barChildrenBeforeOverflow )
        {
            if ( weak.get() == child )
            {
                existedBeforeOverflow = true;
                break;
            }
        }
        if ( !existedBeforeOverflow )
            scrollButtonWindows.emplace_back(child);
    }
    REQUIRE_FALSE(scrollButtonWindows.empty());

    while ( forwardLines < 512 && home->ScrollLines(1) )
        ++forwardLines;
    CHECK(forwardLines < 512);
    CHECK_FALSE(home->ScrollLines(1));
    CHECK(overflowPanelA->GetPosition().x < initialPosition.x);

    unsigned reverseLines = 0;
    while ( reverseLines < 512 && home->ScrollLines(-1) )
        ++reverseLines;
    REQUIRE(reverseLines > 0);
    CHECK(reverseLines < 512);
    CHECK_FALSE(home->ScrollLines(-1));
    CHECK(overflowPanelA->GetPosition() == initialPosition);

    REQUIRE(fixture.Resize(1200));
    REQUIRE(DrainUntil(
        [&scrollButtonWindows]()
        {
            for ( const wxWeakRef<wxWindow>& weak : scrollButtonWindows )
            {
                if ( weak )
                    return false;
            }
            return true;
        }));

    REQUIRE(fixture.DestroyAndWait(before));
    REQUIRE(DrainUntil(
        [weakOverflowPanelA, weakOverflowPanelB,
         weakOverflowButtonA, weakOverflowButtonB]()
        {
            return !weakOverflowPanelA && !weakOverflowPanelB &&
                   !weakOverflowButtonA && !weakOverflowButtonB;
        }));
    before.CheckRestored();
}

TEST_CASE("WinUIRibbon::PageOverflowPublicScrolling",
          "[winui-advanced][winui-ribbon]")
{
    for ( int providerIndex = 0; providerIndex < 3; ++providerIndex )
    {
        DYNAMIC_SECTION("art provider " << providerIndex)
        {
            RunPageOverflowPublicScrolling(providerIndex);
        }
    }
}

TEST_CASE("WinUIRibbon::GalleryOverflowPublicScrolling",
          "[winui-advanced][winui-ribbon]")
{
    REQUIRE(DrainToQuiescence());
    const HostSnapshot before = HostSnapshot::Capture();

    RibbonFixture fixture;
    REQUIRE(fixture.Create(false, 80));
    wxRibbonGallery * const gallery = fixture.GetGallery();
    REQUIRE(gallery);

    wxBitmap bitmap;
    REQUIRE(bitmap.Create(32, 32, 32));
    const wxBitmapBundle icon = wxBitmapBundle::FromBitmap(bitmap);
    REQUIRE(icon.IsOk());
    for ( int item = 0; item < 29; ++item )
    {
        REQUIRE(gallery->Append(icon, wxID_HIGHEST + 5200 + item));
    }
    CHECK(gallery->GetCount() == 32);

    wxRibbonGalleryItem * const selection = fixture.GetGallerySecond();
    REQUIRE(selection);
    gallery->SetSelection(selection);

    const wxSize minimum = gallery->GetMinSize();
    const wxSize best = gallery->GetBestSize();
    REQUIRE(minimum.GetWidth() > 0);
    REQUIRE(minimum.GetHeight() > 0);
    CHECK(best.GetWidth() >= minimum.GetWidth());
    CHECK(best.GetHeight() >= minimum.GetHeight());

    const wxSize smaller =
        gallery->GetNextSmallerSize(wxHORIZONTAL, best);
    const wxSize larger =
        gallery->GetNextLargerSize(wxHORIZONTAL, minimum);
    CHECK(smaller.GetWidth() < best.GetWidth());
    CHECK(smaller.GetHeight() == best.GetHeight());
    CHECK(larger.GetWidth() > minimum.GetWidth());
    CHECK(larger.GetHeight() == minimum.GetHeight());

    // Constrain the gallery using its art-provider-derived public minimum,
    // not a theme/DPI-specific pixel constant. With 32 same-sized items this
    // necessarily exercises the public scroll model.
    gallery->SetSize(minimum);
    REQUIRE(gallery->Realize());
    CHECK(gallery->GetUpButtonState() ==
          wxRIBBON_GALLERY_BUTTON_DISABLED);
    CHECK_FALSE(gallery->ScrollLines(-1));

    unsigned forwardLines = 0;
    while ( forwardLines < 512 && gallery->ScrollLines(1) )
        ++forwardLines;
    REQUIRE(forwardLines > 0);
    CHECK(forwardLines < 512);
    CHECK_FALSE(gallery->ScrollLines(1));
    CHECK(gallery->GetDownButtonState() ==
          wxRIBBON_GALLERY_BUTTON_DISABLED);
    CHECK(gallery->GetSelection() == selection);

    unsigned reverseLines = 0;
    while ( reverseLines < 512 && gallery->ScrollLines(-1) )
        ++reverseLines;
    REQUIRE(reverseLines > 0);
    CHECK(reverseLines < 512);
    CHECK_FALSE(gallery->ScrollLines(-1));
    CHECK(gallery->GetUpButtonState() ==
          wxRIBBON_GALLERY_BUTTON_DISABLED);
    CHECK(gallery->GetSelection() == selection);

    gallery->Clear();
    CHECK(gallery->IsEmpty());
    CHECK(gallery->GetCount() == 0);
    CHECK(gallery->GetSelection() == nullptr);
    REQUIRE(gallery->Realize());
    CHECK_FALSE(gallery->ScrollLines(-1));
    CHECK_FALSE(gallery->ScrollLines(1));
    CHECK(gallery->GetUpButtonState() ==
          wxRIBBON_GALLERY_BUTTON_DISABLED);
    CHECK(gallery->GetDownButtonState() ==
          wxRIBBON_GALLERY_BUTTON_DISABLED);

    REQUIRE(fixture.DestroyAndWait(before));
    before.CheckRestored();
}

TEST_CASE("WinUIRibbon::CrossTLWStress",
          "[winui-advanced][winui-ribbon][stress]")
{
    REQUIRE(DrainToQuiescence());
    const HostSnapshot before = HostSnapshot::Capture();

    RibbonFixture fixture;
    REQUIRE(fixture.Create(false, 70));
    wxRibbonBar * const bar = fixture.GetBar();
    wxRibbonToolBar * const tools = fixture.GetToolBar();
    wxRibbonGallery * const gallery = fixture.GetGallery();
    REQUIRE(bar);
    REQUIRE(tools);
    REQUIRE(gallery);

    for ( unsigned cycle = 0; cycle < 100; ++cycle )
    {
        CAPTURE(cycle);
        REQUIRE(fixture.ReparentTo(fixture.GetFrameB()));
        tools->ToggleTool(ID_RIBBON_TOOL_TOGGLE, (cycle & 1) != 0);
        gallery->SetSelection((cycle & 1) ? fixture.GetGalleryFirst() :
                                            fixture.GetGalleryThird());
        REQUIRE(bar->SetActivePage(
            static_cast<size_t>((cycle % 2) + 1)));
        CHECK(fixture.HasSingleOwner(fixture.GetFrameB()));

        REQUIRE(fixture.ReparentTo(fixture.GetFrameA()));
        REQUIRE(bar->SetActivePage(static_cast<size_t>(0)));
        gallery->SetSelection(fixture.GetGallerySecond());
        CHECK(fixture.HasSingleOwner(fixture.GetFrameA()));
        CHECK(bar->GetPageCount() == 3);
        CHECK(gallery->GetCount() == 3);
    }

    REQUIRE(fixture.DestroyAndWait(before));
    before.CheckRestored();
}

TEST_CASE("WinUIRibbon::NativeR2BoundedSharedHostSpike",
          "[winui-advanced][winui-ribbon][native-spike][stress]")
{
    REQUIRE(DrainToQuiescence());
    const HostSnapshot before = HostSnapshot::Capture();
    const unsigned statesBefore = gs_nativeRibbonSpikeStates;
    const unsigned peersBefore = gs_nativeRibbonSpikePeers;
    const unsigned handlerAddsBefore =
        gs_nativeRibbonSpikeHandlerAdds;
    const unsigned handlerRevokesBefore =
        gs_nativeRibbonSpikeHandlerRevokes;

    NativeRibbonSpikeFixture fixture;
    REQUIRE(fixture.Create());
    NativeRibbonSpikePeer * const peer = fixture.GetPeer();
    std::shared_ptr<NativeRibbonSpikeState> state = fixture.GetState();
    wxWindow * const shell = fixture.GetShell();
    wxWinUITopLevelHost * const host = fixture.GetHost();
    REQUIRE(peer);
    REQUIRE(state);
    REQUIRE(shell);
    REQUIRE(host);
    REQUIRE(peer->Root());
    CHECK(gs_nativeRibbonSpikeStates == statesBefore + 1);
    CHECK(gs_nativeRibbonSpikePeers == peersBefore + 1);
    REQUIRE(DrainUntil(
        [host, shell, peer, state]()
        {
            const wxWinUISlot * const current = host->FindSlot(shell);
            return current && current->GetContent() == peer->Root() &&
                   peer->Root().XamlRoot() != nullptr &&
                   peer->Root().ActualWidth() > 100 &&
                   state->layoutEvents > 0 &&
                   state->lastActualWidth == peer->Root().ActualWidth();
        },
        240));

    // Exactly one new TLW host and one slot: the test peer uses neither a
    // second DesktopWindowXamlSource nor a private dispatcher.
    CHECK(wxWinUITopLevelHost::GetLiveHostCount() == before.hosts + 1);
    CHECK(wxWinUITopLevelHost::GetLiveSlotCount() == before.slots + 1);
    CHECK(wxWinUITopLevelHost::FindSlotOwner(shell) == host);
    CHECK(peer->GetCollapsedCount(0) == 0);
    REQUIRE(peer->EveryCommandHasOneCollection());
    DrainDispatch(3);
    CHECK(state->selectionChangingEvents == 0);
    CHECK(state->selectionChangedEvents == 0);
    CHECK(state->selectionVetoes == 0);
    CHECK(state->selectionFailures == 0);
    CHECK(state->layoutEvents > 0);
    CHECK(state->layoutFailures == 0);
    CHECK(state->lastActualWidth == peer->Root().ActualWidth());

    const uintptr_t pasteIdentity = peer->GetCommandIdentity(6101);
    const uintptr_t pinIdentity = peer->GetCommandIdentity(6102);
    REQUIRE(pasteIdentity != 0);
    REQUIRE(pinIdentity != 0);

    // Programmatic selection is silent. A XAML-originated transition takes
    // the veto path in the callback itself and restores the authoritative wx
    // model without publishing a changed event.
    REQUIRE(peer->SetActivePageProgrammatically(1));
    REQUIRE(DrainUntil(
        [state]() { return state->programmaticSelections.empty(); },
        120));
    CHECK(state->model.activePage == 1);
    CHECK(state->selectionChangingEvents == 0);
    CHECK(state->selectionChangedEvents == 0);
    CHECK(state->selectionFailures == 0);

    REQUIRE(peer->SelectPageAsUser(2, true));
    REQUIRE(DrainUntil(
        [peer, state]()
        {
            return !state->userSelectionPending &&
                   state->programmaticSelections.empty() &&
                   peer->Root().SelectedIndex() == 1;
        },
        120));
    CHECK(state->model.activePage == 1);
    CHECK(peer->Root().SelectedIndex() == 1);
    CHECK(state->selectionChangingEvents == 1);
    CHECK(state->selectionChangedEvents == 0);
    CHECK(state->selectionVetoes == 1);
    CHECK(state->selectionFailures == 0);

    REQUIRE(peer->SelectPageAsUser(2, false));
    REQUIRE(DrainUntil(
        [state]()
        {
            return !state->userSelectionPending &&
                   state->model.activePage == 2;
        },
        120));
    CHECK(state->model.activePage == 2);
    CHECK(peer->Root().SelectedIndex() == 2);
    CHECK(state->selectionChangingEvents == 2);
    CHECK(state->selectionChangedEvents == 1);
    REQUIRE(peer->SetActivePageProgrammatically(0));
    REQUIRE(DrainUntil(
        [state]() { return state->programmaticSelections.empty(); },
        120));
    CHECK(state->model.activePage == 0);
    CHECK(state->selectionChangingEvents == 2);
    CHECK(state->selectionChangedEvents == 1);
    CHECK(state->selectionVetoes == 1);
    CHECK(state->selectionFailures == 0);

    REQUIRE(peer->SetToggleState(6102, true));

    bool invokedFromCollapsedFlyout = false;
    for ( unsigned cycle = 0; cycle < 100; ++cycle )
    {
        CAPTURE(cycle);
        const unsigned narrowLayoutEvent = state->layoutEvents;
        shell->SetSize(wxSize(450, 240));
        host->MarkDirty(shell);
        host->FlushSync();
        REQUIRE(DrainUntil(
            [peer, state, narrowLayoutEvent]()
            {
                return state->layoutEvents > narrowLayoutEvent &&
                       peer->Root().ActualWidth() < 600 &&
                       peer->GetCollapsedCount(0) >= 2;
            },
            240));
        CHECK(peer->GetCollapsedCount(0) >= 2);
        CHECK(peer->EveryCommandHasOneCollection());
        CHECK(peer->GetCommandIdentity(6101) == pasteIdentity);
        CHECK(peer->GetCommandIdentity(6102) == pinIdentity);
        CHECK(state->lastActualWidth == peer->Root().ActualWidth());
        CHECK(state->layoutFailures == 0);

        if ( cycle == 0 )
        {
            // Exercise the command while its unchanged peer is really hosted
            // by the open reduced-group flyout, not by the expanded bar.
            REQUIRE(peer->IsCommandInCollapsedCollection(6101));
            REQUIRE(peer->ShowCollapsedGroup(0, 0));
            REQUIRE(DrainUntil(
                [peer]() { return peer->IsGroupFlyoutOpen(0, 0); },
                120));
            REQUIRE(peer->InvokeCommand(6101));
            CHECK(state->commandEvents[6101] == 1);
            invokedFromCollapsedFlyout = true;
        }

        const unsigned wideLayoutEvent = state->layoutEvents;
        shell->SetSize(wxSize(1200, 240));
        host->MarkDirty(shell);
        host->FlushSync();
        REQUIRE(DrainUntil(
            [peer, state, wideLayoutEvent]()
            {
                return state->layoutEvents > wideLayoutEvent &&
                       peer->Root().ActualWidth() > 900 &&
                       peer->GetCollapsedCount(0) == 0;
            },
            240));
        CHECK(peer->GetCollapsedCount(0) == 0);
        CHECK_FALSE(peer->IsGroupFlyoutOpen(0, 0));
        CHECK(peer->EveryCommandHasOneCollection());
        CHECK(peer->GetCommandIdentity(6101) == pasteIdentity);
        CHECK(peer->GetCommandIdentity(6102) == pinIdentity);
        CHECK(state->lastActualWidth == peer->Root().ActualWidth());
        CHECK(state->layoutFailures == 0);
    }
    CHECK(invokedFromCollapsedFlyout);

    bool checked = false;
    REQUIRE(peer->GetToggleState(6102, &checked));
    CHECK(checked);
    CHECK(state->model.FindCommand(6102)->checked);
    CHECK(state->model.activePage == 0);
    CHECK(state->model.GetCommandCount() == 11);

    // Rebuild from a model delta. All old handlers are revoked first, the
    // generation advances, and the replacement obtains a new peer identity.
    const unsigned long long retiredGeneration = state->generation;
    REQUIRE(state->model.ReplaceCommand(
        6101,
        { 6103, L"Paste special", NativeRibbonCommandKind::Normal }));
    REQUIRE(peer->Rebuild());
    CHECK(peer->GetCommandIdentity(6101) == 0);
    CHECK(peer->GetCommandIdentity(6103) != 0);
    CHECK(peer->GetCommandIdentity(6103) != pasteIdentity);
    CHECK_FALSE(peer->DispatchRetiredGenerationForTest(
        retiredGeneration, 6101));
    CHECK(state->commandEvents[6101] == 1);
    REQUIRE(peer->InvokeCommand(6103));
    CHECK(state->commandEvents[6103] == 1);
    CHECK(peer->EveryCommandHasOneCollection());

    // Leave a real reduced-group flyout open for the terminal transaction.
    // Disconnect() must first retire callbacks/generation, then close/release
    // the transient tree before the common host detaches its exact slot.
    const unsigned terminalLayoutEvent = state->layoutEvents;
    shell->SetSize(wxSize(450, 240));
    host->MarkDirty(shell);
    host->FlushSync();
    REQUIRE(DrainUntil(
        [peer, state, terminalLayoutEvent]()
        {
            return state->layoutEvents > terminalLayoutEvent &&
                   peer->Root().ActualWidth() < 600 &&
                   peer->GetCollapsedCount(0) >= 2;
        },
        240));
    REQUIRE(peer->IsCommandInCollapsedCollection(6103));
    REQUIRE(peer->ShowCollapsedGroup(0, 0));
    REQUIRE(DrainUntil(
        [peer]() { return peer->IsGroupFlyoutOpen(0, 0); },
        120));
    CHECK(state->layoutFailures == 0);

    state.reset();
    REQUIRE(fixture.DestroyAndWait(before));
    REQUIRE(DrainToQuiescence());

    CHECK(gs_nativeRibbonSpikeStates == statesBefore);
    CHECK(gs_nativeRibbonSpikePeers == peersBefore);
    const unsigned handlerAdds =
        gs_nativeRibbonSpikeHandlerAdds - handlerAddsBefore;
    const unsigned handlerRevokes =
        gs_nativeRibbonSpikeHandlerRevokes - handlerRevokesBefore;
    CHECK(handlerAdds > 0);
    CHECK(handlerAdds == handlerRevokes);
    before.CheckRestored();
}

#if wxUSE_XRC

TEST_CASE("WinUIRibbon::XrcSupportedSurface",
          "[winui-advanced][winui-ribbon][xrc]")
{
    REQUIRE(DrainToQuiescence());
    const HostSnapshot before = HostSnapshot::Capture();

    static const char xrcText[] = R"XRC(
<resource xmlns="http://www.wxwidgets.org/wxxrc" version="2.5.3.0">
  <object class="wxRibbonBar" name="winuiRibbonXrcBar">
    <object class="page" name="winuiRibbonXrcPage">
      <label>XRC page</label>
      <object class="panel" name="winuiRibbonXrcButtonsPanel">
        <label>Buttons</label>
        <object class="wxRibbonButtonBar" name="winuiRibbonXrcButtons">
          <object class="button" name="winuiRibbonXrcButton">
            <label>XRC button</label>
            <bitmap stock_id="wxART_INFORMATION" stock_client="wxART_OTHER"/>
          </object>
        </object>
      </object>
      <object class="panel" name="winuiRibbonXrcGalleryPanel">
        <label>Gallery</label>
        <object class="wxRibbonGallery" name="winuiRibbonXrcGallery">
          <object class="item" name="winuiRibbonXrcGalleryItem">
            <bitmap stock_id="wxART_INFORMATION" stock_client="wxART_OTHER"/>
          </object>
        </object>
      </object>
    </object>
  </object>
</resource>
)XRC";

    wxXmlResource resource;
    resource.AddHandler(new wxRibbonXmlHandler);
    wxStringInputStream stream(wxString::FromUTF8(xrcText));
    std::unique_ptr<wxXmlDocument> document(new wxXmlDocument(stream));
    REQUIRE(document->IsOk());
    REQUIRE(resource.LoadDocument(document.release(), "winui-ribbon-r1.xrc"));

    std::unique_ptr<wxFrame> frame(new wxFrame(
        nullptr, wxID_ANY, "WinUI ribbon XRC",
        wxDefaultPosition, wxSize(800, 300)));
    wxRibbonBar * const bar = wxDynamicCast(
        resource.LoadObject(frame.get(), "winuiRibbonXrcBar", "wxRibbonBar"),
        wxRibbonBar);
    REQUIRE(bar);
    REQUIRE(bar->GetPageCount() == 1);
    wxRibbonPage * const page = bar->GetPage(0);
    REQUIRE(page);
    CHECK(page->GetPanelCount() == 2);
    wxRibbonPanel * const buttonsPanel = page->GetPanel(0);
    wxRibbonPanel * const galleryPanel = page->GetPanel(1);
    REQUIRE(buttonsPanel);
    REQUIRE(galleryPanel);

    wxRibbonButtonBar * const buttons = wxDynamicCast(
        frame->FindWindow(XRCID("winuiRibbonXrcButtons")),
        wxRibbonButtonBar);
    wxRibbonGallery * const gallery = wxDynamicCast(
        frame->FindWindow(XRCID("winuiRibbonXrcGallery")),
        wxRibbonGallery);
    REQUIRE(buttons);
    REQUIRE(gallery);
    CHECK(buttons->GetButtonCount() == 1);
    CHECK(gallery->GetCount() == 1);

    // wxRibbonXmlHandler currently has no wxRibbonToolBar branch. Add a real
    // WinUI child separately so the XRC object's teardown still participates
    // in the same host/slot lifetime gate as the programmatic matrix.
    wxRibbonPanel * const sentinelPanel =
        new wxRibbonPanel(page, wxID_ANY, "WinUI sentinel");
    wxButton * const sentinel = new wxButton(
        sentinelPanel, wxID_ANY, "XRC ribbon slot sentinel");
    wxBoxSizer * const sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(sentinel, 1, wxEXPAND | wxALL, 3);
    sentinelPanel->SetSizer(sizer);

    frame->SetClientSize(wxSize(800, 280));
    bar->SetSize(frame->GetClientRect());
    REQUIRE(bar->Realize());
    REQUIRE(ShowOffscreenWithoutActivation(frame.get(), 72));
    REQUIRE(DrainUntil(
        [framePtr = frame.get(), sentinel]()
        {
            wxWinUITopLevelHost * const host =
                wxWinUITopLevelHost::FindForTLW(framePtr);
            return host &&
                   wxWinUITopLevelHost::FindSlotOwner(sentinel) == host &&
                   host->FindSlot(sentinel);
        }));

    const wxWeakRef<wxFrame> weakFrame(frame.get());
    const wxWeakRef<wxRibbonBar> weakBar(bar);
    const wxWeakRef<wxRibbonPage> weakPage(page);
    const wxWeakRef<wxRibbonPanel> weakButtonsPanel(buttonsPanel);
    const wxWeakRef<wxRibbonPanel> weakGalleryPanel(galleryPanel);
    const wxWeakRef<wxRibbonPanel> weakSentinelPanel(sentinelPanel);
    const wxWeakRef<wxRibbonButtonBar> weakButtons(buttons);
    const wxWeakRef<wxRibbonGallery> weakGallery(gallery);
    const wxWeakRef<wxButton> weakSentinel(sentinel);
    frame.reset();
    REQUIRE(DrainUntil(
        [weakFrame, weakBar, weakPage, weakButtonsPanel,
         weakGalleryPanel, weakSentinelPanel, weakButtons, weakGallery,
         weakSentinel, &before]()
        {
            return !weakFrame && !weakBar && !weakPage &&
                   !weakButtonsPanel && !weakGalleryPanel &&
                   !weakSentinelPanel && !weakButtons && !weakGallery &&
                   !weakSentinel &&
                   before.IsRestored();
        }));
    REQUIRE(DrainToQuiescence());
    before.CheckRestored();
}

#endif // wxUSE_XRC

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_RIBBON
