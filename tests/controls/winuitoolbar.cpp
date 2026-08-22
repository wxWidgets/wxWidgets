///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuitoolbar.cpp
// Purpose:     deterministic WinUI toolbar model/lifetime contract tests
// Author:      wxWidgets development team
// Created:     2026-07-26
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_TOOLBAR

#include "wx/app.h"
#include "wx/bitmap.h"
#include "wx/log.h"
#include "wx/toolbar.h"
#include "wx/weakref.h"

#if wxUSE_BUTTON
    #include "wx/button.h"
#endif
#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif

#include <memory>
#include <new>

namespace
{

wxBitmapBundle MakeToolbarBitmapBundle()
{
    wxBitmap one;
    wxBitmap two;
    REQUIRE(one.CreateWithDIPSize(wxSize(16, 16), 1.0, 32));
    REQUIRE(two.CreateWithDIPSize(wxSize(16, 16), 2.0, 32));
    return wxBitmapBundle::FromBitmaps(one, two);
}

wxBitmapBundle MakeDisabledToolbarBitmapBundle()
{
    wxBitmap one;
    wxBitmap two;
    REQUIRE(one.CreateWithDIPSize(wxSize(18, 18), 1.0, 32));
    REQUIRE(two.CreateWithDIPSize(wxSize(18, 18), 2.0, 32));
    return wxBitmapBundle::FromBitmaps(one, two);
}

void DrainToolbarCallbacks()
{
    wxYield();
    wxYield();
}

template <typename Predicate>
bool WaitToolbarCondition(Predicate predicate)
{
    for ( int i = 0; i < 32; ++i )
    {
        if ( predicate() )
            return true;
        wxYield();
    }
    return predicate();
}

class VetoToolBar final : public wxToolBar
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

struct NestedRebuildProbe
{
    bool invoked = false;
    bool nestedResult = true;
};

void RequestNestedRebuild(wxToolBar *toolbar, void *context)
{
    NestedRebuildProbe * const probe =
        static_cast<NestedRebuildProbe *>(context);
    probe->invoked = true;
    probe->nestedResult =
        toolbar->WinUIRefreshForScaleForTesting(1.5);
}

struct NestedBitmapSizeProbe
{
    bool invoked = false;
    wxSize requestedSize;
};

void RequestNestedBitmapSize(wxToolBar *toolbar, void *context)
{
    NestedBitmapSizeProbe * const probe =
        static_cast<NestedBitmapSizeProbe *>(context);
    probe->invoked = true;
    toolbar->SetToolBitmapSize(probe->requestedSize);
}

struct DestroyDuringLoadedProbe
{
    bool invoked = false;
};

void DestroyToolbarDuringLoaded(wxToolBar *toolbar, void *context)
{
    DestroyDuringLoadedProbe * const probe =
        static_cast<DestroyDuringLoadedProbe *>(context);
    probe->invoked = true;
    // Last operation: Destroy() synchronously invalidates the toolbar's
    // callback state on this port.
    toolbar->Destroy();
}

struct DestroyControlDuringLoadedProbe
{
    wxWindow *control = nullptr;
    bool invoked = false;
};

void DestroyControlDuringLoaded(wxToolBar *, void *context)
{
    DestroyControlDuringLoadedProbe * const probe =
        static_cast<DestroyControlDuringLoadedProbe *>(context);
    probe->invoked = true;
    wxWindow* const control = probe->control;
    probe->control = nullptr;

    // Deliberately synchronous: this is the exact lifetime boundary which used
    // to leave wxToolBarToolBase::m_control dangling until its destructor.
    delete control;
}

struct LoadedMutationProbe
{
    int toolId = wxID_NONE;
    bool invoked = false;
    bool sawCandidate = false;
};

struct LoadedLabelProbe
{
    wxToolBarToolBase *tool = nullptr;
    wxString label;
    bool invoked = false;
};

struct CrossToolbarInsertProbe
{
    wxToolBar *other = nullptr;
    wxToolBarToolBase *tool = nullptr;
    wxToolBarToolBase *nestedResult = nullptr;
    bool invoked = false;
};

void InsertCandidateIntoOtherToolbar(wxToolBar *, void *context)
{
    CrossToolbarInsertProbe * const probe =
        static_cast<CrossToolbarInsertProbe *>(context);
    probe->invoked = true;
    probe->nestedResult = probe->other->AddTool(probe->tool);
}

void SetToolLabelDuringLoaded(wxToolBar *, void *context)
{
    LoadedLabelProbe * const probe =
        static_cast<LoadedLabelProbe *>(context);
    probe->invoked = true;
    probe->tool->SetLabel(probe->label);
}

void MutateCandidateDuringLoaded(wxToolBar *toolbar, void *context)
{
    LoadedMutationProbe * const probe =
        static_cast<LoadedMutationProbe *>(context);
    probe->invoked = true;
    toolbar->EnableTool(probe->toolId, false);
    toolbar->SetToolShortHelp(probe->toolId, "loaded-final");

    wxWinUIToolPeerSnapshot state;
    probe->sawCandidate =
        toolbar->WinUIGetToolPeerStateForTesting(
            probe->toolId, &state) &&
        !state.enabled &&
        state.toolTip == "loaded-final" &&
        state.peerToolTip == "loaded-final" &&
        state.helpText == "loaded-final";
}

struct ReentrantShortHelpProbe
{
    int toolId = wxID_NONE;
    bool invoked = false;
};

void ReplaceShortHelpFromSetter(wxToolBar *toolbar, void *context)
{
    ReentrantShortHelpProbe * const probe =
        static_cast<ReentrantShortHelpProbe *>(context);
    probe->invoked = true;
    toolbar->SetToolShortHelp(probe->toolId, "nested-final");
}

struct DestroyDuringShortHelpProbe
{
    bool invoked = false;
};

void DestroyToolbarDuringShortHelp(wxToolBar *toolbar, void *context)
{
    DestroyDuringShortHelpProbe * const probe =
        static_cast<DestroyDuringShortHelpProbe *>(context);
    probe->invoked = true;
    toolbar->Destroy();
}

struct OtherShortHelpProbe
{
    int toolId = wxID_NONE;
    bool invoked = false;
};

void QueueOtherShortHelpFromSetter(wxToolBar *toolbar, void *context)
{
    OtherShortHelpProbe * const probe =
        static_cast<OtherShortHelpProbe *>(context);
    probe->invoked = true;
    toolbar->SetToolShortHelp(probe->toolId, "healthy-final");
}

struct ShortHelpStormProbe
{
    int toolId = wxID_NONE;
    unsigned remaining = 0;
    unsigned invocations = 0;
};

void ContinueShortHelpStorm(wxToolBar *toolbar, void *context)
{
    ShortHelpStormProbe * const probe =
        static_cast<ShortHelpStormProbe *>(context);
    ++probe->invocations;
    if ( probe->remaining )
        --probe->remaining;

    if ( probe->remaining )
    {
        toolbar->WinUISetNextShortHelpSetterHookForTesting(
            &ContinueShortHelpStorm, probe);
    }
    toolbar->SetToolShortHelp(
        probe->toolId,
        wxString::Format("storm-%u", probe->invocations));
}

struct ReentrantEnableProbe
{
    int toolId = wxID_NONE;
    bool invoked = false;
};

void ReenableFromIsEnabledBoundary(wxToolBar *toolbar, void *context)
{
    ReentrantEnableProbe * const probe =
        static_cast<ReentrantEnableProbe *>(context);
    probe->invoked = true;
    toolbar->EnableTool(probe->toolId, true);
}

struct OverflowMutationProbe
{
    wxCoord nestedExtent = 0;
    bool invoked = false;
    bool nestedResult = true;
};

void ReenterOverflowMutation(wxToolBar *toolbar, void *context)
{
    OverflowMutationProbe * const probe =
        static_cast<OverflowMutationProbe *>(context);
    probe->invoked = true;
    probe->nestedResult =
        toolbar->WinUIApplyOverflowExtentForTesting(
            probe->nestedExtent);
}

struct OverflowEnableMutationProbe
{
    int toolId = wxID_NONE;
    bool invoked = false;
};

void DisableToolDuringOverflow(wxToolBar *toolbar, void *context)
{
    OverflowEnableMutationProbe * const probe =
        static_cast<OverflowEnableMutationProbe *>(context);
    probe->invoked = true;
    toolbar->EnableTool(probe->toolId, false);
}

} // anonymous namespace

TEST_CASE("wxWinUI ToolBar deletion is atomic and leaves no ghost peer",
          "[winui-toolbar][transaction][default-create]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxToolBar toolbar;
    REQUIRE(toolbar.Create(
        parent, wxID_ANY, wxDefaultPosition, wxSize(360, 72)));

    const wxBitmapBundle bitmap = MakeToolbarBitmapBundle();
    REQUIRE(toolbar.AddTool(101, "one", bitmap, "one tip"));
    REQUIRE(toolbar.AddCheckTool(102, "check", bitmap));
    REQUIRE(toolbar.AddSeparator());
    REQUIRE(toolbar.AddTool(103, "three", bitmap));
    REQUIRE(toolbar.Realize());
    CHECK(toolbar.WinUIGetPeerToolCountForTesting() == 4);

    REQUIRE(toolbar.AddTool(104, "late", bitmap));
    CHECK(toolbar.WinUIGetPeerToolCountForTesting() == 5);
    REQUIRE(toolbar.DeleteTool(104));
    CHECK(toolbar.WinUIGetPeerToolCountForTesting() == 4);
    toolbar.WinUIFailNextRebuildForTesting();
    CHECK(toolbar.AddTool(105, "rejected", bitmap) == nullptr);
    CHECK(toolbar.FindById(105) == nullptr);
    CHECK(toolbar.WinUIGetPeerToolCountForTesting() == 4);

    // A failed candidate must leave both the common model and the old visual
    // generation intact.
    toolbar.WinUIFailNextRebuildForTesting();
    CHECK_FALSE(toolbar.DeleteTool(102));
    CHECK(toolbar.FindById(102) != nullptr);
    CHECK(toolbar.WinUIGetPeerToolCountForTesting() == 4);
    CHECK(toolbar.WinUIInvokeToolForTesting(102));
    CHECK(toolbar.GetToolState(102));

    wxToolBarToolBase * const removed = toolbar.RemoveTool(101);
    REQUIRE(removed);
    CHECK(toolbar.FindById(101) == nullptr);
    CHECK(toolbar.WinUIGetPeerToolCountForTesting() == 3);
    CHECK_FALSE(toolbar.WinUIInvokeToolForTesting(101));
    delete removed;

    REQUIRE(toolbar.DeleteTool(102));
    CHECK(toolbar.WinUIGetPeerToolCountForTesting() == 2);
    CHECK_FALSE(toolbar.WinUIInvokeToolForTesting(102));

    int ghostClicks = 0;
    toolbar.Bind(
        wxEVT_TOOL,
        [&ghostClicks](wxCommandEvent&)
        {
            ++ghostClicks;
        },
        103);
    REQUIRE(toolbar.WinUIQueueToolClickForTesting(103));
    REQUIRE(toolbar.DeleteTool(103));
    DrainToolbarCallbacks();
    CHECK(ghostClicks == 0);
    CHECK(toolbar.WinUIGetPeerToolCountForTesting() == 1);

    toolbar.WinUIFailNextRebuildForTesting();
    toolbar.ClearTools();
    CHECK(toolbar.GetToolsCount() == 1);
    CHECK(toolbar.WinUIGetPeerToolCountForTesting() == 1);

    toolbar.ClearTools();
    CHECK(toolbar.GetToolsCount() == 0);
    CHECK(toolbar.WinUIGetPeerToolCountForTesting() == 0);
    CHECK(toolbar.FindToolForPosition(1, 1) == nullptr);
}

