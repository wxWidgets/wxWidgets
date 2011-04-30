///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/htmllboxtest.cpp
// Purpose:     wxSimpleHtmlListBoxNameStr unit test
// Author:      Vadim Zeitlin
// Created:     2010-11-27
// RCS-ID:      $Id$
// Copyright:   (c) 2010 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/htmllbox.h"
#include "itemcontainertest.h"

class HtmlListBoxTestCase : public ItemContainerTestCase,
                            public CppUnit::TestCase
{
public:
    HtmlListBoxTestCase() { }

    virtual void setUp();
    virtual void tearDown();

private:
    virtual wxItemContainer *GetContainer() const { return m_htmllbox; }
    virtual wxWindow *GetContainerWindow() const { return m_htmllbox; }

    CPPUNIT_TEST_SUITE( HtmlListBoxTestCase );
        wxITEM_CONTAINER_TESTS();
    CPPUNIT_TEST_SUITE_END();

    wxSimpleHtmlListBox* m_htmllbox;

    DECLARE_NO_COPY_CLASS(HtmlListBoxTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( HtmlListBoxTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( HtmlListBoxTestCase, "HtmlListBoxTestCase" );

void HtmlListBoxTestCase::setUp()
{
    m_htmllbox = new wxSimpleHtmlListBox(wxTheApp->GetTopWindow(), wxID_ANY);
}

void HtmlListBoxTestCase::tearDown()
{
    wxDELETE(m_htmllbox);
}
