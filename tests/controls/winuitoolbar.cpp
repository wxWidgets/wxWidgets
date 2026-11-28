///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuitoolbar.cpp
// Purpose:     the tool bar of wxWinUI: what its CommandBar shows and sends
// Author:      wxWidgets development team
// Created:     2026-07-26
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_TOOLBAR

#include "wx/app.h"
#include "wx/bitmap.h"
#include "wx/menu.h"
#include "wx/toolbar.h"
#include "wx/utils.h"
#include "../winui/test-support/toolbar-test-access.h"

#if wxUSE_BUTTON
    #include "wx/button.h"
#endif

namespace
{

using Access = wxWinUIToolBarTestAccess;

wxBitmapBundle MakeToolbarBitmapBundle(int dip = 16)
{
    wxBitmap one;
    wxBitmap two;
    REQUIRE(one.CreateWithDIPSize(wxSize(dip, dip), 1.0, 32));
    REQUIRE(two.CreateWithDIPSize(wxSize(dip, dip), 2.0, 32));
    return wxBitmapBundle::FromBitmaps(one, two);
}

// XAML lays its elements out between two messages.
template <typename Predicate>
bool WaitToolbarCondition(Predicate predicate)
{
    for ( int i = 0; i < 200; ++i )
    {
        if ( predicate() )
            return true;
        wxYield();
        wxMilliSleep(5);
    }
    return predicate();
}

// What a click does, whether XAML tells of it at once or a message later.
bool ClickTool(wxToolBar& toolbar, int toolid, bool dropdownPart = false)
{
    const bool done = Access::InvokeTool(toolbar, toolid, dropdownPart);
    wxYield();
    wxYield();
    return done;
}

Access::PeerSnapshot GetPeer(const wxToolBar& toolbar, int toolid)
{
    Access::PeerSnapshot snapshot;
    REQUIRE(Access::GetToolPeerState(toolbar, toolid, &snapshot));
    return snapshot;
}

// Counts the clicks the tool bar tells the program of, and refuses one.
class CountingToolBar final : public wxToolBar
{
public:
    using wxToolBar::wxToolBar;

    bool OnLeftClick(int toolid, bool toggleDown) override
    {
        ++clicks;
        lastId = toolid;
        lastToggle = toggleDown;
        return toolid != vetoId;
    }

    int vetoId = wxID_NONE;
    int clicks = 0;
    int lastId = wxID_NONE;
    bool lastToggle = false;
};

} // anonymous namespace

TEST_CASE("wxWinUI ToolBar tools are the commands of its bar",
          "[winui-toolbar]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxToolBar toolbar;
    REQUIRE(toolbar.Create(parent, wxID_ANY, wxDefaultPosition,
                           wxSize(700, 48), wxTB_HORIZONTAL | wxTB_TEXT));

    const wxBitmapBundle bitmap = MakeToolbarBitmapBundle();
    REQUIRE(toolbar.AddTool(101, "&one", bitmap, "one tip"));
    REQUIRE(toolbar.AddCheckTool(102, "check", bitmap));
    REQUIRE(toolbar.AddSeparator());
    REQUIRE(toolbar.AddRadioTool(103, "first", bitmap));
    REQUIRE(toolbar.AddRadioTool(104, "second", bitmap));
    REQUIRE(toolbar.AddTool(105, "menu", bitmap, "", wxITEM_DROPDOWN));
    REQUIRE(toolbar.AddTool(106, "plain", wxBitmapBundle()));

    // Nothing is shown before Realize(), as in the other ports.
    CHECK(Access::GetPeerToolCount(toolbar) == 0);
    REQUIRE(toolbar.Realize());
    CHECK(Access::GetPeerToolCount(toolbar) == 7);

    const Access::PeerSnapshot one = GetPeer(toolbar, 101);
    CHECK(one.enabled);
    CHECK_FALSE(one.toggled);
    CHECK(one.hasIcon);
    CHECK(one.showsText);
    CHECK_FALSE(one.dropdown);
    CHECK(one.label == "one");
    CHECK(one.toolTip == "one tip");

    CHECK_FALSE(GetPeer(toolbar, 102).toggled);

    // The first tool of a group of radio tools is down.
    CHECK(toolbar.GetToolState(103));
    CHECK(GetPeer(toolbar, 103).toggled);
    CHECK_FALSE(GetPeer(toolbar, 104).toggled);

    CHECK(GetPeer(toolbar, 105).dropdown);
    CHECK_FALSE(GetPeer(toolbar, 106).hasIcon);

    // The labels follow the style.
    toolbar.SetWindowStyle(wxTB_HORIZONTAL);
    CHECK_FALSE(GetPeer(toolbar, 101).showsText);
    CHECK(GetPeer(toolbar, 101).label == "one");

    toolbar.SetWindowStyle(wxTB_HORIZONTAL | wxTB_TEXT | wxTB_NOICONS);
    CHECK(GetPeer(toolbar, 101).showsText);
    CHECK_FALSE(GetPeer(toolbar, 101).hasIcon);
}