TEST_CASE("wxWinUI ToolBar check radio dropdown and veto are exact",
          "[winui-toolbar][events][toggle][dropdown]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const wxBitmapBundle normal = MakeToolbarBitmapBundle();
    const wxBitmapBundle disabled = MakeDisabledToolbarBitmapBundle();
    VetoToolBar toolbar(
        parent, wxID_ANY, wxDefaultPosition, wxSize(420, 72));

    REQUIRE(toolbar.AddTool(
        201, "check", normal, disabled, wxITEM_CHECK));
    REQUIRE(toolbar.AddRadioTool(202, "first", normal));
    REQUIRE(toolbar.AddRadioTool(203, "second", normal));
    REQUIRE(toolbar.AddTool(
        204, "drop", normal, wxBitmapBundle(), wxITEM_DROPDOWN));
    REQUIRE(toolbar.AddTool(205, "promoted", normal));
    REQUIRE(toolbar.Realize());

    REQUIRE(toolbar.WinUIInvokeToolForTesting(201));
    CHECK(toolbar.GetToolState(201));
    CHECK(toolbar.lastId == 201);
    CHECK(toolbar.lastToggle);

    toolbar.vetoId = 201;
    REQUIRE(toolbar.WinUIInvokeToolForTesting(201));
    CHECK(toolbar.GetToolState(201));
    CHECK_FALSE(toolbar.lastToggle);

    toolbar.vetoId = wxID_NONE;
    toolbar.ToggleTool(202, true);
    REQUIRE(toolbar.WinUIInvokeToolForTesting(203));
    CHECK_FALSE(toolbar.GetToolState(202));
    CHECK(toolbar.GetToolState(203));

    // Vetoing another radio restores the entire old group, including the
    // previously selected radio.
    toolbar.vetoId = 202;
    REQUIRE(toolbar.WinUIInvokeToolForTesting(202));
    CHECK_FALSE(toolbar.GetToolState(202));
    CHECK(toolbar.GetToolState(203));

    toolbar.EnableTool(201, false);
    wxWinUIToolPeerSnapshot state;
    REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(201, &state));
    CHECK_FALSE(state.enabled);
    CHECK(state.hasIcon);
    CHECK(state.usesDisabledBitmap);
    CHECK_FALSE(toolbar.WinUIInvokeToolForTesting(201));

    toolbar.SetToggle(205, true);
    REQUIRE(toolbar.WinUIInvokeToolForTesting(205));
    CHECK(toolbar.GetToolState(205));

    toolbar.Enable(false);
    CHECK_FALSE(toolbar.WinUIInvokeToolForTesting(204));
    toolbar.Enable(true);

    int dropdownEvents = 0;
    toolbar.Bind(
        wxEVT_TOOL_DROPDOWN,
        [&dropdownEvents](wxCommandEvent& event)
        {
            ++dropdownEvents;
            CHECK(event.GetId() == 204);
        },
        204);
    toolbar.vetoId = wxID_NONE;
    const int clicksBeforeMain = toolbar.clicks;
    REQUIRE(toolbar.WinUIInvokeToolForTesting(204));
    CHECK(toolbar.clicks == clicksBeforeMain + 1);
    CHECK(dropdownEvents == 0);
    REQUIRE(toolbar.WinUIInvokeToolForTesting(204, true));
    CHECK(dropdownEvents == 1);
    CHECK(toolbar.clicks == clicksBeforeMain + 1);
}

TEST_CASE("wxWinUI ToolBar detached tools reinsert and kind rollback is exact",
          "[winui-toolbar][transaction][reinsert][toggle][bitmap]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    const wxBitmapBundle bitmap = MakeToolbarBitmapBundle();

    wxToolBar toolbar(
        parent, wxID_ANY, wxDefaultPosition, wxSize(420, 72));
    REQUIRE(toolbar.AddTool(271, "normal", bitmap));
    REQUIRE(toolbar.AddCheckTool(272, "check", bitmap));
    REQUIRE(toolbar.AddRadioTool(273, "radio", bitmap));
    REQUIRE(toolbar.AddTool(
        274, "dropdown", bitmap, wxBitmapBundle(),
        wxITEM_DROPDOWN));
    REQUIRE(toolbar.Realize());

    for ( const int id : { 271, 272, 274 } )
    {
        wxToolBarToolBase * const detached = toolbar.RemoveTool(id);
        REQUIRE(detached);
        CHECK(detached->GetToolBar() == nullptr);
        REQUIRE(toolbar.AddTool(detached) == detached);
        wxWinUIToolPeerSnapshot state;
        REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(id, &state));
        CHECK(state.hasIcon);
        CHECK_FALSE(state.usesDisabledBitmap);
    }

    wxToolBarToolBase * const radio = toolbar.FindById(273);
    wxToolBarToolBase * const dropdown = toolbar.FindById(274);
    REQUIRE(radio);
    REQUIRE(dropdown);
    REQUIRE(radio->GetKind() == wxITEM_RADIO);
    REQUIRE(dropdown->GetKind() == wxITEM_DROPDOWN);

    toolbar.WinUIFailNextRebuildForTesting();
    toolbar.SetToggle(273, false);
    CHECK(radio->GetKind() == wxITEM_RADIO);

    toolbar.WinUIFailNextRebuildForTesting();
    toolbar.SetToggle(274, true);
    CHECK(dropdown->GetKind() == wxITEM_DROPDOWN);

    int dropdownEvents = 0;
    toolbar.Bind(
        wxEVT_TOOL_DROPDOWN,
        [&dropdownEvents](wxCommandEvent&)
        {
            ++dropdownEvents;
        },
        274);
    REQUIRE(toolbar.WinUIInvokeToolForTesting(274, true));
    CHECK(dropdownEvents == 1);
}

#if wxUSE_BUTTON

TEST_CASE("wxWinUI ToolBar styles bounds stretch controls and DPI are real",
          "[winui-toolbar][layout][styles][dpi][control]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const wxBitmapBundle bitmap = MakeToolbarBitmapBundle();
    wxToolBar toolbar(
        parent, wxID_ANY, wxDefaultPosition, wxSize(560, 82),
        wxTB_HORIZONTAL | wxTB_TEXT | wxTB_HORZ_LAYOUT);
    REQUIRE(toolbar.AddTool(301, "&Open", bitmap, "open tip"));
    REQUIRE(toolbar.AddStretchableSpace());
    REQUIRE(toolbar.AddTool(302, "Save", bitmap));
    wxButton * const embedded =
        new wxButton(&toolbar, 303, "Embedded",
                     wxDefaultPosition, wxSize(92, 30));
    REQUIRE(toolbar.AddControl(embedded, "control"));
    REQUIRE(toolbar.Realize());
    DrainToolbarCallbacks();

    wxWinUIToolPeerSnapshot first;
    wxWinUIToolPeerSnapshot stretch;
    wxWinUIToolPeerSnapshot second;
    wxWinUIToolPeerSnapshot control;
    REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(301, &first));
    REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(
        wxID_SEPARATOR, &stretch));
    REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(302, &second));
    REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(303, &control));

    CHECK(first.hasIcon);
    CHECK(first.showsText);
    CHECK(first.horizontalText);
    CHECK(first.toolTip == "open tip");
    CHECK(first.bounds.width > 0);
    CHECK(first.bounds.height > 0);
    CHECK(stretch.stretchable);
    CHECK(stretch.bounds.width > 0);
    CHECK(second.bounds.x > first.bounds.GetRight());
    CHECK(control.control);
    CHECK(control.bounds.width > 0);
    CHECK(control.bounds.height > 0);

    toolbar.SetToolShortHelp(303, "embedded tip");
    REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(303, &control));
    CHECK(control.toolTip == "embedded tip");
#if wxUSE_TOOLTIPS
    CHECK(embedded->GetToolTipText() == "embedded tip");
