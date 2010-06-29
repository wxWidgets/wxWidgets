///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/choice.cpp
// Purpose:     wxChoice unit test
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
    #include "wx/choice.h"
#endif // WX_PRECOMP

#include "itemcontainertest.h"

class ChoiceTestCase : public ItemContainerTestCase
{
public:
    ChoiceTestCase() { }

    virtual void setUp();
    virtual void tearDown();

private:
    virtual wxItemContainer *GetContainer() const { return m_choice; }
    virtual wxWindow *GetContainerWindow() const { return m_choice; }

    CPPUNIT_TEST_SUITE( ChoiceTestCase );
        wxITEM_CONTAINER_TESTS();
    CPPUNIT_TEST_SUITE_END();

    wxChoice* m_choice;

    DECLARE_NO_COPY_CLASS(ChoiceTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( ChoiceTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ChoiceTestCase, "ChoiceTestCase" );

void ChoiceTestCase::setUp()
{
    m_choice = new wxChoice(wxTheApp->GetTopWindow(), wxID_ANY);
}

void ChoiceTestCase::tearDown()
{
    wxDELETE(m_choice);
}
