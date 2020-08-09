///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/listctrltest.cpp
// Purpose:     wxListCtrl unit test
// Author:      Vadim Zeitlin
// Created:     2008-11-26
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
//              (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#if wxUSE_LISTCTRL

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/listctrl.h"
#include "wx/artprov.h"
#include "wx/imaglist.h"
#include "listbasetest.h"
#include "testableframe.h"
#include "wx/uiaction.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class ListCtrlTestCase : public ListBaseTestCase, public CppUnit::TestCase
{
public:
    ListCtrlTestCase() { }

    virtual void setUp() wxOVERRIDE;
    virtual void tearDown() wxOVERRIDE;

    virtual wxListCtrl *GetList() const wxOVERRIDE { return m_list; }

private:
    CPPUNIT_TEST_SUITE( ListCtrlTestCase );
        wxLIST_BASE_TESTS();
        CPPUNIT_TEST( EditLabel );
        CPPUNIT_TEST( SubitemRect );
    CPPUNIT_TEST_SUITE_END();

    void EditLabel();
    void SubitemRect();

    wxListCtrl *m_list;

    wxDECLARE_NO_COPY_CLASS(ListCtrlTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( ListCtrlTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ListCtrlTestCase, "ListCtrlTestCase" );

// ----------------------------------------------------------------------------
// test initialization
// ----------------------------------------------------------------------------

void ListCtrlTestCase::setUp()
{
    m_list = new wxListCtrl(wxTheApp->GetTopWindow());
    m_list->SetWindowStyle(wxLC_REPORT);
    m_list->SetSize(400, 200);
}

void ListCtrlTestCase::tearDown()
{
    DeleteTestWindow(m_list);
    m_list = NULL;
}

void ListCtrlTestCase::EditLabel()
{
    m_list->InsertColumn(0, "Column 0");
    m_list->InsertItem(0, "foo");
    m_list->EditLabel(0);
}

void ListCtrlTestCase::SubitemRect()
{
    wxBitmap bmp = wxArtProvider::GetBitmap(wxART_ERROR);

    wxImageList* const iml = new wxImageList(bmp.GetWidth(), bmp.GetHeight());
    iml->Add(bmp);
    m_list->AssignImageList(iml, wxIMAGE_LIST_SMALL);

    m_list->InsertColumn(0, "Column 0");
    m_list->InsertColumn(1, "Column 1");
    m_list->InsertColumn(2, "Column 2");
    for ( int i = 0; i < 3; i++ )
    {
        long index = m_list->InsertItem(i, wxString::Format("This is item %d", i), 0);
        m_list->SetItem(index, 1, wxString::Format("Column 1 item %d", i));
        m_list->SetItem(index, 2, wxString::Format("Column 2 item %d", i));
    }

    wxRect rectLabel, rectIcon, rectItem;

    // First check a subitem with an icon: it should have a valid icon
    // rectangle and the label rectangle should be adjacent to it.
    m_list->GetSubItemRect(1, 0, rectItem, wxLIST_RECT_BOUNDS);
    m_list->GetSubItemRect(1, 0, rectIcon, wxLIST_RECT_ICON);
    m_list->GetSubItemRect(1, 0, rectLabel, wxLIST_RECT_LABEL);

    CHECK(!rectIcon.IsEmpty());
    // Note that we can't use "==" here, in the native MSW version there is a
    // gap between the item rectangle and the icon one.
    CHECK(rectIcon.GetLeft() >= rectItem.GetLeft());
    CHECK(rectLabel.GetLeft() == rectIcon.GetRight() + 1);
    CHECK(rectLabel.GetRight() == rectItem.GetRight());

    // For a subitem without an icon, label rectangle is the same one as the
    // entire item one and the icon rectangle should be empty.
    m_list->GetSubItemRect(1, 1, rectItem, wxLIST_RECT_BOUNDS);
    m_list->GetSubItemRect(1, 1, rectIcon, wxLIST_RECT_ICON);
    m_list->GetSubItemRect(1, 1, rectLabel, wxLIST_RECT_LABEL);

    CHECK(rectIcon.IsEmpty());
    // Here we can't check for exact equality neither as there can be a margin.
    CHECK(rectLabel.GetLeft() >= rectItem.GetLeft());
    CHECK(rectLabel.GetRight() == rectItem.GetRight());
}

#endif // wxUSE_LISTCTRL