#endif

    wxToolBarToolBase * const hit = toolbar.FindToolForPosition(
        first.bounds.x + first.bounds.width / 2,
        first.bounds.y + first.bounds.height / 2);
    REQUIRE(hit);
    CHECK(hit->GetId() == 301);
    CHECK(embedded->GetPosition().x >= control.bounds.x - 1);
    CHECK(embedded->GetPosition().y >= control.bounds.y - 1);

    toolbar.SetToolShortHelp(301, "updated tip");
    REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(301, &first));
    CHECK(first.toolTip == "updated tip");
    toolbar.SetToolShortHelp(301, wxString());
    REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(301, &first));
    CHECK(first.toolTip.empty());

    REQUIRE(toolbar.WinUIRefreshForScaleForTesting(2.0));
    REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(301, &first));
    CHECK(first.selectedPixelSize == wxSize(32, 32));

    wxToolBar noIcons(
        parent, wxID_ANY, wxDefaultPosition, wxSize(180, 70),
        wxTB_HORIZONTAL | wxTB_TEXT | wxTB_NOICONS);
    REQUIRE(noIcons.AddTool(311, "Text only", bitmap));
    REQUIRE(noIcons.Realize());
    REQUIRE(noIcons.WinUIGetToolPeerStateForTesting(311, &first));
    CHECK_FALSE(first.hasIcon);
    CHECK(first.showsText);

    wxToolBar noTips(
        parent, wxID_ANY, wxDefaultPosition, wxSize(180, 60),
        wxTB_HORIZONTAL | wxTB_NO_TOOLTIPS);
    REQUIRE(noTips.AddTool(312, "No tip", bitmap, "suppressed"));
    noTips.SetToolLongHelp(312, "accessible long help");
    REQUIRE(noTips.Realize());
    REQUIRE(noTips.WinUIGetToolPeerStateForTesting(312, &first));
    CHECK(first.toolTip.empty());
    CHECK(first.peerToolTip.empty());
    CHECK(first.helpText == "accessible long help");
    noTips.SetToolLongHelp(312, wxString());
    REQUIRE(noTips.WinUIGetToolPeerStateForTesting(312, &first));
    CHECK(first.helpText == "suppressed");

    wxToolBar bitmapSized(
        parent, wxID_ANY, wxDefaultPosition, wxSize(260, 72));
    bitmapSized.SetToolBitmapSize(wxSize(24, 24));
    REQUIRE(bitmapSized.AddTool(313, "Sized", bitmap));
    REQUIRE(bitmapSized.Realize());
    REQUIRE(bitmapSized.WinUIGetToolPeerStateForTesting(313, &first));
    CHECK(first.selectedPixelSize ==
          bitmapSized.FromDIP(wxSize(24, 24)));
    bitmapSized.SetToolBitmapSize(wxSize(20, 20));
    REQUIRE(bitmapSized.WinUIGetToolPeerStateForTesting(313, &first));
    CHECK(first.selectedPixelSize ==
          bitmapSized.FromDIP(wxSize(20, 20)));

    // A rejected native-peer publication restores both the effective size
    // and wxToolBarBase's retained DIP preference. Re-realizing exercises
    // AdjustToolBitmapSize(): the rejected 28-DIP request must not reappear.
    bitmapSized.WinUIFailNextRebuildForTesting();
    bitmapSized.SetToolBitmapSize(wxSize(28, 28));
    CHECK(bitmapSized.GetToolBitmapSize() == wxSize(20, 20));
    REQUIRE(bitmapSized.Realize());
    CHECK(bitmapSized.GetToolBitmapSize() == wxSize(20, 20));
    REQUIRE(bitmapSized.WinUIGetToolPeerStateForTesting(313, &first));
    CHECK(first.selectedPixelSize ==
          bitmapSized.FromDIP(wxSize(20, 20)));

    // A rejected nested request restores the outer transaction's revision,
    // allowing the outer request to restore its own parent state when the
    // host then rejects A as well.
    NestedBitmapSizeProbe nestedBitmapSize;
    nestedBitmapSize.requestedSize = wxSize(30, 30);
    bitmapSized.WinUISetNextRebuildLoadedHookForTesting(
        &RequestNestedBitmapSize, &nestedBitmapSize);
    bitmapSized.WinUIFailNextRebuildForTesting();
    bitmapSized.SetToolBitmapSize(wxSize(24, 24));
    CHECK(nestedBitmapSize.invoked);
    CHECK(bitmapSized.GetToolBitmapSize() == wxSize(20, 20));
    REQUIRE(bitmapSized.Realize());
    CHECK(bitmapSized.GetToolBitmapSize() == wxSize(20, 20));
    REQUIRE(bitmapSized.WinUIGetToolPeerStateForTesting(313, &first));
    CHECK(first.selectedPixelSize ==
          bitmapSized.FromDIP(wxSize(20, 20)));

    wxToolBar configuredLayout(
        parent, wxID_ANY, wxDefaultPosition, wxSize(320, 72));
    configuredLayout.SetMargins(9, 7);
    configuredLayout.SetToolPacking(12);
    configuredLayout.SetToolSeparation(28);
    REQUIRE(configuredLayout.AddTool(314, "First", bitmap));
    REQUIRE(configuredLayout.AddSeparator());
    REQUIRE(configuredLayout.AddTool(315, "Second", bitmap));
    REQUIRE(configuredLayout.Realize());
    CHECK(configuredLayout.GetToolMargins() == wxSize(9, 7));
    CHECK(configuredLayout.GetToolPacking() == 12);
    CHECK(configuredLayout.GetToolSeparation() == 28);
    REQUIRE(configuredLayout.WinUIGetToolPeerStateForTesting(
        wxID_SEPARATOR, &stretch));
    CHECK(stretch.bounds.width >= 27);

    configuredLayout.SetMargins(15, 11);
    configuredLayout.SetToolPacking(18);
    configuredLayout.SetToolSeparation(34);
    CHECK(configuredLayout.GetToolMargins() == wxSize(15, 11));
    CHECK(configuredLayout.GetToolPacking() == 18);
    CHECK(configuredLayout.GetToolSeparation() == 34);

    wxToolBar vertical(
        parent, wxID_ANY, wxDefaultPosition, wxSize(84, 220),
        wxTB_VERTICAL | wxTB_TEXT);
    REQUIRE(vertical.AddTool(321, "upper", bitmap));
    REQUIRE(vertical.AddSeparator());
    REQUIRE(vertical.AddTool(322, "lower", bitmap));
    REQUIRE(vertical.Realize());
    DrainToolbarCallbacks();
    REQUIRE(vertical.WinUIGetToolPeerStateForTesting(321, &first));
    REQUIRE(vertical.WinUIGetToolPeerStateForTesting(
        wxID_SEPARATOR, &stretch));
    REQUIRE(vertical.WinUIGetToolPeerStateForTesting(322, &second));
    CHECK(second.bounds.y > first.bounds.GetBottom());
    CHECK(stretch.bounds.width > stretch.bounds.height);

    wxWeakRef<wxWindow> embeddedLifetime(embedded);
    wxToolBarToolBase * const controlTool = toolbar.RemoveTool(303);
    REQUIRE(controlTool);
    CHECK_FALSE(embedded->IsShown());
    CHECK_FALSE(toolbar.WinUIGetToolPeerStateForTesting(303, &control));
    REQUIRE(toolbar.AddTool(controlTool) == controlTool);
    DrainToolbarCallbacks();
    CHECK(embedded->IsShown());
    REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(303, &control));
    wxToolBarToolBase * const removedAgain = toolbar.RemoveTool(303);
    REQUIRE(removedAgain == controlTool);
    delete removedAgain;
    DrainToolbarCallbacks();
    CHECK(embeddedLifetime.get() == nullptr);
}

