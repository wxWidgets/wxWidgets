///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/auitest.cpp
// Purpose:     wxAui control tests
// Author:      Sebastian Walderich
// Created:     2018-12-19
// Copyright:   (c) 2018 Sebastian Walderich
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#if wxUSE_AUI


#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/panel.h"

#include "wx/aui/auibar.h"
#include "wx/aui/auibook.h"
#include "wx/aui/serializer.h"

#include "asserthelper.h"

#include <memory>

// ----------------------------------------------------------------------------
// test fixtures
// ----------------------------------------------------------------------------

class AuiNotebookTestCase
{
public:
    AuiNotebookTestCase()
        : nb(new wxAuiNotebook(wxTheApp->GetTopWindow()))
    {
    }

    ~AuiNotebookTestCase()
    {
        delete nb;
    }

protected:
    wxAuiNotebook* const nb;
};

// ----------------------------------------------------------------------------
// the tests themselves
// ----------------------------------------------------------------------------

TEST_CASE_METHOD(AuiNotebookTestCase, "wxAuiNotebook::DoGetBestSize", "[aui]")
{
    wxPanel *p = new wxPanel(nb);
    p->SetMinSize(wxSize(100, 100));
    REQUIRE( nb->AddPage(p, "Center Pane") );

    const int tabHeight = nb->GetTabCtrlHeight();

    SECTION( "Single pane with multiple tabs" )
    {
        p = new wxPanel(nb);
        p->SetMinSize(wxSize(300, 100));
        nb->AddPage(p, "Center Tab 2");

        p = new wxPanel(nb);
        p->SetMinSize(wxSize(100, 200));
        nb->AddPage(p, "Center Tab 3");

        CHECK( nb->GetBestSize() == wxSize(300, 200 + tabHeight) );
    }

    SECTION( "Horizontal split" )
    {
        p = new wxPanel(nb);
        p->SetMinSize(wxSize(25, 0));
        nb->AddPage(p, "Left Pane");
        nb->Split(nb->GetPageCount()-1, wxLEFT);

        CHECK( nb->GetBestSize() == wxSize(125, 100 + tabHeight) );

        p = new wxPanel(nb);
        p->SetMinSize(wxSize(50, 0));
        nb->AddPage(p, "Right Pane 1");
        nb->Split(nb->GetPageCount()-1, wxRIGHT);

        CHECK( nb->GetBestSize() == wxSize(175, 100 + tabHeight) );

        p = new wxPanel(nb);
        p->SetMinSize(wxSize(100, 0));
        nb->AddPage(p, "Right Pane 2");
        nb->Split(nb->GetPageCount()-1, wxRIGHT);

        CHECK( nb->GetBestSize() == wxSize(275, 100 + tabHeight) );
    }

    SECTION( "Vertical split" )
    {
        p = new wxPanel(nb);
        p->SetMinSize(wxSize(0, 100));
        nb->AddPage(p, "Top Pane 1");
        nb->Split(nb->GetPageCount()-1, wxTOP);

        p = new wxPanel(nb);
        p->SetMinSize(wxSize(0, 50));
        nb->AddPage(p, "Top Pane 2");
        nb->Split(nb->GetPageCount()-1, wxTOP);

        CHECK( nb->GetBestSize() == wxSize(100, 250 + 3*tabHeight) );

        p = new wxPanel(nb);
        p->SetMinSize(wxSize(0, 25));
        nb->AddPage(p, "Bottom Pane");
        nb->Split(nb->GetPageCount()-1, wxBOTTOM);

        CHECK( nb->GetBestSize() == wxSize(100, 275 + 4*tabHeight) );
    }

    SECTION( "Surrounding panes" )
    {
        p = new wxPanel(nb);
        p->SetMinSize(wxSize(50, 25));
        nb->AddPage(p, "Bottom Pane");
        nb->Split(nb->GetPageCount()-1, wxBOTTOM);

        p = new wxPanel(nb);
        p->SetMinSize(wxSize(50, 120));
        nb->AddPage(p, "Right Pane");
        nb->Split(nb->GetPageCount()-1, wxRIGHT);

        p = new wxPanel(nb);
        p->SetMinSize(wxSize(225, 50));
        nb->AddPage(p, "Top Pane");
        nb->Split(nb->GetPageCount()-1, wxTOP);

        p = new wxPanel(nb);
        p->SetMinSize(wxSize(25, 105));
        nb->AddPage(p, "Left Pane");
        nb->Split(nb->GetPageCount()-1, wxLEFT);

        CHECK( nb->GetBestSize() == wxSize(250, 175 + 3*tabHeight) );
    }
}

