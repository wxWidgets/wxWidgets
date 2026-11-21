///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/buttonboxtest.cpp
// Purpose:     wxButtonBox unit test
// Author:      Wlodzimierz ABX Skiba, LLM supported
// Created:     12.11.26
// Copyright:   (c) 2026 Wlodzimierz ABX Skiba
// Licence:     wxWindows Licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_BUTTONBOX

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/panel.h"
    #include "wx/sizer.h"
#endif // WX_PRECOMP

#include "wx/artprov.h"
#include "wx/buttonbox.h"
#include "wx/uiaction.h"

#include "testableframe.h"

#include <memory>

namespace
{

// Segments have the same label length to make them roughly equally wide.
class ButtonBoxTestCase
{
public:
    ButtonBoxTestCase()
    {
        CreateBox(wxBB_SELECT_ONE);
    }

    // (Re)creates the box with the given style and 3 segments "AAA", "BBB"
    // and "CCC".
    void CreateBox(long style)
    {
        m_box.reset();
        m_box = make_unique<wxButtonBox>(wxTheApp->GetTopWindow(), wxID_ANY,
                                         wxDefaultPosition, wxDefaultSize,
                                         style);
        m_box->Append("AAA");
        m_box->Append("BBB");
        m_box->Append("CCC");

        // The size is not updated automatically when the segments are added
        // to a control which is not managed by a sizer in all ports.
        m_box->SetSize(m_box->GetBestSize());
    }

    // Clicks in the middle of the n-th segment (0, 1 or 2).
    void ClickSegment(int n)
    {
#if wxUSE_UIACTIONSIMULATOR
        const wxSize size = m_box->GetSize();

        wxUIActionSimulator sim;
        sim.MouseMove(m_box->GetScreenPosition() +
                      wxPoint(size.x * (2*n + 1) / 6, size.y / 2));
        wxYield();

        sim.MouseClick();
        wxYield();
#else
        wxUnusedVar(n);
#endif
    }

    std::unique_ptr<wxButtonBox> m_box;

    wxDECLARE_NO_COPY_CLASS(ButtonBoxTestCase);
};

// Records the last wxEVT_BUTTONBOX event.
struct LastEvent
{
    explicit LastEvent(wxButtonBox* box)
    {
        // This handler is called before the one of EventCounter created earlier
        // and as the latter doesn't skip command events, we must do it here.
        box->Bind(wxEVT_BUTTONBOX, [this](wxCommandEvent& event)
        {
            event.Skip();

            ++count;
            index = event.GetInt();
            selected = event.GetExtraLong() != 0;
            label = event.GetString();
        });
    }

    int count = 0;
    int index = wxNOT_FOUND;
    bool selected = false;
    wxString label;
};

} // anonymous namespace

TEST_CASE_METHOD(ButtonBoxTestCase, "ButtonBox::Segments", "[buttonbox]")
{
    wxButtonBox& box = *m_box;

    REQUIRE( box.GetCount() == 3 );
    CHECK( box.GetItemLabel(0) == "AAA" );
    CHECK( box.GetItemLabel(2) == "CCC" );

    box.Insert(1, "XXX");
    REQUIRE( box.GetCount() == 4 );
    CHECK( box.GetItemLabel(0) == "AAA" );
    CHECK( box.GetItemLabel(1) == "XXX" );
    CHECK( box.GetItemLabel(2) == "BBB" );
    CHECK( box.GetItemLabel(3) == "CCC" );

    box.Delete(0);
    REQUIRE( box.GetCount() == 3 );
    CHECK( box.GetItemLabel(0) == "XXX" );
    CHECK( box.GetItemLabel(2) == "CCC" );

    box.Insert(3, "END");
    REQUIRE( box.GetCount() == 4 );
    CHECK( box.GetItemLabel(3) == "END" );

    box.Clear();
    CHECK( box.GetCount() == 0 );

    box.Append("NEW");
    REQUIRE( box.GetCount() == 1 );
    CHECK( box.GetItemLabel(0) == "NEW" );
}