TEST_CASE("wxWinUI ToolBar overflow reparents one peer without losing state",
          "[winui-toolbar][overflow][events][uia][control][identity]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const wxBitmapBundle normal = MakeToolbarBitmapBundle();
    const wxBitmapBundle disabled = MakeDisabledToolbarBitmapBundle();
    wxToolBar toolbar(
        parent, wxID_ANY, wxDefaultPosition, wxSize(560, 72));
    wxButton * const embedded =
        new wxButton(&toolbar, 601, "Pinned control",
                     wxDefaultPosition, wxSize(92, 30));
    REQUIRE(toolbar.AddControl(embedded, "control"));
    REQUIRE(toolbar.AddTool(610, "Normal", normal));
    REQUIRE(toolbar.AddTool(
        611, "Check", normal, disabled, wxITEM_CHECK));
    REQUIRE(toolbar.AddRadioTool(612, "Radio one", normal));
    REQUIRE(toolbar.AddRadioTool(613, "Radio two", normal));
    REQUIRE(toolbar.AddTool(
        614, "Dropdown", normal, wxBitmapBundle(), wxITEM_DROPDOWN));
    REQUIRE(toolbar.AddStretchableSpace());
    REQUIRE(toolbar.Realize());

    // Let the shared island attach and settle its real roomy geometry before
    // the deterministic overflow extent below is applied. Otherwise the first
    // Loaded/SizeChanged turn can legitimately overwrite that synthetic
    // extent with the toolbar's actual width.
    REQUIRE(WaitToolbarCondition(
        [&]()
        {
            return toolbar.WinUIIsRootLoadedForTesting();
        }));
    DrainToolbarCallbacks();

    const size_t peerCount =
        toolbar.WinUIGetPeerToolCountForTesting();
    REQUIRE(peerCount == toolbar.GetToolsCount());
    const wxCoord roomyExtent = toolbar.FromDIP(1000);
    const wxCoord narrowExtent = toolbar.FromDIP(180);
    REQUIRE(toolbar.WinUIApplyOverflowExtentForTesting(roomyExtent));
    CHECK_FALSE(toolbar.WinUIIsOverflowChevronVisibleForTesting());
    CHECK(toolbar.WinUIGetOverflowedToolCountForTesting() == 0);

    wxWinUIToolPeerSnapshot checkBefore;
    wxWinUIToolPeerSnapshot firstRadioBefore;
    wxWinUIToolPeerSnapshot dropdownBefore;
    REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(
        611, &checkBefore));
    REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(
        612, &firstRadioBefore));
    REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(
        614, &dropdownBefore));

    int dropdownEvents = 0;
    toolbar.Bind(
        wxEVT_TOOL_DROPDOWN,
        [&](wxCommandEvent& event)
        {
            ++dropdownEvents;
            CHECK(event.GetId() == 614);
        },
        614);
    REQUIRE(toolbar.WinUIInvokeToolForTesting(614, true));
    CHECK(dropdownEvents == 1);

    toolbar.SetSize(wxSize(narrowExtent, toolbar.GetSize().y));
    DrainToolbarCallbacks();
    REQUIRE(toolbar.WinUIApplyOverflowExtentForTesting(narrowExtent));
    CHECK(toolbar.WinUIIsOverflowChevronVisibleForTesting());
    CHECK(toolbar.WinUIGetOverflowedToolCountForTesting() >= 4);
    CHECK(toolbar.WinUIGetPeerToolCountForTesting() == peerCount);
    CHECK_FALSE(toolbar.WinUIGetOverflowChevronNameForTesting().empty());

    wxWinUIToolPeerSnapshot control;
    wxWinUIToolPeerSnapshot stretch;
    wxWinUIToolPeerSnapshot check;
    wxWinUIToolPeerSnapshot firstRadio;
    wxWinUIToolPeerSnapshot secondRadio;
    wxWinUIToolPeerSnapshot dropdown;
    REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(601, &control));
    REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(
        wxID_SEPARATOR, &stretch));
    REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(611, &check));
    REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(612, &firstRadio));
    REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(613, &secondRadio));
    REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(614, &dropdown));
    CHECK(control.control);
    CHECK_FALSE(control.overflowEligible);
    CHECK_FALSE(control.overflowed);
    CHECK(stretch.stretchable);
    CHECK_FALSE(stretch.overflowEligible);
    CHECK_FALSE(stretch.overflowed);
    CHECK(check.overflowed);
    CHECK(firstRadio.overflowed);
    CHECK(secondRadio.overflowed);
    CHECK(dropdown.overflowed);
    CHECK(check.bounds.IsEmpty());
    CHECK(check.automationName == "Check");
    CHECK(dropdown.automationName == "Dropdown");
    CHECK(check.peerIdentity == checkBefore.peerIdentity);
    CHECK(firstRadio.peerIdentity ==
          firstRadioBefore.peerIdentity);
    CHECK(dropdown.peerIdentity == dropdownBefore.peerIdentity);
    CHECK(check.supportsTogglePattern);
    CHECK(firstRadio.radioAutomationRole);
    CHECK(firstRadio.supportsSelectionItemPattern);
    CHECK(dropdown.supportsInvokePattern);

    int checkEvents = 0;
    int radioEvents = 0;
    int enterEvents = 0;
    int lastEnteredTool = wxID_NONE;
    int rightClickEvents = 0;
    bool lastCheckState = false;
    toolbar.Bind(
        wxEVT_TOOL,
        [&](wxCommandEvent& event)
        {
            ++checkEvents;
            CHECK(event.GetId() == 611);
            lastCheckState = event.IsChecked();
        },
        611);
    toolbar.Bind(
        wxEVT_TOOL,
        [&](wxCommandEvent& event)
        {
            ++radioEvents;
            CHECK(event.IsChecked());
        },
        612, 613);
    toolbar.Bind(
        wxEVT_TOOL_ENTER,
        [&](wxCommandEvent& event)
        {
            ++enterEvents;
            lastEnteredTool = event.GetInt();
        });
    toolbar.Bind(
        wxEVT_TOOL_RCLICKED,
        [&](wxCommandEvent& event)
        {
            ++rightClickEvents;
            CHECK(event.GetId() == 611);
        },
        611);

    REQUIRE(toolbar.WinUIHoverToolForTesting(611, true));
    CHECK(enterEvents == 1);
    CHECK(lastEnteredTool == 611);
    REQUIRE(toolbar.WinUIHoverToolForTesting(611, false));
    CHECK(enterEvents == 2);
    CHECK(lastEnteredTool == wxID_ANY);
    REQUIRE(toolbar.WinUIRightClickToolForTesting(611));
    CHECK(rightClickEvents == 1);

    REQUIRE(toolbar.WinUIInvokeOverflowToolForTesting(611));
    CHECK(checkEvents == 1);
    CHECK(lastCheckState);
    CHECK(toolbar.GetToolState(611));
    REQUIRE(toolbar.WinUIInvokeOverflowToolForTesting(612));
    REQUIRE(toolbar.WinUIInvokeOverflowToolForTesting(613));
    CHECK(radioEvents == 2);
    CHECK_FALSE(toolbar.GetToolState(612));
    CHECK(toolbar.GetToolState(613));
    // Layout and island attachment are two separate gates. Require real
    // transformed bounds and a loaded chevron with its live XamlRoot.
    REQUIRE(WaitToolbarCondition(
        [&]()
        {
            const wxRect chevron =
                toolbar.WinUIGetOverflowChevronBoundsForTesting();
            return !chevron.IsEmpty();
        }));
    REQUIRE(WaitToolbarCondition(
        [&]()
        {
            return toolbar.WinUIIsOverflowChevronReadyForTesting();
        }));

    // The isolated desktop runner deliberately does not activate its desktop;
    // WinUI rejects all popup creation there with E_INVALIDARG (the tree UIA
    // provider, Flyout::ShowAt and ShowAttachedFlyout alike). The loaded UIA
    // role/pattern and peer identity are proven above, while the exact
    // generation-checked dropdown route was proven before reparenting.
    // Physical flyout opening remains an active-desktop qualification gate.
    CHECK(dropdownEvents == 1);

    toolbar.EnableTool(611, false);
    CHECK_FALSE(toolbar.WinUIInvokeOverflowToolForTesting(611));
    REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(611, &check));
    CHECK_FALSE(check.enabled);
    CHECK(check.usesDisabledBitmap);
    toolbar.EnableTool(611, true);

    toolbar.Enable(false);
    CHECK_FALSE(toolbar.WinUIInvokeOverflowToolForTesting(613));
    REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(613, &secondRadio));
    CHECK_FALSE(secondRadio.enabled);
    toolbar.Enable(true);

    // Moving back is an identity transfer, not a rebuild: the exact peer
    // count, model state, handler cardinality and UIA name all survive.
    toolbar.SetSize(wxSize(roomyExtent, toolbar.GetSize().y));
    DrainToolbarCallbacks();
    REQUIRE(toolbar.WinUIApplyOverflowExtentForTesting(roomyExtent));
    CHECK_FALSE(toolbar.WinUIIsOverflowChevronVisibleForTesting());
    CHECK(toolbar.WinUIGetOverflowedToolCountForTesting() == 0);
    CHECK(toolbar.WinUIGetPeerToolCountForTesting() == peerCount);
    REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(611, &check));
    REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(613, &secondRadio));
    CHECK_FALSE(check.overflowed);
    CHECK_FALSE(secondRadio.overflowed);
    CHECK(toolbar.GetToolState(611));
    CHECK(toolbar.GetToolState(613));
    CHECK(check.automationName == "Check");
    CHECK(check.peerIdentity == checkBefore.peerIdentity);
    CHECK(secondRadio.peerIdentity != 0);
    CHECK_FALSE(toolbar.WinUIInvokeOverflowToolForTesting(611));
    REQUIRE(toolbar.WinUIInvokeToolForTesting(611));
    CHECK(checkEvents == 2);
    CHECK_FALSE(lastCheckState);
}

TEST_CASE("wxWinUI ToolBar overflow respects logical RTL and vertical ends",
          "[winui-toolbar][overflow][layout][rtl][vertical][dpi][uia]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    const wxBitmapBundle bitmap = MakeToolbarBitmapBundle();

    wxToolBar ltr(
        parent, wxID_ANY, wxDefaultPosition, wxSize(360, 72));
    REQUIRE(ltr.AddTool(620, "First", bitmap));
    REQUIRE(ltr.AddTool(621, "Second", bitmap));
    REQUIRE(ltr.AddTool(622, "Third", bitmap));
    REQUIRE(ltr.Realize());
    REQUIRE(ltr.WinUIApplyOverflowExtentForTesting(ltr.FromDIP(120)));
    wxWinUIToolPeerSnapshot ltrFirst;
    REQUIRE(ltr.WinUIGetToolPeerStateForTesting(620, &ltrFirst));
    const wxRect ltrChevron =
        ltr.WinUIGetOverflowChevronBoundsForTesting();
    CHECK_FALSE(ltrChevron.IsEmpty());
    CHECK(ltrChevron.x > ltrFirst.bounds.x);
    ltr.SetLayoutDirection(wxLayout_RightToLeft);
    REQUIRE(ltr.WinUIApplyOverflowExtentForTesting(ltr.FromDIP(120)));
    REQUIRE(ltr.WinUIGetToolPeerStateForTesting(620, &ltrFirst));
    const wxRect mirroredChevron =
        ltr.WinUIGetOverflowChevronBoundsForTesting();
    CHECK_FALSE(mirroredChevron.IsEmpty());
    CHECK(mirroredChevron.x < ltrFirst.bounds.x);

    wxToolBar rtl(
        parent, wxID_ANY, wxDefaultPosition, wxSize(360, 72));
    rtl.SetLayoutDirection(wxLayout_RightToLeft);
    REQUIRE(rtl.AddTool(630, "First", bitmap));
    REQUIRE(rtl.AddTool(631, "Second", bitmap));
    REQUIRE(rtl.AddTool(632, "Third", bitmap));
    REQUIRE(rtl.Realize());
    REQUIRE(rtl.WinUIApplyOverflowExtentForTesting(rtl.FromDIP(120)));
    wxWinUIToolPeerSnapshot rtlFirst;
    REQUIRE(rtl.WinUIGetToolPeerStateForTesting(630, &rtlFirst));
    const wxRect rtlChevron =
        rtl.WinUIGetOverflowChevronBoundsForTesting();
    CHECK_FALSE(rtlChevron.IsEmpty());
    CHECK(rtlChevron.x < rtlFirst.bounds.x);

    wxToolBar vertical(
        parent, wxID_ANY, wxDefaultPosition, wxSize(92, 320),
        wxTB_VERTICAL | wxTB_TEXT);
    REQUIRE(vertical.AddTool(640, "First", bitmap));
    REQUIRE(vertical.AddTool(641, "Second", bitmap));
    REQUIRE(vertical.AddTool(642, "Third", bitmap));
    REQUIRE(vertical.Realize());
    REQUIRE(vertical.WinUIApplyOverflowExtentForTesting(
        vertical.FromDIP(150)));
    wxWinUIToolPeerSnapshot verticalFirst;
    REQUIRE(vertical.WinUIGetToolPeerStateForTesting(
        640, &verticalFirst));
    const wxRect verticalChevron =
        vertical.WinUIGetOverflowChevronBoundsForTesting();
    CHECK_FALSE(verticalChevron.IsEmpty());
    CHECK(verticalChevron.y > verticalFirst.bounds.y);

    const size_t beforeDpi =
        vertical.WinUIGetOverflowedToolCountForTesting();
    REQUIRE(vertical.WinUIRefreshForScaleForTesting(2.0));
    REQUIRE(vertical.WinUIApplyOverflowExtentForTesting(
        vertical.FromDIP(150)));
    CHECK(vertical.WinUIGetOverflowedToolCountForTesting() == beforeDpi);
    CHECK_FALSE(
        vertical.WinUIGetOverflowChevronNameForTesting().empty());
}

