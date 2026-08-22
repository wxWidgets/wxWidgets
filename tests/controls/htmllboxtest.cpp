///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/htmllboxtest.cpp
// Purpose:     wxSimpleHtmlListBoxNameStr unit test
// Author:      Vadim Zeitlin
// Created:     2010-11-27
// Copyright:   (c) 2010 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_HTML


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/panel.h"
#endif // WX_PRECOMP

#include "wx/filename.h"
#include "wx/filesys.h"
#include "wx/htmllbox.h"
#include "wx/html/htmlcell.h"
#include "wx/stdpaths.h"
#include "itemcontainertest.h"
#include "testableframe.h"

namespace
{

class CountingClientData final : public wxClientData
{
public:
    CountingClientData(const wxString& value, int& destroyed)
        : m_value(value),
          m_destroyed(destroyed)
    {
    }

    ~CountingClientData() override
    {
        ++m_destroyed;
    }

    const wxString& GetValue() const { return m_value; }

private:
    wxString m_value;
    int& m_destroyed;
};

class ReentrantHtmlListBox final : public wxHtmlListBox
{
public:
    explicit ReentrantHtmlListBox(wxWindow* parent)
        : wxHtmlListBox(parent, wxID_ANY,
                        wxDefaultPosition, wxSize(180, 90))
    {
    }

    void ArmClearOnMarkup() const { m_clearOnMarkup = true; }
    bool HadOutOfRangeMarkup() const { return m_outOfRangeMarkup; }

protected:
    wxString OnGetItem(size_t item) const override
    {
        if ( item >= GetItemCount() )
            m_outOfRangeMarkup = true;

        if ( m_clearOnMarkup )
        {
            m_clearOnMarkup = false;
            const_cast<ReentrantHtmlListBox *>(this)->SetItemCount(0);
        }

        return wxString::Format("<b>item %lu</b>",
                                static_cast<unsigned long>(item));
    }

private:
    mutable bool m_clearOnMarkup{false};
    mutable bool m_outOfRangeMarkup{false};
};

} // anonymous namespace

class HtmlListBoxTestCase : public ItemContainerTestCase,
                            public CppUnit::TestCase
{
public:
    HtmlListBoxTestCase() { }

    virtual void setUp() override;
    virtual void tearDown() override;

private:
    virtual wxItemContainer *GetContainer() const override { return m_htmllbox; }
    virtual wxWindow *GetContainerWindow() const override { return m_htmllbox; }

    CPPUNIT_TEST_SUITE( HtmlListBoxTestCase );
        wxITEM_CONTAINER_TESTS();
        CPPUNIT_TEST( MarkupGeometryAndMutation );
        CPPUNIT_TEST( LinkAndSelectionRouting );
        CPPUNIT_TEST( CellEventMutationIsSafe );
        CPPUNIT_TEST( ReentrantMarkupMutation );
        CPPUNIT_TEST( LocalImageAndClientDataLifetime );
        CPPUNIT_TEST( LargeLocalModelAndLifetime );
    CPPUNIT_TEST_SUITE_END();

    void MarkupGeometryAndMutation();
    void LinkAndSelectionRouting();
    void CellEventMutationIsSafe();
    void ReentrantMarkupMutation();
    void LocalImageAndClientDataLifetime();
    void LargeLocalModelAndLifetime();

    wxSimpleHtmlListBox* m_htmllbox;

    wxDECLARE_NO_COPY_CLASS(HtmlListBoxTestCase);
};

wxREGISTER_UNIT_TEST_WITH_TAGS(HtmlListBoxTestCase,
                               "[HtmlListBoxTestCase][item-container]");

void HtmlListBoxTestCase::setUp()
{
    m_htmllbox = new wxSimpleHtmlListBox(wxTheApp->GetTopWindow(), wxID_ANY);
}

void HtmlListBoxTestCase::tearDown()
{
    wxDELETE(m_htmllbox);
}