TEST_CASE_METHOD(ButtonBoxTestCase, "ButtonBox::Attributes", "[buttonbox]")
{
    wxButtonBox& box = *m_box;

    SECTION("Label")
    {
        box.SetItemLabel(1, "Changed");
        CHECK( box.GetItemLabel(1) == "Changed" );
        CHECK( box.GetItemLabel(0) == "AAA" );
        CHECK( box.GetItemLabel(2) == "CCC" );

        box.SetItemLabel(1, wxString());
        CHECK( box.GetItemLabel(1).empty() );
        CHECK( box.GetCount() == 3 );
    }

    SECTION("Bitmap")
    {
        CHECK( !box.GetItemBitmap(0).IsOk() );

        box.SetItemBitmap(0, wxArtProvider::GetBitmapBundle(wxART_INFORMATION,
                                                            wxART_BUTTON));
        CHECK( box.GetItemBitmap(0).IsOk() );
        CHECK( !box.GetItemBitmap(1).IsOk() );
        CHECK( box.GetItemLabel(0) == "AAA" );

        box.SetItemBitmap(0, wxBitmapBundle());
        CHECK( !box.GetItemBitmap(0).IsOk() );
    }

    SECTION("Width")
    {
        box.SetItemWidth(1, 80);
        CHECK( box.GetItemWidth(1) == 80 );

        box.SetItemWidth(1, wxDefaultCoord);
        CHECK( box.GetItemWidth(1) == wxDefaultCoord );
    }

    SECTION("Enable")
    {
        CHECK( box.IsItemEnabled(0) );

        box.EnableItem(0, false);
        CHECK( !box.IsItemEnabled(0) );
        CHECK( box.IsItemEnabled(1) );

        box.EnableItem(0);
        CHECK( box.IsItemEnabled(0) );
    }

    SECTION("ToolTip")
    {
        CHECK( box.GetItemToolTip(2).empty() );

        box.SetItemToolTip(2, "Third segment");
        CHECK( box.GetItemToolTip(2) == "Third segment" );
        CHECK( box.GetItemToolTip(1).empty() );
    }

    SECTION("Attributes follow the segment")
    {
        box.EnableItem(2, false);
        box.SetItemToolTip(2, "Last");

        box.Insert(0, "NEW");
        REQUIRE( box.GetCount() == 4 );
        CHECK( box.GetItemLabel(3) == "CCC" );
        CHECK( !box.IsItemEnabled(3) );
        CHECK( box.GetItemToolTip(3) == "Last" );
        CHECK( box.IsItemEnabled(0) );

        box.Delete(0);
        REQUIRE( box.GetCount() == 3 );
        CHECK( box.GetItemLabel(2) == "CCC" );
        CHECK( !box.IsItemEnabled(2) );
        CHECK( box.GetItemToolTip(2) == "Last" );
    }
}

TEST_CASE_METHOD(ButtonBoxTestCase, "ButtonBox::SelectOne", "[buttonbox]")
{
    wxButtonBox& box = *m_box;

    // Programmatic changes must not generate events.
    EventCounter clicked(&box, wxEVT_BUTTONBOX);

    CHECK( box.GetSelection() == wxNOT_FOUND );
    CHECK( !box.IsSelected(0) );

    box.SetSelection(1);
    CHECK( box.GetSelection() == 1 );
    CHECK( box.IsSelected(1) );
    CHECK( !box.IsSelected(0) );
    CHECK( !box.IsSelected(2) );

    box.SetSelection(2);
    CHECK( box.GetSelection() == 2 );
    CHECK( !box.IsSelected(1) );

    box.SetSelected(0, true);
    CHECK( box.GetSelection() == 0 );
    CHECK( !box.IsSelected(2) );

    box.SetSelected(0, false);
    CHECK( box.GetSelection() == wxNOT_FOUND );

    box.SetSelection(1);
    box.SetSelection(wxNOT_FOUND);
    CHECK( box.GetSelection() == wxNOT_FOUND );
    CHECK( !box.IsSelected(1) );

    CHECK( clicked.GetCount() == 0 );
}

TEST_CASE_METHOD(ButtonBoxTestCase, "ButtonBox::SelectAny", "[buttonbox]")
{
    CreateBox(wxBB_SELECT_ANY);
    wxButtonBox& box = *m_box;

    EventCounter clicked(&box, wxEVT_BUTTONBOX);

    CHECK( box.HasFlag(wxBB_SELECT_ANY) );
    CHECK( box.GetSelection() == wxNOT_FOUND );

    box.SetSelected(0, true);
    box.SetSelected(2, true);
    CHECK( box.IsSelected(0) );
    CHECK( !box.IsSelected(1) );
    CHECK( box.IsSelected(2) );

    // The exact segment is unspecified, but it must be one of the selected.
    const int sel = box.GetSelection();
    REQUIRE( sel != wxNOT_FOUND );
    CHECK( box.IsSelected(sel) );

    box.SetSelected(0, false);
    CHECK( !box.IsSelected(0) );
    CHECK( box.IsSelected(2) );

    box.SetSelection(wxNOT_FOUND);
    CHECK( !box.IsSelected(0) );
    CHECK( !box.IsSelected(1) );
    CHECK( !box.IsSelected(2) );
    CHECK( box.GetSelection() == wxNOT_FOUND );

    CHECK( clicked.GetCount() == 0 );
}

TEST_CASE_METHOD(ButtonBoxTestCase, "ButtonBox::Momentary", "[buttonbox]")
{
    CreateBox(wxBB_MOMENTARY);
    wxButtonBox& box = *m_box;

    CHECK( box.HasFlag(wxBB_MOMENTARY) );

    // Momentary segments never stay selected.
    box.SetSelected(0, true);
    box.SetSelection(1);
    CHECK( box.GetSelection() == wxNOT_FOUND );
    CHECK( !box.IsSelected(0) );
    CHECK( !box.IsSelected(1) );
}