TEST_CASE("wxWinUI ToolBar overflow mutation is bounded and destruction-safe",
          "[winui-toolbar][overflow][transaction][reentrancy][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    const wxBitmapBundle bitmap = MakeToolbarBitmapBundle();

    wxToolBar toolbar(
        parent, wxID_ANY, wxDefaultPosition, wxSize(360, 72));
    REQUIRE(toolbar.AddTool(650, "First", bitmap));
    REQUIRE(toolbar.AddTool(651, "Second", bitmap));
    REQUIRE(toolbar.AddTool(652, "Third", bitmap));
    REQUIRE(toolbar.Realize());
    REQUIRE(toolbar.WinUIApplyOverflowExtentForTesting(
        toolbar.FromDIP(1000)));

    wxWinUIToolPeerSnapshot baselineFirst;
    wxWinUIToolPeerSnapshot baselineSecond;
    wxWinUIToolPeerSnapshot baselineThird;
    REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(
        650, &baselineFirst));
    REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(
        651, &baselineSecond));
    REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(
        652, &baselineThird));

    // Boundary 2 is exactly RemoveAt(source) -> InsertAt(destination),
    // boundary 3 is after destination insertion, and boundary 7 is the
    // chevron publication for this three-command topology.
    for ( const unsigned boundary : { 2u, 3u, 7u } )
    {
        REQUIRE(toolbar.WinUIApplyOverflowExtentForTesting(
            toolbar.FromDIP(1000)));
        toolbar.WinUIFailNextOverflowMutationForTesting(boundary);
        {
            wxLogNull suppressExpectedFault;
            CHECK_FALSE(toolbar.WinUIApplyOverflowExtentForTesting(
                toolbar.FromDIP(120)));
        }

        wxWinUIToolPeerSnapshot first;
        wxWinUIToolPeerSnapshot second;
        wxWinUIToolPeerSnapshot third;
        REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(650, &first));
        REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(651, &second));
        REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(652, &third));
        CHECK_FALSE(first.overflowed);
        CHECK_FALSE(second.overflowed);
        CHECK_FALSE(third.overflowed);
        CHECK_FALSE(toolbar.WinUIIsOverflowChevronVisibleForTesting());
        CHECK(toolbar.WinUIGetOverflowedToolCountForTesting() == 0);
        CHECK(first.peerIdentity == baselineFirst.peerIdentity);
        CHECK(second.peerIdentity == baselineSecond.peerIdentity);
        CHECK(third.peerIdentity == baselineThird.peerIdentity);
    }

    OverflowMutationProbe nested;
    nested.nestedExtent = toolbar.FromDIP(1000);
    toolbar.WinUISetNextOverflowMutationHookForTesting(
        &ReenterOverflowMutation, &nested);
    REQUIRE(toolbar.WinUIApplyOverflowExtentForTesting(
        toolbar.FromDIP(120)));
    CHECK(nested.invoked);
    CHECK_FALSE(nested.nestedResult);
    CHECK(toolbar.WinUIIsOverflowChevronVisibleForTesting());
    CHECK(toolbar.WinUIGetOverflowedToolCountForTesting() == 2);

    REQUIRE(toolbar.WinUIApplyOverflowExtentForTesting(
        toolbar.FromDIP(1000)));
    OverflowEnableMutationProbe enableMutation;
    enableMutation.toolId = 652;
    toolbar.WinUISetNextOverflowMutationHookForTesting(
        &DisableToolDuringOverflow, &enableMutation);
    REQUIRE(toolbar.WinUIApplyOverflowExtentForTesting(
        toolbar.FromDIP(120)));
    CHECK(enableMutation.invoked);
    wxWinUIToolPeerSnapshot disabled;
    REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(652, &disabled));
    CHECK_FALSE(disabled.enabled);
    CHECK(disabled.overflowed);

    toolbar.EnableTool(652, true);
    REQUIRE(toolbar.WinUIApplyOverflowExtentForTesting(
        toolbar.FromDIP(1000)));
    NestedRebuildProbe replacement;
    toolbar.WinUISetNextOverflowMutationHookForTesting(
        &RequestNestedRebuild, &replacement);
    CHECK_FALSE(toolbar.WinUIApplyOverflowExtentForTesting(
        toolbar.FromDIP(120)));
    CHECK(replacement.invoked);
    CHECK(replacement.nestedResult);
    CHECK(toolbar.WinUIGetPeerToolCountForTesting() == 3);

    DrainToolbarCallbacks();
    const size_t baseline =
        wxToolBar::WinUIGetLiveCallbackStateCountForTesting();
    wxToolBar * const doomed = new wxToolBar(
        parent, wxID_ANY, wxDefaultPosition, wxSize(360, 72));
    REQUIRE(doomed->AddTool(660, "First", bitmap));
    REQUIRE(doomed->AddTool(661, "Second", bitmap));
    REQUIRE(doomed->AddTool(662, "Third", bitmap));
    REQUIRE(doomed->Realize());
    REQUIRE(doomed->WinUIApplyOverflowExtentForTesting(
        doomed->FromDIP(1000)));
    wxWeakRef<wxWindow> lifetime(doomed);
    DestroyDuringLoadedProbe destruction;
    doomed->WinUISetNextOverflowMutationHookForTesting(
        &DestroyToolbarDuringLoaded, &destruction);
    CHECK_FALSE(doomed->WinUIApplyOverflowExtentForTesting(
        doomed->FromDIP(120)));
    CHECK(destruction.invoked);
    CHECK(lifetime.get() == nullptr);
    CHECK(
        wxToolBar::WinUIGetLiveCallbackStateCountForTesting() ==
        baseline);
}

TEST_CASE("wxWinUI ToolBar rejects a control candidate destroyed in Loaded",
          "[winui-toolbar][control][transaction][lifetime][reentrancy]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    DrainToolbarCallbacks();
    const size_t callbackBaseline =
        wxToolBar::WinUIGetLiveCallbackStateCountForTesting();

    {
        wxToolBar toolbar(
            parent, wxID_ANY, wxDefaultPosition, wxSize(360, 72));
        REQUIRE(toolbar.AddTool(
            350, "stable", MakeToolbarBitmapBundle()));
        REQUIRE(toolbar.Realize());
        const size_t peerCount =
            toolbar.WinUIGetPeerToolCountForTesting();
        const size_t liveCallbackCount =
            wxToolBar::WinUIGetLiveCallbackStateCountForTesting();

        wxButton * const candidate =
            new wxButton(&toolbar, 351, "Candidate",
                         wxDefaultPosition, wxSize(92, 30));
        wxWeakRef<wxWindow> candidateLifetime(candidate);
        DestroyControlDuringLoadedProbe destruction{candidate};
        toolbar.WinUISetNextRebuildLoadedHookForTesting(
            &DestroyControlDuringLoaded, &destruction);

        CHECK(toolbar.AddControl(candidate, "candidate") == nullptr);
        CHECK(destruction.invoked);
        CHECK(candidateLifetime.get() == nullptr);
        CHECK(toolbar.FindById(351) == nullptr);
        CHECK(toolbar.GetToolsCount() == 1);
        CHECK(toolbar.WinUIGetPeerToolCountForTesting() == peerCount);
        CHECK(
            wxToolBar::WinUIGetLiveCallbackStateCountForTesting() ==
            liveCallbackCount);
    }

    CHECK(
        wxToolBar::WinUIGetLiveCallbackStateCountForTesting() ==
        callbackBaseline);
}

TEST_CASE("wxWinUI ToolBar insertion has one transactional owner",
          "[winui-toolbar][transaction][lifetime][reentrancy][ownership]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    const wxBitmapBundle bitmap = MakeToolbarBitmapBundle();

    wxToolBar source(
        parent, wxID_ANY, wxDefaultPosition, wxSize(240, 64));
    wxToolBarToolBase * const created =
        source.AddTool(353, "transfer", bitmap);
    REQUIRE(created);
    wxToolBarToolBase * const candidate = source.RemoveTool(353);
    REQUIRE(candidate == created);
    REQUIRE(candidate->GetToolBar() == nullptr);

    wxToolBar first(
        parent, wxID_ANY, wxDefaultPosition, wxSize(300, 64));
    wxToolBar second(
        parent, wxID_ANY, wxDefaultPosition, wxSize(300, 64));
    REQUIRE(first.AddTool(354, "first-stable", bitmap));
    REQUIRE(second.AddTool(355, "second-stable", bitmap));
    REQUIRE(first.Realize());
    REQUIRE(second.Realize());

    CrossToolbarInsertProbe probe{&second, candidate};
    first.WinUISetNextRebuildLoadedHookForTesting(
        &InsertCandidateIntoOtherToolbar, &probe);
    REQUIRE(first.AddTool(candidate) == candidate);

    CHECK(probe.invoked);
    CHECK(probe.nestedResult == nullptr);
    CHECK(candidate->GetToolBar() == &first);
    CHECK(first.GetToolsCount() == 2);
    CHECK(first.WinUIGetPeerToolCountForTesting() == 2);
    CHECK(second.GetToolsCount() == 1);
    CHECK(second.WinUIGetPeerToolCountForTesting() == 1);

    // The same wrapper cannot be published a second time after the outer
    // insertion has committed either.
    CHECK(second.AddTool(candidate) == nullptr);
    CHECK(candidate->GetToolBar() == &first);
    CHECK(first.GetToolsCount() == 2);
    CHECK(second.GetToolsCount() == 1);
}