TEST_CASE("wxWinUI ToolBar radio tool put down by the program stays down",
          "[winui-toolbar]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxToolBar toolbar(parent, wxID_ANY);
    const wxBitmapBundle bitmap = MakeToolbarBitmapBundle();
    toolbar.AddRadioTool(111, "a", bitmap);
    toolbar.AddRadioTool(112, "b", bitmap);
    toolbar.AddRadioTool(113, "c", bitmap);
    toolbar.AddSeparator();
    toolbar.AddRadioTool(114, "d", bitmap);
    toolbar.ToggleTool(112, true);
    REQUIRE(toolbar.Realize());

    CHECK_FALSE(GetPeer(toolbar, 111).toggled);
    CHECK(GetPeer(toolbar, 112).toggled);
    CHECK_FALSE(GetPeer(toolbar, 113).toggled);

    // Another group.
    CHECK(GetPeer(toolbar, 114).toggled);
}

TEST_CASE("wxWinUI ToolBar clicks checks radio groups menus and refusals",
          "[winui-toolbar]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    CountingToolBar toolbar(parent, wxID_ANY);
    const wxBitmapBundle bitmap = MakeToolbarBitmapBundle();
    toolbar.AddTool(201, "normal", bitmap);
    toolbar.AddCheckTool(202, "check", bitmap);
    toolbar.AddRadioTool(203, "first", bitmap);
    toolbar.AddRadioTool(204, "second", bitmap);
    toolbar.AddTool(205, "menu", bitmap, "", wxITEM_DROPDOWN);
    REQUIRE(toolbar.Realize());

    REQUIRE(ClickTool(toolbar, 201));
    CHECK(toolbar.clicks == 1);
    CHECK(toolbar.lastId == 201);
    CHECK_FALSE(toolbar.lastToggle);

    REQUIRE(ClickTool(toolbar, 202));
    CHECK(toolbar.clicks == 2);
    CHECK(toolbar.lastToggle);
    CHECK(toolbar.GetToolState(202));
    CHECK(GetPeer(toolbar, 202).toggled);

    REQUIRE(ClickTool(toolbar, 202));
    CHECK(toolbar.clicks == 3);
    CHECK_FALSE(toolbar.lastToggle);
    CHECK_FALSE(toolbar.GetToolState(202));

    // The other tool of the group comes up.
    REQUIRE(ClickTool(toolbar, 204));
    CHECK(toolbar.clicks == 4);
    CHECK(toolbar.GetToolState(204));
    CHECK_FALSE(toolbar.GetToolState(203));
    CHECK(GetPeer(toolbar, 204).toggled);
    CHECK_FALSE(GetPeer(toolbar, 203).toggled);

    // A radio tool which is down stays down.
    REQUIRE(ClickTool(toolbar, 204));
    CHECK(toolbar.GetToolState(204));
    CHECK(GetPeer(toolbar, 204).toggled);

    // The program refuses the change: nothing is checked.
    toolbar.vetoId = 202;
    const int before = toolbar.clicks;
    REQUIRE(ClickTool(toolbar, 202));
    CHECK(toolbar.clicks == before + 1);
    CHECK_FALSE(toolbar.GetToolState(202));
    CHECK_FALSE(GetPeer(toolbar, 202).toggled);
    toolbar.vetoId = wxID_NONE;

    // The chevron of a tool with a menu is not a click on the tool.
    int dropdowns = 0;
    toolbar.Bind(wxEVT_TOOL_DROPDOWN,
                 [&dropdowns](wxCommandEvent&) { ++dropdowns; }, 205);
    const int clicksBefore = toolbar.clicks;
    REQUIRE(ClickTool(toolbar, 205, true));
    CHECK(dropdowns == 1);
    CHECK(toolbar.clicks == clicksBefore);

    REQUIRE(ClickTool(toolbar, 205));
    CHECK(dropdowns == 1);
    CHECK(toolbar.clicks == clicksBefore + 1);

    // A disabled tool cannot be clicked.
    toolbar.EnableTool(201, false);
    CHECK_FALSE(ClickTool(toolbar, 201));
    CHECK(toolbar.clicks == clicksBefore + 1);
}