TEST_CASE_METHOD(ButtonBoxTestCase, "ButtonBox::Size", "[buttonbox]")
{
#ifndef wxHAS_NATIVE_BUTTONBOX
    // A box created without any segments must become visible once they are
    // added, even if it is not managed by a sizer: this used to leave it with
    // the size of 0*0 forever.
    wxButtonBox* const empty = new wxButtonBox(wxTheApp->GetTopWindow(),
                                               wxID_ANY);
    empty->Append("AAA");
    empty->Append("BBB");
    CHECK( empty->GetSize().x > 0 );
    CHECK( empty->GetSize().y > 0 );
    empty->Destroy();
#endif // !wxHAS_NATIVE_BUTTONBOX

    // The same when the box is managed by a sizer and is created empty.
    wxPanel* const panel = new wxPanel(wxTheApp->GetTopWindow());
    wxBoxSizer* const sizer = new wxBoxSizer(wxVERTICAL);
    wxButtonBox* const sizedBox = new wxButtonBox(panel, wxID_ANY);
    sizer->Add(sizedBox);
    panel->SetSizer(sizer);

    sizedBox->Append("AAA");
    sizedBox->Append("BBB");
    panel->SetSize(300, 100);
    panel->Layout();

    CHECK( sizedBox->GetBestSize().x > 0 );
    CHECK( sizedBox->GetSize().x > 0 );
    CHECK( sizedBox->GetSize().y > 0 );

    // Adding more segments must make the best size bigger.
    const int widthBefore = sizedBox->GetBestSize().x;
    sizedBox->Append("CCC");
    sizedBox->Append("DDD");
    CHECK( sizedBox->GetBestSize().x > widthBefore );

    panel->Destroy();
}

TEST_CASE_METHOD(ButtonBoxTestCase, "ButtonBox::Click", "[buttonbox]")
{
#if wxUSE_UIACTIONSIMULATOR
    if ( !EnableUITests() )
        return;

    SECTION("Select one")
    {
        wxButtonBox& box = *m_box;
        EventCounter clicked(&box, wxEVT_BUTTONBOX);
        LastEvent last(&box);

        ClickSegment(0);
        CHECK( clicked.GetCount() == 1 );
        CHECK( last.index == 0 );
        CHECK( last.selected );
        CHECK( last.label == "AAA" );
        CHECK( box.GetSelection() == 0 );
        clicked.Clear();

        ClickSegment(2);
        CHECK( clicked.GetCount() == 1 );
        CHECK( last.index == 2 );
        CHECK( box.GetSelection() == 2 );
        CHECK( !box.IsSelected(0) );
        clicked.Clear();

        // Clicking the selected segment keeps it selected but still notifies.
        ClickSegment(2);
        CHECK( clicked.GetCount() == 1 );
        CHECK( last.selected );
        CHECK( box.GetSelection() == 2 );
    }

    SECTION("Select any")
    {
        CreateBox(wxBB_SELECT_ANY);
        wxButtonBox& box = *m_box;
        EventCounter clicked(&box, wxEVT_BUTTONBOX);
        LastEvent last(&box);

        ClickSegment(0);
        CHECK( clicked.GetCount() == 1 );
        CHECK( last.index == 0 );
        CHECK( last.selected );
        CHECK( box.IsSelected(0) );

        ClickSegment(2);
        CHECK( box.IsSelected(0) );
        CHECK( box.IsSelected(2) );
        clicked.Clear();

        // A second click deselects the segment.
        ClickSegment(0);
        CHECK( clicked.GetCount() == 1 );
        CHECK( last.index == 0 );
        CHECK( !last.selected );
        CHECK( !box.IsSelected(0) );
        CHECK( box.IsSelected(2) );
    }

    SECTION("Momentary")
    {
        CreateBox(wxBB_MOMENTARY);
        wxButtonBox& box = *m_box;
        EventCounter clicked(&box, wxEVT_BUTTONBOX);
        LastEvent last(&box);

        ClickSegment(1);
        CHECK( clicked.GetCount() == 1 );
        CHECK( last.index == 1 );
        CHECK( last.selected );
        CHECK( !box.IsSelected(1) );
        CHECK( box.GetSelection() == wxNOT_FOUND );
    }

    SECTION("Disabled segment")
    {
        wxButtonBox& box = *m_box;
        EventCounter clicked(&box, wxEVT_BUTTONBOX);

        box.EnableItem(0, false);

        ClickSegment(0);
        CHECK( clicked.GetCount() == 0 );
        CHECK( box.GetSelection() == wxNOT_FOUND );
    }
#endif // wxUSE_UIACTIONSIMULATOR
}

#endif // wxUSE_BUTTONBOX