TEST_CASE("wxWinUI ToolBar neutralizes controls destroyed during removal",
          "[winui-toolbar][control][transaction][lifetime][reentrancy]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxToolBar toolbar(
        parent, wxID_ANY, wxDefaultPosition, wxSize(360, 72));
    wxButton * const embedded =
        new wxButton(&toolbar, 355, "Embedded",
                     wxDefaultPosition, wxSize(92, 30));
    REQUIRE(toolbar.AddControl(embedded, "control"));
    REQUIRE(toolbar.AddTool(
        356, "stable", MakeToolbarBitmapBundle()));
    REQUIRE(toolbar.Realize());

    wxWeakRef<wxWindow> embeddedLifetime(embedded);
    DestroyControlDuringLoadedProbe destruction{embedded};
    toolbar.WinUISetNextRebuildLoadedHookForTesting(
        &DestroyControlDuringLoaded, &destruction);

    wxToolBarToolBase * const removed = toolbar.RemoveTool(355);
    REQUIRE(removed);
    CHECK(destruction.invoked);
    CHECK(embeddedLifetime.get() == nullptr);
    CHECK(removed->GetControl() == nullptr);
    CHECK(toolbar.GetToolsCount() == 1);
    CHECK(toolbar.WinUIGetPeerToolCountForTesting() == 1);

    // The detached tool has already forgotten the dead control.
    delete removed;

    wxButton * const clearing =
        new wxButton(&toolbar, 359, "Clear",
                     wxDefaultPosition, wxSize(92, 30));
    REQUIRE(toolbar.AddControl(clearing, "clear-control"));
    wxWeakRef<wxWindow> clearingLifetime(clearing);
    DestroyControlDuringLoadedProbe clearDestruction{clearing};
    toolbar.WinUISetNextRebuildLoadedHookForTesting(
        &DestroyControlDuringLoaded, &clearDestruction);

    toolbar.ClearTools();
    CHECK(clearDestruction.invoked);
    CHECK(clearingLifetime.get() == nullptr);
    CHECK(toolbar.GetToolsCount() == 0);
    CHECK(toolbar.WinUIGetPeerToolCountForTesting() == 0);

    wxButton * const firstCleared =
        new wxButton(&toolbar, 360, "First clear",
                     wxDefaultPosition, wxSize(92, 30));
    wxButton * const laterReinserted =
        new wxButton(&toolbar, 361, "Later clear",
                     wxDefaultPosition, wxSize(92, 30));
    wxToolBarToolBase * const firstClearedTool =
        toolbar.AddControl(firstCleared, "first-clear");
    wxToolBarToolBase * const laterReinsertedTool =
        toolbar.AddControl(laterReinserted, "later-clear");
    REQUIRE(firstClearedTool);
    REQUIRE(laterReinsertedTool);

    wxToolBarToolBase *nestedReinsertion = nullptr;
    firstCleared->Bind(
        wxEVT_DESTROY,
        [&](wxWindowDestroyEvent& event)
        {
            nestedReinsertion = toolbar.AddTool(laterReinsertedTool);
            event.Skip();
        });

    toolbar.ClearTools();
    CHECK(nestedReinsertion == laterReinsertedTool);
    CHECK(laterReinsertedTool->GetToolBar() == &toolbar);
    CHECK(toolbar.GetToolsCount() == 1);
    CHECK(toolbar.FindById(361) == laterReinsertedTool);
    CHECK(toolbar.WinUIGetPeerToolCountForTesting() == 1);

    toolbar.ClearTools();
    CHECK(toolbar.GetToolsCount() == 0);

    wxButton * const roundTripTrigger =
        new wxButton(&toolbar, 364, "Round-trip trigger",
                     wxDefaultPosition, wxSize(92, 30));
    wxButton * const roundTripControl =
        new wxButton(&toolbar, 365, "Round-trip transfer",
                     wxDefaultPosition, wxSize(92, 30));
    REQUIRE(toolbar.AddControl(roundTripTrigger, "round-trip-trigger"));
    wxToolBarToolBase * const roundTripTool =
        toolbar.AddControl(roundTripControl, "round-trip-transfer");
    REQUIRE(roundTripTool);
    wxToolBarToolBase *nestedTransfer = nullptr;
    roundTripTrigger->Bind(
        wxEVT_DESTROY,
        [&](wxWindowDestroyEvent& event)
        {
            REQUIRE(toolbar.AddTool(roundTripTool) == roundTripTool);
            nestedTransfer = toolbar.RemoveTool(365);
            event.Skip();
        });

    // Reinsert+remove is an ABA for a plain attached/null ownership check.
    // The outer ClearTools() must observe the ownership epoch and leave the
    // twice-transferred wrapper to the nested RemoveTool() caller.
    toolbar.ClearTools();
    REQUIRE(nestedTransfer == roundTripTool);
    CHECK(nestedTransfer->GetToolBar() == nullptr);
    CHECK(toolbar.GetToolsCount() == 0);
    CHECK(toolbar.WinUIGetPeerToolCountForTesting() == 0);
    delete nestedTransfer;
}

TEST_CASE("wxWinUI ToolBar candidate cleanup survives owner destruction",
          "[winui-toolbar][control][transaction][lifetime][reentrancy]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    DrainToolbarCallbacks();
    const size_t callbackBaseline =
        wxToolBar::WinUIGetLiveCallbackStateCountForTesting();

    wxToolBar * const doomed = new wxToolBar(
        parent, wxID_ANY, wxDefaultPosition, wxSize(360, 72));
    REQUIRE(doomed->AddTool(
        357, "stable", MakeToolbarBitmapBundle()));
    REQUIRE(doomed->Realize());
    wxButton * const candidate =
        new wxButton(doomed, 358, "Candidate",
                     wxDefaultPosition, wxSize(92, 30));
    wxWeakRef<wxWindow> toolbarLifetime(doomed);
    wxWeakRef<wxWindow> candidateLifetime(candidate);

    DestroyDuringLoadedProbe destruction;
    doomed->WinUISetNextRebuildLoadedHookForTesting(
        &DestroyToolbarDuringLoaded, &destruction);
    CHECK(doomed->AddControl(candidate, "candidate") == nullptr);

    CHECK(destruction.invoked);
    CHECK(toolbarLifetime.get() == nullptr);
    CHECK(candidateLifetime.get() == nullptr);
    CHECK(
        wxToolBar::WinUIGetLiveCallbackStateCountForTesting() ==
        callbackBaseline);
}

TEST_CASE("wxWinUI ToolBar control removal commits before Hide re-entry",
          "[winui-toolbar][control][transaction][reentrancy][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxToolBar toolbar(
        parent, wxID_ANY, wxDefaultPosition, wxSize(360, 72));
    wxButton * const embedded =
        new wxButton(&toolbar, 361, "Embedded",
                     wxDefaultPosition, wxSize(92, 30));
    wxToolBarToolBase * const controlTool =
        toolbar.AddControl(embedded, "control");
    REQUIRE(controlTool);
    REQUIRE(toolbar.AddTool(
        362, "other", MakeToolbarBitmapBundle()));
    REQUIRE(toolbar.Realize());
    DrainToolbarCallbacks();
    REQUIRE(embedded->IsShown());

    int hideEvents = 0;
    wxToolBarToolBase *recursiveRemoval = nullptr;
    embedded->Bind(
        wxEVT_SHOW,
        [&](wxShowEvent& event)
        {
            if ( !event.IsShown() )
            {
                ++hideEvents;
                // The outer transaction must already have erased this tool.
                recursiveRemoval = toolbar.RemoveTool(361);
            }
            event.Skip();
        });

    wxToolBarToolBase * const removed = toolbar.RemoveTool(361);
    REQUIRE(removed == controlTool);
    CHECK(hideEvents == 1);
    CHECK(recursiveRemoval == nullptr);
    CHECK(toolbar.FindById(361) == nullptr);
    CHECK(toolbar.GetToolsCount() == 1);
    CHECK(toolbar.WinUIGetPeerToolCountForTesting() == 1);

    wxWeakRef<wxWindow> embeddedLifetime(embedded);
    delete removed;
    DrainToolbarCallbacks();
    CHECK(embeddedLifetime.get() == nullptr);

    wxToolBar reinsertionToolbar(
        parent, wxID_ANY, wxDefaultPosition, wxSize(360, 72));
    wxButton * const reinsertedControl =
        new wxButton(&reinsertionToolbar, 363, "Reinserted",
                     wxDefaultPosition, wxSize(92, 30));
    wxToolBarToolBase * const reinsertedTool =
        reinsertionToolbar.AddControl(reinsertedControl, "reinserted");
    REQUIRE(reinsertedTool);
    REQUIRE(reinsertionToolbar.Realize());
    DrainToolbarCallbacks();

    int reinsertHideEvents = 0;
    wxToolBarToolBase *nestedInsertion = nullptr;
    wxToolBarToolBase *nestedTransfer = nullptr;
    bool roundTrip = false;
    bool handlingRoundTrip = false;
    reinsertedControl->Bind(
        wxEVT_SHOW,
        [&](wxShowEvent& event)
        {
            if ( !event.IsShown() && !handlingRoundTrip )
            {
                handlingRoundTrip = true;
                ++reinsertHideEvents;
                nestedInsertion =
                    reinsertionToolbar.AddTool(reinsertedTool);
                if ( roundTrip )
                {
                    nestedTransfer =
                        reinsertionToolbar.RemoveTool(363);
                }
                handlingRoundTrip = false;
            }
            event.Skip();
        });

    // The Hide callback takes ownership back before the outer RemoveTool()
    // returns. The stale outer caller must report no transfer and must never
    // hand the same wrapper to two owners.
    CHECK(reinsertionToolbar.RemoveTool(363) == nullptr);
    CHECK(reinsertHideEvents == 1);
    CHECK(nestedInsertion == reinsertedTool);
    CHECK(reinsertedTool->GetToolBar() == &reinsertionToolbar);
    CHECK(reinsertionToolbar.FindById(363) == reinsertedTool);
    CHECK(reinsertionToolbar.GetToolsCount() == 1);
    CHECK(reinsertionToolbar.WinUIGetPeerToolCountForTesting() == 1);
    DrainToolbarCallbacks();

    roundTrip = true;
    // The nested writer briefly reattaches and transfers the wrapper back out.
    // A final null toolbar pointer alone cannot distinguish this ABA from the
    // original outer detach; the ownership epoch must make the outer call
    // report no transfer.
    CHECK(reinsertionToolbar.RemoveTool(363) == nullptr);
    REQUIRE(nestedTransfer == reinsertedTool);
    CHECK(nestedTransfer->GetToolBar() == nullptr);
    CHECK(reinsertionToolbar.GetToolsCount() == 0);
    CHECK(reinsertionToolbar.WinUIGetPeerToolCountForTesting() == 0);
    delete nestedTransfer;
}

TEST_CASE("wxWinUI ToolBar DeleteToolByPos uses identity with duplicate IDs",
          "[winui-toolbar][control][transaction][duplicate-id]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxToolBar toolbar(
        parent, wxID_ANY, wxDefaultPosition, wxSize(360, 72));
    wxButton * const first =
        new wxButton(&toolbar, 365, "First",
                     wxDefaultPosition, wxSize(82, 30));
    wxButton * const second =
        new wxButton(&toolbar, 365, "Second",
                     wxDefaultPosition, wxSize(82, 30));
    wxToolBarToolBase * const firstTool =
        toolbar.AddControl(first, "first");
    wxToolBarToolBase * const secondTool =
        toolbar.AddControl(second, "second");
    REQUIRE(firstTool);
    REQUIRE(secondTool);
    REQUIRE(toolbar.Realize());
    CHECK(toolbar.WinUIGetPeerToolCountForTesting() == 2);

    wxWeakRef<wxWindow> secondLifetime(second);
    REQUIRE(toolbar.DeleteToolByPos(1));
    DrainToolbarCallbacks();

    CHECK(secondLifetime.get() == nullptr);
    CHECK(toolbar.GetToolsCount() == 1);
    CHECK(toolbar.FindById(365) == firstTool);
    CHECK(toolbar.WinUIGetPeerToolCountForTesting() == 1);
    wxWinUIToolPeerSnapshot state;
    REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(365, &state));
    CHECK(state.control);
}

#if wxUSE_TOOLTIPS

