///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/virtlistctrltest.cpp
// Purpose:     wxListCtrl unit tests for virtual mode
// Author:      Vadim Zeitlin
// Created:     2010-11-13
// RCS-ID:      $Id$
// Copyright:   (c) 2010 Vadim Zeitlin <vadim@wxwidgets.org>
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

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class VirtListCtrlTestCase : public CppUnit::TestCase
{
public:
    VirtListCtrlTestCase() { }

    virtual void setUp();
    virtual void tearDown();

private:
    CPPUNIT_TEST_SUITE( VirtListCtrlTestCase );
        CPPUNIT_TEST( UpdateSelection );
    CPPUNIT_TEST_SUITE_END();

    void UpdateSelection();

    wxListCtrl *m_list;

    wxDECLARE_NO_COPY_CLASS(VirtListCtrlTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( VirtListCtrlTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( VirtListCtrlTestCase, "VirtListCtrlTestCase" );

// ----------------------------------------------------------------------------
// test initialization
// ----------------------------------------------------------------------------

void VirtListCtrlTestCase::setUp()
{
    // Define a class overriding OnGetItemText() which must be overridden for
    // any virtual list control.
    class VirtListCtrl : public wxListCtrl
    {
    public:
        VirtListCtrl()
            : wxListCtrl(wxTheApp->GetTopWindow(), wxID_ANY,
                         wxPoint(0, 0), wxSize(400, 200),
                         wxLC_REPORT | wxLC_VIRTUAL)
        {
        }

    protected:
        virtual wxString OnGetItemText(long item, long column) const
        {
            return wxString::Format("Row %ld, col %ld", item, column);
        }
    };

    m_list = new VirtListCtrl;
}

void VirtListCtrlTestCase::tearDown()
{
    delete m_list;
    m_list = NULL;
}

void VirtListCtrlTestCase::UpdateSelection()
{
    m_list->SetItemCount(10);
    CPPUNIT_ASSERT_EQUAL( 0, m_list->GetSelectedItemCount() );

    m_list->SetItemState(7, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
    CPPUNIT_ASSERT_EQUAL( 1, m_list->GetSelectedItemCount() );

    m_list->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
    CPPUNIT_ASSERT_EQUAL( 2, m_list->GetSelectedItemCount() );

    // The item 7 is now invalid and so shouldn't be counted as selected any
    // more.
    m_list->SetItemCount(5);
    CPPUNIT_ASSERT_EQUAL( 1, m_list->GetSelectedItemCount() );
}

#endif // wxUSE_LISTCTRL
