///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/listbox.cpp
// Purpose:     wxListBox unit test
// Author:      Steven Lamerton
// Created:     2010-06-29
// RCS-ID:      $Id$
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/listbox.h"
#endif // WX_PRECOMP

#include "itemcontainertest.h"

class ListBoxTestCase : public ItemContainerTestCase
{
public:
    ListBoxTestCase() { }

    virtual void setUp();
    virtual void tearDown();

private:
    virtual wxItemContainer *GetContainer() const { return m_list; }
    virtual wxWindow *GetContainerWindow() const { return m_list; }

    CPPUNIT_TEST_SUITE( ListBoxTestCase );
        wxITEM_CONTAINER_TESTS();
    CPPUNIT_TEST_SUITE_END();

    wxListBox* m_list;

    DECLARE_NO_COPY_CLASS(ListBoxTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( ListBoxTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ListBoxTestCase, "ListBoxTestCase" );

void ListBoxTestCase::setUp()
{
    m_list = new wxListBox(wxTheApp->GetTopWindow(), wxID_ANY);
}

void ListBoxTestCase::tearDown()
{
    wxDELETE(m_list);
}