TEST_CASE("wxWinUI ToolBar restores embedded control tooltip ownership exactly",
          "[winui-toolbar][control][tooltip][transaction][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxToolBar toolbar(
        parent, wxID_ANY, wxDefaultPosition, wxSize(420, 72));
    wxButton * const embedded =
        new wxButton(&toolbar, 371, "Embedded",
                     wxDefaultPosition, wxSize(92, 30));
    wxToolTip * const baseline = new wxToolTip("application-baseline");
    embedded->SetToolTip(baseline);
    wxToolBarToolBase * const tool =
        toolbar.AddControl(embedded, "control");
    REQUIRE(tool);
    toolbar.SetToolShortHelp(371, "toolbar-tip");
    REQUIRE(toolbar.Realize());

    CHECK(embedded->GetToolTip() == baseline);
    CHECK(baseline->GetTip() == "toolbar-tip");

    wxToolBarToolBase *removed = toolbar.RemoveTool(371);
    REQUIRE(removed == tool);
    CHECK(embedded->GetToolTip() == baseline);
    CHECK(baseline->GetTip() == "application-baseline");

    REQUIRE(toolbar.AddTool(removed) == removed);
    CHECK(embedded->GetToolTip() == baseline);
    CHECK(baseline->GetTip() == "toolbar-tip");
    baseline->SetTip("application-mutated");
    removed = toolbar.RemoveTool(371);
    REQUIRE(removed == tool);
    CHECK(embedded->GetToolTip() == baseline);
    CHECK(baseline->GetTip() == "application-mutated");

    REQUIRE(toolbar.AddTool(removed) == removed);
    CHECK(embedded->GetToolTip() == baseline);
    CHECK(baseline->GetTip() == "toolbar-tip");
    wxToolTip * const replacement =
        new wxToolTip("application-replacement");
    embedded->SetToolTip(replacement);
    removed = toolbar.RemoveTool(371);
    REQUIRE(removed == tool);
    CHECK(embedded->GetToolTip() == replacement);
    CHECK(replacement->GetTip() == "application-replacement");
    delete removed;

    wxToolBar withoutBaseline(
        parent, wxID_ANY, wxDefaultPosition, wxSize(420, 72));
    wxButton * const generated =
        new wxButton(&withoutBaseline, 372, "Generated",
                     wxDefaultPosition, wxSize(92, 30));
    wxToolBarToolBase * const generatedTool =
        withoutBaseline.AddControl(generated, "control");
    REQUIRE(generatedTool);
    withoutBaseline.SetToolShortHelp(372, "toolbar-created");
    REQUIRE(withoutBaseline.Realize());
    REQUIRE(generated->GetToolTip() != nullptr);
    CHECK(generated->GetToolTip()->GetTip() == "toolbar-created");
    wxToolBarToolBase * const generatedRemoved =
        withoutBaseline.RemoveTool(372);
    REQUIRE(generatedRemoved == generatedTool);
    CHECK(generated->GetToolTip() == nullptr);
    delete generatedRemoved;

    wxToolBar closePath(
        parent, wxID_ANY, wxDefaultPosition, wxSize(420, 72));
    wxButton * const closing =
        new wxButton(&closePath, 373, "Closing",
                     wxDefaultPosition, wxSize(92, 30));
    wxToolTip * const closeBaseline =
        new wxToolTip("close-baseline");
    closing->SetToolTip(closeBaseline);
    REQUIRE(closePath.AddControl(closing, "control"));
    closePath.SetToolShortHelp(373, "close-toolbar");
    REQUIRE(closePath.Realize());
    REQUIRE(closing->GetToolTip() == closeBaseline);
    CHECK(closeBaseline->GetTip() == "close-toolbar");
    closePath.WinUIClosePeerForTesting();
    CHECK(closing->GetToolTip() == closeBaseline);
    CHECK(closeBaseline->GetTip() == "close-baseline");
}

TEST_CASE("wxWinUI ToolBar never adopts a same-address tooltip ABA",
          "[winui-toolbar][control][tooltip][transaction][ownership]")
{
    class ReplacingToolTipButton final : public wxButton
    {
    public:
        using wxButton::wxButton;

        wxToolTip *replacement = nullptr;
        unsigned long long originalIdentity = 0;
        unsigned long long replacementIdentity = 0;

    protected:
        void DoSetToolTip(wxToolTip *tip) override
        {
            if ( tip && !replacement )
            {
                const wxString text = tip->GetTip();
                // Adopt the supplied object first, then replace it from the
                // virtual boundary with an application-owned equal-text object
                // at the exact same address. This deterministically exercises
                // allocator ABA instead of relying on heap reuse by chance.
                wxButton::DoSetToolTip(tip);
                originalIdentity = tip->GetWinUIIdentity();
                tip->~wxToolTip();
                replacement = new (tip) wxToolTip(text);
                replacementIdentity = replacement->GetWinUIIdentity();
                return;
            }
            wxButton::DoSetToolTip(tip);
        }
    };

    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    wxToolBar toolbar(
        parent, wxID_ANY, wxDefaultPosition, wxSize(360, 72));
    ReplacingToolTipButton * const control =
        new ReplacingToolTipButton(
            &toolbar, 374, "Replacing",
            wxDefaultPosition, wxSize(92, 30));
    wxToolBarToolBase * const tool =
        toolbar.AddControl(control, "control");
    REQUIRE(tool);
    toolbar.SetToolShortHelp(374, "same text");
    REQUIRE(toolbar.Realize());
    REQUIRE(control->replacement != nullptr);
    REQUIRE(control->GetToolTip() == control->replacement);
    CHECK(control->originalIdentity != control->replacementIdentity);
    CHECK(control->GetToolTip()->GetTip() == "same text");

    wxToolBarToolBase * const removed = toolbar.RemoveTool(374);
    REQUIRE(removed == tool);
    CHECK(control->GetToolTip() == control->replacement);
    CHECK(control->GetToolTip()->GetTip() == "same text");
    delete removed;
}

#endif // wxUSE_TOOLTIPS

#endif // wxUSE_BUTTON

TEST_CASE("wxWinUI ToolBar rebuild is bounded under Loaded re-entry",
          "[winui-toolbar][transaction][lifetime][reentrancy]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    const wxBitmapBundle bitmap = MakeToolbarBitmapBundle();

    wxToolBar toolbar(
        parent, wxID_ANY, wxDefaultPosition, wxSize(240, 64));
    REQUIRE(toolbar.AddTool(391, "stable", bitmap));
    REQUIRE(toolbar.Realize());
    const size_t peerCount =
        toolbar.WinUIGetPeerToolCountForTesting();

    NestedRebuildProbe nested;
    toolbar.WinUISetNextRebuildLoadedHookForTesting(
        &RequestNestedRebuild, &nested);
    CHECK(toolbar.WinUIRefreshForScaleForTesting(2.0));
    CHECK(nested.invoked);
    CHECK_FALSE(nested.nestedResult);
    CHECK(toolbar.WinUIGetPeerToolCountForTesting() == peerCount);

    wxWinUIToolPeerSnapshot state;
    REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(391, &state));
    CHECK(state.selectedPixelSize == wxSize(32, 32));

    LoadedMutationProbe mutation;
    mutation.toolId = 391;
    toolbar.WinUISetNextRebuildLoadedHookForTesting(
        &MutateCandidateDuringLoaded, &mutation);
    CHECK(toolbar.WinUIRefreshForScaleForTesting(1.5));
    CHECK(mutation.invoked);
    CHECK(mutation.sawCandidate);
    REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(391, &state));
    CHECK_FALSE(state.enabled);
    CHECK(state.toolTip == "loaded-final");
    CHECK(state.peerToolTip == "loaded-final");
    CHECK(state.helpText == "loaded-final");
}

TEST_CASE("wxWinUI ToolBar label publication is transactional and last-writer wins",
          "[winui-toolbar][label][uia][transaction][reentrancy]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    const wxBitmapBundle bitmap = MakeToolbarBitmapBundle();

    wxToolBar toolbar(
        parent, wxID_ANY, wxDefaultPosition, wxSize(300, 64),
        wxTB_HORIZONTAL | wxTB_TEXT);
    wxToolBarToolBase * const stable =
        toolbar.AddTool(392, "initial", bitmap);
    REQUIRE(stable);
    REQUIRE(toolbar.Realize());

    // A failed outer candidate must retain and project the nested label writer
    // onto the restored generation, not roll it back with the unrelated peer.
    LoadedLabelProbe nested{stable, "nested-final"};
    toolbar.WinUISetNextRebuildLoadedHookForTesting(
        &SetToolLabelDuringLoaded, &nested);
    toolbar.WinUIFailNextRebuildForTesting();
    stable->SetLabel("outer-obsolete");
    CHECK(nested.invoked);
    CHECK(stable->GetLabel() == "nested-final");
    wxWinUIToolPeerSnapshot state;
    REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(392, &state));
    CHECK(state.automationName == "nested-final");

    // The wrapper passed to AddTool() is still detached while DoInsertTool()
    // runs. A Loaded mutation on that exact candidate must be reconciled
    // before the generation commits.
    wxToolBarToolBase * const detached =
        toolbar.AddTool(393, "candidate-old", bitmap);
    REQUIRE(detached);
    REQUIRE(toolbar.RemoveTool(393) == detached);
    LoadedLabelProbe candidateMutation{detached, "candidate-final"};
    toolbar.WinUISetNextRebuildLoadedHookForTesting(
        &SetToolLabelDuringLoaded, &candidateMutation);
    REQUIRE(toolbar.AddTool(detached) == detached);
    CHECK(candidateMutation.invoked);
    CHECK(detached->GetLabel() == "candidate-final");
    REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(393, &state));
    CHECK(state.automationName == "candidate-final");
}

