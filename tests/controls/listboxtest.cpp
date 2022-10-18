///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/listbox.cpp
// Purpose:     wxListBox unit test
// Author:      Steven Lamerton
// Created:     2010-06-29
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_LISTBOX


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/listbox.h"
#endif // WX_PRECOMP

#include "itemcontainertest.h"
#include "testableframe.h"
#include "wx/uiaction.h"

class ListBoxTestCase : public ItemContainerTestCase, public CppUnit::TestCase
{
public:
    ListBoxTestCase() { }

    virtual void setUp() override;
    virtual void tearDown() override;

private:
    virtual wxItemContainer *GetContainer() const override { return m_list; }
    virtual wxWindow *GetContainerWindow() const override { return m_list; }

    CPPUNIT_TEST_SUITE( ListBoxTestCase );
        wxITEM_CONTAINER_TESTS();
        CPPUNIT_TEST( Sort );
        CPPUNIT_TEST( MultipleSelect );
        WXUISIM_TEST( ClickEvents );
        WXUISIM_TEST( ClickNotOnItem );
        CPPUNIT_TEST( HitTest );
        //We also run all tests as an ownerdrawn list box.  We do not need to
        //run the wxITEM_CONTAINER_TESTS as they are tested with wxCheckListBox
#ifdef __WXMSW__
        CPPUNIT_TEST( PseudoTest_OwnerDrawn );
        CPPUNIT_TEST( Sort );
        CPPUNIT_TEST( MultipleSelect );
        WXUISIM_TEST( ClickEvents );
        WXUISIM_TEST( ClickNotOnItem );
        CPPUNIT_TEST( HitTest );
#endif
    CPPUNIT_TEST_SUITE_END();

    void Sort();
    void MultipleSelect();
    void ClickEvents();
    void ClickNotOnItem();
    void HitTest();
    void PseudoTest_OwnerDrawn() { ms_ownerdrawn = true; }

    static bool ms_ownerdrawn;

    wxListBox* m_list;

    wxDECLARE_NO_COPY_CLASS(ListBoxTestCase);
};

wxREGISTER_UNIT_TEST_WITH_TAGS(ListBoxTestCase,
                               "[ListBoxTestCase][item-container]");

//initialise the static variable
bool ListBoxTestCase::ms_ownerdrawn = false;

void ListBoxTestCase::setUp()
{
    if( ms_ownerdrawn )
    {
        m_list = new wxListBox(wxTheApp->GetTopWindow(), wxID_ANY,
                               wxDefaultPosition, wxSize(300, 200), 0, nullptr,
                               wxLB_OWNERDRAW);
    }
    else
    {
        m_list = new wxListBox(wxTheApp->GetTopWindow(), wxID_ANY,
                               wxDefaultPosition, wxSize(300, 200));
    }
}

void ListBoxTestCase::tearDown()
{
    wxDELETE(m_list);
}

void ListBoxTestCase::Sort()
{
#ifndef __WXOSX__
    wxDELETE(m_list);
    m_list = new wxListBox(wxTheApp->GetTopWindow(), wxID_ANY,
                            wxDefaultPosition, wxDefaultSize, 0, nullptr,
                            wxLB_SORT);

    wxArrayString testitems;
    testitems.Add("aaa");
    testitems.Add("Aaa");
    testitems.Add("aba");
    testitems.Add("aaab");
    testitems.Add("aab");
    testitems.Add("AAA");

    m_list->Append(testitems);

    CPPUNIT_ASSERT_EQUAL("AAA", m_list->GetString(0));
    CPPUNIT_ASSERT_EQUAL("Aaa", m_list->GetString(1));
    CPPUNIT_ASSERT_EQUAL("aaa", m_list->GetString(2));
    CPPUNIT_ASSERT_EQUAL("aaab", m_list->GetString(3));
    CPPUNIT_ASSERT_EQUAL("aab", m_list->GetString(4));
    CPPUNIT_ASSERT_EQUAL("aba", m_list->GetString(5));

    m_list->Append("a", wxUIntToPtr(1));

    CPPUNIT_ASSERT_EQUAL("a", m_list->GetString(0));
    CPPUNIT_ASSERT_EQUAL(wxUIntToPtr(1), m_list->GetClientData(0));
#endif
}