TEST_CASE("wxWinUI ToolBar shows what the program changes",
          "[winui-toolbar]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    CountingToolBar toolbar(parent, wxID_ANY, wxDefaultPosition,
                            wxDefaultSize, wxTB_HORIZONTAL | wxTB_TEXT);
    const wxBitmapBundle bitmap = MakeToolbarBitmapBundle();
    toolbar.AddTool(301, "normal", bitmap, "tip");
    toolbar.AddCheckTool(302, "check", bitmap);
    toolbar.AddTool(303, "bare", wxBitmapBundle());
    REQUIRE(toolbar.Realize());

    // Checking a tool from the program is not a click.
    toolbar.ToggleTool(302, true);
    CHECK(GetPeer(toolbar, 302).toggled);
    toolbar.ToggleTool(302, false);
    CHECK_FALSE(GetPeer(toolbar, 302).toggled);
    wxYield();
    CHECK(toolbar.clicks == 0);

    // A disabled tool has its picture fainter, or the one given for it.
    toolbar.EnableTool(301, false);
    Access::PeerSnapshot peer = GetPeer(toolbar, 301);
    CHECK_FALSE(peer.enabled);
    CHECK_FALSE(peer.usesDisabledBitmap);
    CHECK(peer.iconOpacity < 1.0);

    toolbar.SetToolDisabledBitmap(301, MakeToolbarBitmapBundle());
    peer = GetPeer(toolbar, 301);
    CHECK(peer.usesDisabledBitmap);
    CHECK(peer.iconOpacity == 1.0);

    toolbar.EnableTool(301, true);
    peer = GetPeer(toolbar, 301);
    CHECK(peer.enabled);
    CHECK_FALSE(peer.usesDisabledBitmap);

    // The whole bar, then back: each tool is as it was.
    toolbar.EnableTool(303, false);
    toolbar.Enable(false);
    CHECK_FALSE(GetPeer(toolbar, 301).enabled);
    CHECK_FALSE(GetPeer(toolbar, 302).enabled);
    toolbar.Enable(true);
    CHECK(GetPeer(toolbar, 301).enabled);
    CHECK(GetPeer(toolbar, 302).enabled);
    CHECK_FALSE(GetPeer(toolbar, 303).enabled);

    toolbar.SetToolShortHelp(302, "new tip");
    CHECK(GetPeer(toolbar, 302).toolTip == "new tip");
    toolbar.SetToolShortHelp(301, "");
    CHECK(GetPeer(toolbar, 301).toolTip.empty());

    wxToolBarToolBase * const tool = toolbar.FindById(301);
    REQUIRE(tool);
    tool->SetLabel("renamed");
    CHECK(GetPeer(toolbar, 301).label == "renamed");

    CHECK_FALSE(GetPeer(toolbar, 303).hasIcon);
    toolbar.SetToolNormalBitmap(303, bitmap);
    CHECK(GetPeer(toolbar, 303).hasIcon);
    toolbar.SetToolNormalBitmap(303, wxBitmapBundle());
    CHECK_FALSE(GetPeer(toolbar, 303).hasIcon);
}

TEST_CASE("wxWinUI ToolBar removes and takes back its tools",
          "[winui-toolbar]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    CountingToolBar toolbar(parent, wxID_ANY);
    const wxBitmapBundle bitmap = MakeToolbarBitmapBundle();
    toolbar.AddTool(401, "one", bitmap);
    toolbar.AddTool(402, "two", bitmap);
    toolbar.AddSeparator();
    toolbar.AddTool(403, "three", bitmap);
    REQUIRE(toolbar.Realize());
    CHECK(Access::GetPeerToolCount(toolbar) == 4);

    // Gone at once: no button is left to click a tool which is no more.
    REQUIRE(toolbar.DeleteTool(402));
    CHECK(Access::GetPeerToolCount(toolbar) == 3);
    CHECK_FALSE(ClickTool(toolbar, 402));
    CHECK(toolbar.clicks == 0);

    wxToolBarToolBase * const removed = toolbar.RemoveTool(401);
    REQUIRE(removed);
    CHECK(Access::GetPeerToolCount(toolbar) == 2);
    CHECK_FALSE(ClickTool(toolbar, 401));

    REQUIRE(toolbar.InsertTool(0, removed));
    REQUIRE(toolbar.Realize());
    CHECK(Access::GetPeerToolCount(toolbar) == 3);
    REQUIRE(ClickTool(toolbar, 401));
    CHECK(toolbar.clicks == 1);

    // A tool added once the bar is realized is shown without another
    // Realize(), when the code which adds it has finished.
    REQUIRE(toolbar.AddTool(404, "late", bitmap));
    REQUIRE(WaitToolbarCondition([&toolbar]()
    {
        return Access::GetPeerToolCount(toolbar) == 4;
    }));
    REQUIRE(ClickTool(toolbar, 404));
    CHECK(toolbar.lastId == 404);

    // Two tools with the same identifier: the one at the position goes.
    REQUIRE(toolbar.AddTool(403, "three again", bitmap));
    REQUIRE(toolbar.Realize());
    CHECK(Access::GetPeerToolCount(toolbar) == 5);
    REQUIRE(toolbar.DeleteToolByPos(toolbar.GetToolsCount() - 1));
    CHECK(Access::GetPeerToolCount(toolbar) == 4);
    CHECK(GetPeer(toolbar, 403).label == "three");

    toolbar.ClearTools();
    CHECK(toolbar.GetToolsCount() == 0);
    CHECK(Access::GetPeerToolCount(toolbar) == 0);
    CHECK(toolbar.FindToolForPosition(1, 1) == nullptr);
    CHECK(toolbar.Realize());
}