void HtmlListBoxTestCase::MarkupGeometryAndMutation()
{
    m_htmllbox->SetSize(240, 120);
    m_htmllbox->Append("<b>short</b>");
    m_htmllbox->Append("line 1<br>line 2<br>line 3");

    const wxRect first = m_htmllbox->GetItemRect(0);
    const wxRect second = m_htmllbox->GetItemRect(1);
    CPPUNIT_ASSERT( !first.IsEmpty() );
    CPPUNIT_ASSERT( !second.IsEmpty() );
    CPPUNIT_ASSERT( second.height > first.height );
    CPPUNIT_ASSERT_EQUAL(
        0,
        m_htmllbox->VirtualHitTest(first.GetTop() + first.height / 2) );
    CPPUNIT_ASSERT_EQUAL(
        1,
        m_htmllbox->VirtualHitTest(second.GetTop() + second.height / 2) );

    m_htmllbox->SetSelection(1);
    m_htmllbox->SetString(1, "<i>replacement</i>");
    CPPUNIT_ASSERT_EQUAL(
        wxString("<i>replacement</i>"),
        m_htmllbox->GetString(1) );

    m_htmllbox->Delete(0);
    CPPUNIT_ASSERT_EQUAL( 1u, m_htmllbox->GetCount() );
    CPPUNIT_ASSERT_EQUAL(
        wxString("<i>replacement</i>"),
        m_htmllbox->GetString(0) );
}

void HtmlListBoxTestCase::LinkAndSelectionRouting()
{
    m_htmllbox->SetSize(240, 120);
    m_htmllbox->Append("<a href=\"local://target\">link</a>");
    m_htmllbox->Append("plain");

    EventCounter links(m_htmllbox, wxEVT_HTML_LINK_CLICKED);
    EventCounter selections(m_htmllbox, wxEVT_LISTBOX);
    EventCounter contexts(m_htmllbox, wxEVT_CONTEXT_MENU);

    const wxRect linkRect = m_htmllbox->GetItemRect(0);
    CPPUNIT_ASSERT( !linkRect.IsEmpty() );
    wxMouseEvent linkClick(wxEVT_LEFT_DOWN);
    linkClick.m_x = linkRect.x + 5;
    linkClick.m_y = linkRect.y + linkRect.height / 2;
    m_htmllbox->GetEventHandler()->ProcessEvent(linkClick);

    CPPUNIT_ASSERT_EQUAL( 1, links.GetCount() );
    CPPUNIT_ASSERT_EQUAL( 0, selections.GetCount() );
    CPPUNIT_ASSERT_EQUAL( wxNOT_FOUND, m_htmllbox->GetSelection() );

    const wxRect plainRect = m_htmllbox->GetItemRect(1);
    CPPUNIT_ASSERT( !plainRect.IsEmpty() );
    wxMouseEvent plainClick(wxEVT_LEFT_DOWN);
    plainClick.m_x = plainRect.x + 5;
    plainClick.m_y = plainRect.y + plainRect.height / 2;
    m_htmllbox->GetEventHandler()->ProcessEvent(plainClick);

    CPPUNIT_ASSERT_EQUAL( 1, links.GetCount() );
    CPPUNIT_ASSERT_EQUAL( 1, selections.GetCount() );
    CPPUNIT_ASSERT_EQUAL( 1, m_htmllbox->GetSelection() );

    wxContextMenuEvent context(wxEVT_CONTEXT_MENU, m_htmllbox->GetId(),
                               plainRect.GetPosition());
    context.SetEventObject(m_htmllbox);
    m_htmllbox->GetEventHandler()->ProcessEvent(context);
    CPPUNIT_ASSERT_EQUAL( 1, contexts.GetCount() );
}

void HtmlListBoxTestCase::CellEventMutationIsSafe()
{
    m_htmllbox->SetSize(240, 120);
    m_htmllbox->Append("<a href=\"local://cleared\">clear me</a>");

    int cellEvents = 0;
    EventCounter links(m_htmllbox, wxEVT_HTML_LINK_CLICKED);
    EventCounter selections(m_htmllbox, wxEVT_LISTBOX);
    m_htmllbox->Bind(wxEVT_HTML_CELL_CLICKED,
                     [this, &cellEvents](wxHtmlCellEvent& event)
    {
        ++cellEvents;
        m_htmllbox->Clear();

        // Request normal HTML processing after invalidating the exact cell
        // passed in the event. The control must detect this invalidation.
        event.Skip();
    });

    const wxRect rect = m_htmllbox->GetItemRect(0);
    CPPUNIT_ASSERT( !rect.IsEmpty() );

    wxMouseEvent click(wxEVT_LEFT_DOWN);
    click.m_x = rect.x + 5;
    click.m_y = rect.y + rect.height / 2;
    m_htmllbox->GetEventHandler()->ProcessEvent(click);

    CPPUNIT_ASSERT_EQUAL( 1, cellEvents );
    CPPUNIT_ASSERT_EQUAL( 0u, m_htmllbox->GetCount() );
    CPPUNIT_ASSERT_EQUAL( 0, links.GetCount() );
    CPPUNIT_ASSERT_EQUAL( 0, selections.GetCount() );
    CPPUNIT_ASSERT_EQUAL( wxNOT_FOUND, m_htmllbox->GetSelection() );
}