TEST_CASE_METHOD(AuiNotebookTestCase, "wxAuiNotebook::RTTI", "[aui][rtti]")
{
    wxBookCtrlBase* const book = nb;
    CHECK( wxDynamicCast(book, wxAuiNotebook) == nb );

    CHECK( wxDynamicCast(nb, wxBookCtrlBase) == book );
}

TEST_CASE_METHOD(AuiNotebookTestCase, "wxAuiNotebook::FindPage", "[aui]")
{
    wxPanel *p1 = new wxPanel(nb);
    wxPanel *p2 = new wxPanel(nb);
    wxPanel *p3 = new wxPanel(nb);
    REQUIRE( nb->AddPage(p1, "Page 1") );
    REQUIRE( nb->AddPage(p2, "Page 2") );

    CHECK( nb->FindPage(nullptr) == wxNOT_FOUND );
    CHECK( nb->FindPage(p1) == 0 );
    CHECK( nb->FindPage(p2) == 1 );
    CHECK( nb->FindPage(p3) == wxNOT_FOUND );
}

TEST_CASE_METHOD(AuiNotebookTestCase, "wxAuiNotebook::Layout", "[aui]")
{
    const auto addPage = [this](int n)
    {
        return nb->AddPage(new wxPanel(nb), wxString::Format("Page %d", n + 1));
    };

    for ( int n = 0; n < 5; n++ )
    {
        REQUIRE( addPage(n) );
    }

    using Ints = std::vector<int>;
    using Indices = std::vector<size_t>;

    // This serializer allows the code below to tweak its data before using it
    // as deserializer.
    class TestSerializer : public wxAuiBookSerializer,
                           public wxAuiBookDeserializer
    {
    public:
        virtual void BeforeSaveNotebook(const wxString& name) override
        {
            m_name = name;
            m_afterSaveCalled = false;
            m_tabsLayoutInfo.clear();
        }

        virtual void
        SaveNotebookTabControl(const wxAuiTabLayoutInfo& tab) override
        {
            m_tabsLayoutInfo.push_back(tab);
        }

        virtual void AfterSaveNotebook() override
        {
            m_afterSaveCalled = true;
        }

        virtual std::vector<wxAuiTabLayoutInfo>
        LoadNotebookTabs(const wxString& name) override
        {
            CHECK( name == m_name );

            m_orphanedPages.clear();

            return m_tabsLayoutInfo;
        }

        virtual bool
        HandleOrphanedPage(wxAuiNotebook& WXUNUSED(book),
                           int page,
                           wxAuiTabCtrl** WXUNUSED(tabCtrl),
                           int* tabIndex) override
        {
            m_orphanedPages.push_back(page);

            *tabIndex = m_orphanedPageReturnIndex;

            return m_orphanedPageReturnValue;
        }

        wxString m_name;
        std::vector<wxAuiTabLayoutInfo> m_tabsLayoutInfo;
        bool m_afterSaveCalled = false;

        Ints m_orphanedPages;
        bool m_orphanedPageReturnValue = true;
        int m_orphanedPageReturnIndex = wxNOT_FOUND;
    } ser;

    // Just for convenience.
    auto& info = ser.m_tabsLayoutInfo;

    // Check the default layout has expected representation.
    nb->SaveLayout("layout", ser);
    CHECK( ser.m_name == "layout" );
    CHECK( ser.m_afterSaveCalled );
    REQUIRE( info.size() == 1 );

    CHECK( info[0].pages == Ints{} );
    CHECK( info[0].pinned == Ints{} );
    CHECK( info[0].active == 0 );


    // Check that the active page is restored correctly.
    info[0].active = 1;
    nb->LoadLayout("layout", ser);

    CHECK( nb->GetSelection() == 1 );


    // Check that page order is serialized as expected.
    auto* mainTabCtrl = nb->GetMainTabCtrl();
    REQUIRE( mainTabCtrl );
    CHECK( mainTabCtrl->MovePage(1, 4) );

    nb->SaveLayout("layout", ser);
    REQUIRE( info.size() == 1 );
    CHECK( info[0].pages == Ints{0, 2, 3, 4, 1} );


    // Check that pinned pages are serialized as expected.
    REQUIRE( nb->SetPageKind(2, wxAuiTabKind::Pinned) );

    nb->SaveLayout("layout", ser);
    REQUIRE( info.size() == 1 );

    // Note that pinning a page moves it in front of all other pages.
    CHECK( info[0].pages == Ints{2, 0, 3, 4, 1} );
    CHECK( info[0].pinned == Ints{2} );


    // Check a more complicated case with both locked and pinned pages.
    REQUIRE( nb->SetPageKind(4, wxAuiTabKind::Locked) );
    REQUIRE( nb->SetPageKind(3, wxAuiTabKind::Pinned) );

    nb->SaveLayout("layout", ser);
    REQUIRE( info.size() == 1 );

    // Note that pinning a page moves it in front of all other pages.
    CHECK( info[0].pages == Ints{4, 2, 3, 0, 1} );
    CHECK( info[0].pinned == Ints{2, 3} );


    // Check that restoring existing layout after adding some pages works.
    addPage(5);
    addPage(6);
    nb->LoadLayout("layout", ser);
    CHECK( ser.m_orphanedPages == Ints{5, 6} );

    // By default, orphaned pages should have been appended.
    CHECK( nb->GetPagesInDisplayOrder(mainTabCtrl) ==
                Indices{4, 2, 3, 0, 1, 5, 6} );

    // But we can change this by telling deserializer to insert them in front.
    ser.m_orphanedPageReturnIndex = 0;

    nb->LoadLayout("layout", ser);
    CHECK( ser.m_orphanedPages == Ints{5, 6} );

    CHECK( nb->GetPagesInDisplayOrder(mainTabCtrl) ==
                Indices{6, 5, 4, 2, 3, 0, 1} );

    // Or drop them entirely.
    ser.m_orphanedPageReturnValue = false;

    nb->LoadLayout("layout", ser);
    CHECK( ser.m_orphanedPages == Ints{5, 6} );

    CHECK( nb->GetPagesInDisplayOrder(mainTabCtrl) ==
                Indices{4, 2, 3, 0, 1} );


    // Finally, check that invalid data is handled gracefully.
    info[0].active = 100;
    info[0].pages = Ints{10, 0, 1, 2, 3, 4};
    info[0].pinned = Ints{2, 99, 0};

    nb->LoadLayout("layout", ser);
    CHECK( ser.m_orphanedPages == Ints{} );

    // Locked tab should have remained first.
    CHECK( nb->GetPagesInDisplayOrder(mainTabCtrl) == Indices{4, 0, 1, 2, 3} );

    // And selection should have been set to it because the specified value was
    // invalid.
    CHECK( nb->GetSelection() == 4 );

    // And only tabs appearing before the normal ones can be pinned.
    CHECK( nb->GetPageKind(0) == wxAuiTabKind::Pinned );
    CHECK( nb->GetPageKind(1) == wxAuiTabKind::Normal );
    CHECK( nb->GetPageKind(2) == wxAuiTabKind::Normal );
    CHECK( nb->GetPageKind(3) == wxAuiTabKind::Normal );
    CHECK( nb->GetPageKind(4) == wxAuiTabKind::Locked );
}

TEST_CASE("wxAuiToolBar::Items", "[aui][toolbar]")
{
    std::unique_ptr<wxAuiToolBar> tbar{new wxAuiToolBar(wxTheApp->GetTopWindow())};

    // Check that adding more toolbar elements doesn't invalidate the existing
    // pointers.
    auto first = tbar->AddLabel(wxID_ANY, "first");
    tbar->AddLabel(wxID_ANY, "second");
    CHECK( first->GetLabel() == "first" );
}

#endif