#if wxUSE_BUTTON

TEST_CASE("wxWinUI ToolBar control keeps its window at its place",
          "[winui-toolbar]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxToolBar toolbar(parent, wxID_ANY, wxDefaultPosition, wxSize(700, 48));
    const wxBitmapBundle bitmap = MakeToolbarBitmapBundle();
    wxButton * const button = new wxButton(&toolbar, wxID_ANY, "embedded");
    toolbar.AddTool(501, "before", bitmap);
    toolbar.AddControl(button);
    toolbar.AddTool(502, "after", bitmap);
    REQUIRE(toolbar.Realize());
    toolbar.SetSize(700, toolbar.GetBestSize().y);

    CHECK(GetPeer(toolbar, button->GetId()).control);

    // Between its two neighbours, and inside the bar.
    REQUIRE(WaitToolbarCondition([&]()
    {
        const wxRect before = GetPeer(toolbar, 501).bounds;
        const wxRect after = GetPeer(toolbar, 502).bounds;
        const wxRect rect = button->GetRect();
        return button->IsShown() && !before.IsEmpty() && !after.IsEmpty() &&
               rect.GetLeft() >= before.GetRight() &&
               rect.GetRight() <= after.GetLeft() &&
               wxRect(toolbar.GetClientSize()).Contains(rect);
    }));
    CHECK(toolbar.FindToolForPosition(GetPeer(toolbar, 502).bounds.x + 2,
                                      GetPeer(toolbar, 502).bounds.y + 2)
            == toolbar.FindById(502));

    // No longer a tool: its window has no place in the bar.
    wxToolBarToolBase * const removed = toolbar.RemoveTool(button->GetId());
    REQUIRE(removed);
    CHECK_FALSE(button->IsShown());
    delete removed;
}

#endif // wxUSE_BUTTON

TEST_CASE("wxWinUI ToolBar puts the tools which do not fit in its menu",
          "[winui-toolbar]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    CountingToolBar toolbar(parent, wxID_ANY, wxDefaultPosition,
                            wxSize(900, 48), wxTB_HORIZONTAL | wxTB_TEXT);
    const wxBitmapBundle bitmap = MakeToolbarBitmapBundle();
    for ( int n = 0; n < 10; ++n )
        toolbar.AddTool(600 + n, wxString::Format("tool %d", n), bitmap);
    REQUIRE(toolbar.Realize());

    const wxSize best = toolbar.GetBestSize();
    CHECK(best.x > toolbar.FromDIP(10 * 48));
    CHECK(best.y >= toolbar.FromDIP(48));

    // Wide enough for all of them.
    toolbar.SetSize(best);
    REQUIRE(WaitToolbarCondition([&toolbar]()
    {
        return !GetPeer(toolbar, 600).bounds.IsEmpty();
    }));
    CHECK(WaitToolbarCondition([&toolbar]()
    {
        return !GetPeer(toolbar, 609).bounds.IsEmpty() &&
               Access::GetOverflowedToolCount(toolbar) == 0;
    }));
    CAPTURE(best, GetPeer(toolbar, 600).bounds, GetPeer(toolbar, 608).bounds,
            GetPeer(toolbar, 609).bounds);
    CHECK(Access::GetOverflowedToolCount(toolbar) == 0);

    // Too narrow: the last ones leave the bar, and still work.
    toolbar.SetSize(best.x / 3, best.y);
    REQUIRE(WaitToolbarCondition([&toolbar]()
    {
        return Access::GetOverflowedToolCount(toolbar) > 0;
    }));
    CHECK(GetPeer(toolbar, 609).overflowed);
    CHECK_FALSE(GetPeer(toolbar, 600).overflowed);
    CHECK(toolbar.FindToolForPosition(GetPeer(toolbar, 600).bounds.x + 2,
                                      best.y / 2) == toolbar.FindById(600));
    REQUIRE(ClickTool(toolbar, 609));
    CHECK(toolbar.lastId == 609);

    toolbar.SetSize(best);
    REQUIRE(WaitToolbarCondition([&toolbar]()
    {
        return Access::GetOverflowedToolCount(toolbar) == 0;
    }));
}