void ListBoxTestCase::MultipleSelect()
{
    wxDELETE(m_list);
    m_list = new wxListBox(wxTheApp->GetTopWindow(), wxID_ANY,
                            wxDefaultPosition, wxDefaultSize, 0, nullptr,
                            wxLB_MULTIPLE);

    wxArrayString testitems;
    testitems.Add("item 0");
    testitems.Add("item 1");
    testitems.Add("item 2");
    testitems.Add("item 3");

    m_list->Append(testitems);

    m_list->SetSelection(0);

    wxArrayInt selected;
    m_list->GetSelections(selected);

    CPPUNIT_ASSERT_EQUAL(1, selected.Count());
    CPPUNIT_ASSERT_EQUAL(0, selected.Item(0));

    m_list->SetSelection(2);

    m_list->GetSelections(selected);

    CPPUNIT_ASSERT_EQUAL(2, selected.Count());
    CPPUNIT_ASSERT_EQUAL(2, selected.Item(1));

    m_list->Deselect(0);

    m_list->GetSelections(selected);

    CPPUNIT_ASSERT_EQUAL(1, selected.Count());
    CPPUNIT_ASSERT_EQUAL(2, selected.Item(0));

    CPPUNIT_ASSERT(!m_list->IsSelected(0));
    CPPUNIT_ASSERT(!m_list->IsSelected(1));
    CPPUNIT_ASSERT(m_list->IsSelected(2));
    CPPUNIT_ASSERT(!m_list->IsSelected(3));

    m_list->SetSelection(0);
    m_list->SetSelection(wxNOT_FOUND);

    m_list->GetSelections(selected);
    CPPUNIT_ASSERT_EQUAL(0, selected.Count());
}

void ListBoxTestCase::ClickEvents()
{
#if wxUSE_UIACTIONSIMULATOR
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                              wxTestableFrame);

    EventCounter selected(frame, wxEVT_LISTBOX);
    EventCounter dclicked(frame, wxEVT_LISTBOX_DCLICK);

    wxUIActionSimulator sim;

    wxArrayString testitems;
    testitems.Add("item 0");
    testitems.Add("item 1");
    testitems.Add("item 2");

    m_list->Append(testitems);

    m_list->Update();
    m_list->Refresh();

    sim.MouseMove(m_list->ClientToScreen(wxPoint(10, 10)));
    wxYield();

    sim.MouseClick();
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, selected.GetCount());

    sim.MouseDblClick();
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, dclicked.GetCount());
#endif
}

void ListBoxTestCase::ClickNotOnItem()
{
#if wxUSE_UIACTIONSIMULATOR
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                              wxTestableFrame);

    EventCounter selected(frame, wxEVT_LISTBOX);
    EventCounter dclicked(frame, wxEVT_LISTBOX_DCLICK);

    wxUIActionSimulator sim;

    wxArrayString testitems;
    testitems.Add("item 0");
    testitems.Add("item 1");
    testitems.Add("item 2");

    m_list->Append(testitems);

    // It is important to set a valid selection: if the control doesn't have
    // any, clicking anywhere in it, even outside of any item, selects the
    // first item in the control under GTK resulting in a selection changed
    // event. This is not a wx bug, just the native platform behaviour so
    // simply avoid it by starting with a valid selection.
    m_list->SetSelection(0);

    m_list->Update();
    m_list->Refresh();

    sim.MouseMove(m_list->ClientToScreen(wxPoint(m_list->GetSize().x - 10, m_list->GetSize().y - 10)));
    wxYield();

    sim.MouseClick();
    wxYield();

    sim.MouseDblClick();
    wxYield();

    //If we are not clicking on an item we shouldn't have any events
    CPPUNIT_ASSERT_EQUAL(0, selected.GetCount());
    CPPUNIT_ASSERT_EQUAL(0, dclicked.GetCount());
#endif
}

void ListBoxTestCase::HitTest()
{
    wxArrayString testitems;
    testitems.Add("item 0");
    testitems.Add("item 1");
    testitems.Add("item 2");

    m_list->Append(testitems);

#ifdef __WXGTK__
    // The control needs to be realized for HitTest() to work.
    wxYield();
#endif

    wxPoint p(5, 5);
#ifdef __WXOSX__
    // On macOS >= 11 wxListBox has a new layout because underlying
    // NSTableView has a new style with padding so we need to move
    // the point to be tested to another position.
    if ( wxCheckOsVersion(11, 0) )
    {
        p = wxPoint(10, 10);
    }
#endif
    CPPUNIT_ASSERT_EQUAL( 0, m_list->HitTest(p) );

    CPPUNIT_ASSERT_EQUAL( wxNOT_FOUND, m_list->HitTest(290, 190) );
}

#endif //wxUSE_LISTBOX
