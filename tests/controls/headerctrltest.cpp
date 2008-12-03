///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/headerctrltest.cpp
// Purpose:     wxHeaderCtrl unit test
// Author:      Vadim Zeitlin
// Created:     2008-11-26
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifdef __WXMSW__ // no generic version of this control yet

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/headerctrl.h"
#endif // WX_PRECOMP

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class HeaderCtrlTestCase : public CppUnit::TestCase
{
public:
    HeaderCtrlTestCase() { }

    virtual void setUp();
    virtual void tearDown();

private:
    CPPUNIT_TEST_SUITE( HeaderCtrlTestCase );
        CPPUNIT_TEST( AddDelete );
        CPPUNIT_TEST( BestSize );
    CPPUNIT_TEST_SUITE_END();

    void AddDelete();
    void BestSize();

    wxHeaderCtrl *m_header;

    DECLARE_NO_COPY_CLASS(HeaderCtrlTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( HeaderCtrlTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( HeaderCtrlTestCase, "HeaderCtrlTestCase" );

// ----------------------------------------------------------------------------
// test initialization
// ----------------------------------------------------------------------------

void HeaderCtrlTestCase::setUp()
{
    m_header = new wxHeaderCtrl(wxTheApp->GetTopWindow());
}

void HeaderCtrlTestCase::tearDown()
{
    delete m_header;
    m_header = NULL;
}

// ----------------------------------------------------------------------------
// the tests themselves
// ----------------------------------------------------------------------------

void HeaderCtrlTestCase::AddDelete()
{
    CPPUNIT_ASSERT_EQUAL( 0, m_header->GetColumnCount() );

    m_header->AppendColumn(wxHeaderCtrlColumn("Column 1"));
    CPPUNIT_ASSERT_EQUAL( 1, m_header->GetColumnCount() );

    m_header->AppendColumn(wxHeaderCtrlColumn("Column 2"));
    CPPUNIT_ASSERT_EQUAL( 2, m_header->GetColumnCount() );

    m_header->InsertColumn(wxHeaderCtrlColumn("Column 0"), 0);
    CPPUNIT_ASSERT_EQUAL( 3, m_header->GetColumnCount() );

    m_header->DeleteColumn(2);
    CPPUNIT_ASSERT_EQUAL( 2, m_header->GetColumnCount() );
}

void HeaderCtrlTestCase::BestSize()
{
    const wxSize sizeEmpty = m_header->GetBestSize();
    CPPUNIT_ASSERT( sizeEmpty.x > 0 );
    CPPUNIT_ASSERT( sizeEmpty.y > 0 );

    m_header->AppendColumn(wxHeaderCtrlColumn("Foo"));
    m_header->AppendColumn(wxHeaderCtrlColumn("Bar"));
    const wxSize size = m_header->GetBestSize();
    CPPUNIT_ASSERT_EQUAL( sizeEmpty.y, size.y );
}

#endif // __WXMSW__