TEST_CASE("wxWinUI ToolBar stretchable space and vertical bar",
          "[winui-toolbar]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const wxBitmapBundle bitmap = MakeToolbarBitmapBundle();

    SECTION("the tools after the space are at the end of the bar")
    {
        wxToolBar toolbar(parent, wxID_ANY, wxDefaultPosition,
                          wxSize(600, 48));
        toolbar.AddTool(701, "start", bitmap);
        toolbar.AddStretchableSpace();
        toolbar.AddTool(702, "end", bitmap);
        REQUIRE(toolbar.Realize());
        toolbar.SetSize(600, toolbar.GetBestSize().y);

        const int width = toolbar.GetClientSize().x;
        REQUIRE(WaitToolbarCondition([&]()
        {
            const wxRect start = GetPeer(toolbar, 701).bounds;
            const wxRect end = GetPeer(toolbar, 702).bounds;
            return !start.IsEmpty() && !end.IsEmpty() &&
                   start.GetLeft() < toolbar.FromDIP(8) &&
                   end.GetRight() > width - toolbar.FromDIP(8);
        }));
    }

    SECTION("a vertical bar is a column")
    {
        wxToolBar toolbar(parent, wxID_ANY, wxDefaultPosition,
                          wxDefaultSize, wxTB_VERTICAL);
        toolbar.AddTool(711, "up", bitmap);
        toolbar.AddSeparator();
        toolbar.AddTool(712, "down", bitmap);
        REQUIRE(toolbar.Realize());
        CHECK(toolbar.IsVertical());

        const wxSize best = toolbar.GetBestSize();
        CHECK(best.y > best.x);
        toolbar.SetSize(best);

        REQUIRE(WaitToolbarCondition([&]()
        {
            const wxRect up = GetPeer(toolbar, 711).bounds;
            const wxRect down = GetPeer(toolbar, 712).bounds;
            return !up.IsEmpty() && !down.IsEmpty() &&
                   down.GetTop() >= up.GetBottom();
        }));
    }

    SECTION("larger pictures make a higher bar")
    {
        wxToolBar little(parent, wxID_ANY);
        little.AddTool(721, "little", bitmap);
        REQUIRE(little.Realize());

        wxToolBar large(parent, wxID_ANY);
        large.AddTool(722, "large", MakeToolbarBitmapBundle(48));
        REQUIRE(large.Realize());

        CHECK(little.GetBestSize().y == little.FromDIP(48));
        CHECK(little.GetBestSize().x < little.FromDIP(2 * 48 + 16));
        CHECK(large.GetBestSize().y > little.GetBestSize().y);
        CHECK(large.GetBestSize().x > little.GetBestSize().x);
    }
}

TEST_CASE("wxWinUI ToolBar may be destroyed by the click it reports",
          "[winui-toolbar]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const size_t before = Access::GetLiveCallbackStateCount();

    wxToolBar * const toolbar = new wxToolBar(parent, wxID_ANY);
    toolbar->AddTool(801, "close", MakeToolbarBitmapBundle());
    toolbar->AddCheckTool(802, "check", MakeToolbarBitmapBundle());
    REQUIRE(toolbar->Realize());
    CHECK(Access::GetLiveCallbackStateCount() == before + 1);

    bool destroyed = false;
    toolbar->Bind(wxEVT_TOOL, [toolbar, &destroyed](wxCommandEvent&)
    {
        destroyed = true;
        toolbar->Destroy();
    });

    CHECK(Access::InvokeTool(*toolbar, 802));
    wxYield();
    wxYield();
    CHECK(destroyed);
    CHECK(Access::GetLiveCallbackStateCount() == before);
}

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_TOOLBAR
