///////////////////////////////////////////////////////////////////////////////
// Name:        tests/window/setsize.cpp
// Purpose:     Tests for SetSize() and related wxWindow methods
// Author:      Vadim Zeitlin
// Created:     2008-05-25
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

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/window.h"
#endif // WX_PRECOMP

#include "asserthelper.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class SetSizeTestCase : public CppUnit::TestCase
{
public:
    SetSizeTestCase() { }

    virtual void setUp();
    virtual void tearDown();

private:
    CPPUNIT_TEST_SUITE( SetSizeTestCase );
        CPPUNIT_TEST( SetSize );
        CPPUNIT_TEST( SetSizeLessThanMinSize );
    CPPUNIT_TEST_SUITE_END();

    void SetSize();
    void SetSizeLessThanMinSize();

    wxWindow *m_win;

    DECLARE_NO_COPY_CLASS(SetSizeTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( SetSizeTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( SetSizeTestCase, "SetSizeTestCase" );

// ----------------------------------------------------------------------------
// test initialization
// ----------------------------------------------------------------------------

void SetSizeTestCase::setUp()
{
    m_win = new wxWindow(wxTheApp->GetTopWindow(), wxID_ANY);
}

void SetSizeTestCase::tearDown()
{
    delete m_win;
    m_win = NULL;
}

// ----------------------------------------------------------------------------
// tests themselves
// ----------------------------------------------------------------------------

void SetSizeTestCase::SetSize()
{
    const wxSize size(127, 35);
    m_win->SetSize(size);
    CPPUNIT_ASSERT_EQUAL( size, m_win->GetSize() );
}

void SetSizeTestCase::SetSizeLessThanMinSize()
{
    m_win->SetMinSize(wxSize(100, 100));

    const wxSize size(200, 50);
    m_win->SetSize(size);
    CPPUNIT_ASSERT_EQUAL( size, m_win->GetSize() );
}