TEST_CASE("wxWinUI ToolBar short help converges across re-entry and destroy",
          "[winui-toolbar][tooltip][transaction][lifetime][reentrancy]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    const wxBitmapBundle bitmap = MakeToolbarBitmapBundle();

    wxToolBar toolbar(
        parent, wxID_ANY, wxDefaultPosition, wxSize(240, 64));
    REQUIRE(toolbar.AddTool(393, "stable", bitmap, "initial"));
    REQUIRE(toolbar.Realize());

    ReentrantShortHelpProbe nested;
    nested.toolId = 393;
    toolbar.WinUISetNextShortHelpSetterHookForTesting(
        &ReplaceShortHelpFromSetter, &nested);
    toolbar.SetToolShortHelp(393, "outer-obsolete");
    CHECK(nested.invoked);

    wxWinUIToolPeerSnapshot state;
    REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(393, &state));
    CHECK(state.toolTip == "nested-final");
    CHECK(state.peerToolTip == "nested-final");
    CHECK(state.helpText == "nested-final");

    DrainToolbarCallbacks();
    const size_t baseline =
        wxToolBar::WinUIGetLiveCallbackStateCountForTesting();
    wxToolBar * const doomed = new wxToolBar(
        parent, wxID_ANY, wxDefaultPosition, wxSize(240, 64));
    REQUIRE(doomed->AddTool(394, "doomed", bitmap, "initial"));
    REQUIRE(doomed->Realize());
    wxWeakRef<wxWindow> lifetime(doomed);

    DestroyDuringShortHelpProbe destruction;
    doomed->WinUISetNextShortHelpSetterHookForTesting(
        &DestroyToolbarDuringShortHelp, &destruction);
    doomed->SetToolShortHelp(394, "destroying");
    CHECK(destruction.invoked);
    CHECK(lifetime.get() == nullptr);
    CHECK(
        wxToolBar::WinUIGetLiveCallbackStateCountForTesting() ==
        baseline);
}

TEST_CASE("wxWinUI ToolBar short-help failure is quarantined per peer",
          "[winui-toolbar][tooltip][transaction][fault][fairness]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    const wxBitmapBundle bitmap = MakeToolbarBitmapBundle();

    wxToolBar toolbar(
        parent, wxID_ANY, wxDefaultPosition, wxSize(300, 64));
    REQUIRE(toolbar.AddTool(395, "failing", bitmap, "failing-initial"));
    REQUIRE(toolbar.AddTool(396, "healthy", bitmap, "healthy-initial"));
    REQUIRE(toolbar.Realize());

    OtherShortHelpProbe other;
    other.toolId = 396;
    toolbar.WinUISetNextShortHelpSetterHookForTesting(
        &QueueOtherShortHelpFromSetter, &other);
    toolbar.WinUIFailNextShortHelpSettersForTesting(395, 3);

    {
        // The warning is the expected observable quarantine diagnostic.
        wxLogNull suppressExpectedWarning;
        toolbar.SetToolShortHelp(395, "failing-model");
    }
    CHECK(other.invoked);

    wxWinUIToolPeerSnapshot failing;
    wxWinUIToolPeerSnapshot healthy;
    REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(395, &failing));
    REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(396, &healthy));
    CHECK(failing.toolTip == "failing-model");
    CHECK(failing.peerToolTip == "failing-initial");
    CHECK(failing.helpText == "failing-initial");
    CHECK(healthy.toolTip == "healthy-final");
    CHECK(healthy.peerToolTip == "healthy-final");
    CHECK(healthy.helpText == "healthy-final");

    // A later explicit write re-arms only the quarantined peer.
    toolbar.SetToolShortHelp(395, "recovered");
    REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(395, &failing));
    CHECK(failing.peerToolTip == "recovered");
    CHECK(failing.helpText == "recovered");
}

TEST_CASE("wxWinUI ToolBar short-help re-entry has a finite driver",
          "[winui-toolbar][tooltip][transaction][storm][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    const wxBitmapBundle bitmap = MakeToolbarBitmapBundle();

    wxToolBar toolbar(
        parent, wxID_ANY, wxDefaultPosition, wxSize(240, 64));
    REQUIRE(toolbar.AddTool(397, "storm", bitmap, "initial"));
    REQUIRE(toolbar.Realize());

    ShortHelpStormProbe storm;
    storm.toolId = 397;
    storm.remaining = 1000;
    toolbar.WinUISetNextShortHelpSetterHookForTesting(
        &ContinueShortHelpStorm, &storm);

    {
        wxLogNull suppressExpectedWarning;
        toolbar.SetToolShortHelp(397, "storm-start");
        DrainToolbarCallbacks();
    }

    CHECK(storm.invocations > 1);
    CHECK(storm.invocations <= 96);
    CHECK(storm.remaining > 0);

    // Stop the adversarial hook. A new top-level model write owns a fresh,
    // finite budget and must converge to the final value.
    toolbar.WinUISetNextShortHelpSetterHookForTesting(nullptr, nullptr);
    toolbar.SetToolShortHelp(397, "storm-final");
    DrainToolbarCallbacks();

    wxWinUIToolPeerSnapshot state;
    REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(397, &state));
    CHECK(state.toolTip == "storm-final");
    CHECK(state.peerToolTip == "storm-final");
    CHECK(state.helpText == "storm-final");
}

TEST_CASE("wxWinUI ToolBar enabled projection is last-writer-wins",
          "[winui-toolbar][enabled][transaction][reentrancy][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    const wxBitmapBundle normal = MakeToolbarBitmapBundle();
    const wxBitmapBundle disabled = MakeDisabledToolbarBitmapBundle();

    wxToolBar toolbar(
        parent, wxID_ANY, wxDefaultPosition, wxSize(260, 64));
    REQUIRE(toolbar.AddTool(
        398, "split", normal, disabled, wxITEM_DROPDOWN));
    REQUIRE(toolbar.Realize());

    ReentrantEnableProbe reenable;
    reenable.toolId = 398;
    toolbar.WinUISetNextEnableSetterHookForTesting(
        &ReenableFromIsEnabledBoundary, &reenable);

    // Disabling a focused XAML button can synchronously emit LostFocus. The
    // seam runs at that exact IsEnabled setter boundary without moving the
    // user's real focus.
    toolbar.EnableTool(398, false);
    CHECK(reenable.invoked);
    CHECK(toolbar.GetToolEnabled(398));

    wxWinUIToolPeerSnapshot state;
    REQUIRE(toolbar.WinUIGetToolPeerStateForTesting(398, &state));
    CHECK(state.enabled);
    CHECK_FALSE(state.usesDisabledBitmap);

    DrainToolbarCallbacks();
    const size_t baseline =
        wxToolBar::WinUIGetLiveCallbackStateCountForTesting();
    wxToolBar * const doomed = new wxToolBar(
        parent, wxID_ANY, wxDefaultPosition, wxSize(240, 64));
    REQUIRE(doomed->AddTool(399, "doomed", normal));
    REQUIRE(doomed->Realize());
    wxWeakRef<wxWindow> lifetime(doomed);
    DestroyDuringShortHelpProbe destruction;
    doomed->WinUISetNextEnableSetterHookForTesting(
        &DestroyToolbarDuringShortHelp, &destruction);
    doomed->EnableTool(399, false);
    CHECK(destruction.invoked);
    CHECK(lifetime.get() == nullptr);
    CHECK(
        wxToolBar::WinUIGetLiveCallbackStateCountForTesting() ==
        baseline);
}

TEST_CASE("wxWinUI ToolBar SetContent return survives Loaded destruction",
          "[winui-toolbar][transaction][lifetime][reentrancy]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    const wxBitmapBundle bitmap = MakeToolbarBitmapBundle();
    DrainToolbarCallbacks();
    const size_t baseline =
        wxToolBar::WinUIGetLiveCallbackStateCountForTesting();

    wxToolBar * const doomed = new wxToolBar(
        parent, wxID_ANY, wxDefaultPosition, wxSize(240, 64));
    REQUIRE(doomed->AddTool(392, "doomed", bitmap));
    REQUIRE(doomed->Realize());
    wxWeakRef<wxWindow> lifetime(doomed);

    DestroyDuringLoadedProbe destruction;
    doomed->WinUISetNextRebuildLoadedHookForTesting(
        &DestroyToolbarDuringLoaded, &destruction);
    CHECK_FALSE(doomed->WinUIRefreshForScaleForTesting(2.0));
    CHECK(destruction.invoked);
    CHECK(lifetime.get() == nullptr);
    CHECK(
        wxToolBar::WinUIGetLiveCallbackStateCountForTesting() ==
        baseline);
}

TEST_CASE("wxWinUI ToolBar callbacks do not outlive their owner",
          "[winui-toolbar][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    const wxBitmapBundle bitmap = MakeToolbarBitmapBundle();
    DrainToolbarCallbacks();
    const size_t baseline =
        wxToolBar::WinUIGetLiveCallbackStateCountForTesting();

    for ( int i = 0; i < 100; ++i )
    {
        std::unique_ptr<wxToolBar> toolbar(
            new wxToolBar(
                parent, wxID_ANY, wxDefaultPosition, wxSize(180, 60)));
        REQUIRE(toolbar->AddTool(400 + i, "tool", bitmap));
        REQUIRE(toolbar->Realize());
        CHECK(
            wxToolBar::WinUIGetLiveCallbackStateCountForTesting() ==
            baseline + 1);
        toolbar->WinUIClosePeerForTesting();
        toolbar->WinUIClosePeerForTesting();
        toolbar.reset();
        CHECK(
            wxToolBar::WinUIGetLiveCallbackStateCountForTesting() ==
            baseline);
    }

    int lateClicks = 0;
    std::unique_ptr<wxToolBar> queued(
        new wxToolBar(
            parent, wxID_ANY, wxDefaultPosition, wxSize(180, 60)));
    REQUIRE(queued->AddTool(501, "queued", bitmap));
    REQUIRE(queued->Realize());
    queued->Bind(
        wxEVT_TOOL,
        [&lateClicks](wxCommandEvent&)
        {
            ++lateClicks;
        },
        501);
    REQUIRE(queued->WinUIQueueToolClickForTesting(501));
    queued->WinUIClosePeerForTesting();
    queued.reset();
    CHECK(
        wxToolBar::WinUIGetLiveCallbackStateCountForTesting() ==
        baseline + 1);
    DrainToolbarCallbacks();
    CHECK(lateClicks == 0);
    CHECK(
        wxToolBar::WinUIGetLiveCallbackStateCountForTesting() ==
        baseline);

    // Destruction from the synchronous wx event must stop dispatch before any
    // model/peer access after HandleWindowEvent() returns.
    wxToolBar *doomed = new wxToolBar(
        parent, wxID_ANY, wxDefaultPosition, wxSize(180, 60));
    REQUIRE(doomed->AddCheckTool(502, "doomed", bitmap));
    REQUIRE(doomed->Realize());
    wxWeakRef<wxWindow> lifetime(doomed);
    doomed->Bind(
        wxEVT_TOOL,
        [doomed](wxCommandEvent&)
        {
            doomed->Destroy();
        },
        502);
    CHECK(doomed->WinUIInvokeToolForTesting(502));
    CHECK(lifetime.get() == nullptr);
    CHECK(
        wxToolBar::WinUIGetLiveCallbackStateCountForTesting() ==
        baseline);
}

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_TOOLBAR