void HtmlListBoxTestCase::ReentrantMarkupMutation()
{
    ReentrantHtmlListBox list(wxTheApp->GetTopWindow());
    list.SetItemCount(20);
    list.ArmClearOnMarkup();

    CPPUNIT_ASSERT( list.GetItemRect(0).IsEmpty() );
    CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(0), list.GetItemCount() );
    CPPUNIT_ASSERT( !list.HadOutOfRangeMarkup() );
}

void HtmlListBoxTestCase::LocalImageAndClientDataLifetime()
{
    wxFileName imageFile(wxStandardPaths::Get().GetExecutablePath());
    imageFile.SetFullName("horse.gif");
    CPPUNIT_ASSERT( imageFile.FileExists() );

    const wxString imageUrl = wxFileSystem::FileNameToURL(imageFile);
    m_htmllbox->SetSize(240, 120);
    m_htmllbox->Append(
        wxString::Format("<img src=\"%s\"> local image", imageUrl));

    const wxRect imageRect = m_htmllbox->GetItemRect(0);
    CPPUNIT_ASSERT( !imageRect.IsEmpty() );
    CPPUNIT_ASSERT( imageRect.height > 4 );

    m_htmllbox->Delete(0);
    wxYield();
    CPPUNIT_ASSERT_EQUAL( 0u, m_htmllbox->GetCount() );

    int destroyed = 0;
    m_htmllbox->Append(
        "zero", new CountingClientData("zero", destroyed));
    m_htmllbox->Append(
        "one", new CountingClientData("one", destroyed));
    m_htmllbox->Append(
        "two", new CountingClientData("two", destroyed));

    m_htmllbox->Delete(1);
    CPPUNIT_ASSERT_EQUAL( 1, destroyed );
    CPPUNIT_ASSERT_EQUAL(
        wxString("zero"),
        static_cast<CountingClientData *>(
            m_htmllbox->GetClientObject(0))->GetValue() );
    CPPUNIT_ASSERT_EQUAL(
        wxString("two"),
        static_cast<CountingClientData *>(
            m_htmllbox->GetClientObject(1))->GetValue() );

    m_htmllbox->Clear();
    CPPUNIT_ASSERT_EQUAL( 3, destroyed );
}

void HtmlListBoxTestCase::LargeLocalModelAndLifetime()
{
    wxArrayString items;
    items.Alloc(10000);
    for ( int i = 0; i < 10000; ++i )
        items.push_back(wxString::Format("<b>row %d</b>", i));

    m_htmllbox->Append(items);
    CPPUNIT_ASSERT_EQUAL( 10000u, m_htmllbox->GetCount() );
    CPPUNIT_ASSERT( m_htmllbox->ScrollToRow(9999) );
    CPPUNIT_ASSERT( m_htmllbox->IsRowVisible(9999) );

    wxWindow* const top = wxTheApp->GetTopWindow();
    wxPanel* const parent1 = new wxPanel(top);
    wxPanel* const parent2 = new wxPanel(top);
    wxSimpleHtmlListBox* const twoStep = new wxSimpleHtmlListBox;
    CPPUNIT_ASSERT( twoStep->Create(parent1, wxID_ANY,
                                    wxDefaultPosition, wxSize(160, 90)) );
    twoStep->Append("local");
    CPPUNIT_ASSERT( twoStep->Reparent(parent2) );
    delete twoStep;

    for ( int i = 0; i < 100; ++i )
    {
        wxSimpleHtmlListBox* const list =
            new wxSimpleHtmlListBox(parent1, wxID_ANY);
        list->Append("<b>local</b>");
        delete list;
    }

    delete parent2;
    delete parent1;
}

#endif //wxUSE_HTML
