///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/htmllboxtest.cpp
// Purpose:     wxSimpleHtmlListBoxNameStr unit test
// Author:      Vadim Zeitlin
// Created:     2010-11-27
// Copyright:   (c) 2010 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_HTML

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

    virtual void setUp() wxOVERRIDE;
    virtual void tearDown() wxOVERRIDE;

private:
    virtual wxItemContainer *GetContainer() const wxOVERRIDE { return m_htmllbox; }
    virtual wxWindow *GetContainerWindow() const wxOVERRIDE { return m_htmllbox; }

    CPPUNIT_TEST_SUITE( HtmlListBoxTestCase );
        wxITEM_CONTAINER_TESTS();
    CPPUNIT_TEST_SUITE_END();

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

#endif //wxUSE_HTML
