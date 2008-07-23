///////////////////////////////////////////////////////////////////////////////
// Name:        tests/textctrl/textctrltest.cpp
// Purpose:     wxTextCtrl unit test
// Author:      Vadim Zeitlin
// Created:     2007-09-25
// RCS-ID:      $Id$
// Copyright:   (c) 2007 Vadim Zeitlin <vadim@wxwidgets.org>
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
    #include "wx/textctrl.h"
#endif // WX_PRECOMP

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class TextCtrlTestCase : public CppUnit::TestCase
{
public:
    TextCtrlTestCase() { }

    virtual void setUp();
    virtual void tearDown();

private:
    CPPUNIT_TEST_SUITE( TextCtrlTestCase );
        CPPUNIT_TEST( SetValue );
        CPPUNIT_TEST( TextChangeEvents );
        CPPUNIT_TEST( Selection );
    CPPUNIT_TEST_SUITE_END();

    void SetValue();
    void TextChangeEvents();
    void Selection();

    // Selection() test helper: verify that selection is as described by the
    // function parameters
    void AssertSelection(int from, int to, const char *sel);

    wxTextCtrl *m_text;
    wxFrame *m_frame;

    DECLARE_NO_COPY_CLASS(TextCtrlTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( TextCtrlTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( TextCtrlTestCase, "TextCtrlTestCase" );

// ----------------------------------------------------------------------------
// test initialization
// ----------------------------------------------------------------------------

void TextCtrlTestCase::setUp()
{
    m_text = new wxTextCtrl(wxTheApp->GetTopWindow(), wxID_ANY);
}

void TextCtrlTestCase::tearDown()
{
    delete m_text;
    m_text = NULL;
}

// ----------------------------------------------------------------------------
// tests themselves
// ----------------------------------------------------------------------------

void TextCtrlTestCase::SetValue()
{
    CPPUNIT_ASSERT( m_text->IsEmpty() );

    m_text->SetValue("foo");
    CPPUNIT_ASSERT_EQUAL( "foo", m_text->GetValue() );

    m_text->SetValue("");
    CPPUNIT_ASSERT( m_text->IsEmpty() );

    m_text->SetValue("hi");
    CPPUNIT_ASSERT_EQUAL( "hi", m_text->GetValue() );

    m_text->SetValue("bye");
    CPPUNIT_ASSERT_EQUAL( "bye", m_text->GetValue() );
}

void TextCtrlTestCase::TextChangeEvents()
{
    class TextTestEventHandler : public wxEvtHandler
    {
    public:
        TextTestEventHandler() { m_events = 0; }

        // calling this automatically resets the events counter
        int GetEvents()
        {
            const int events = m_events;
            m_events = 0;
            return events;
        }

        void OnText(wxCommandEvent& WXUNUSED(event)) { m_events++; }

    private:
        int m_events;
    } handler;

    m_text->Connect(wxEVT_COMMAND_TEXT_UPDATED,
                    wxCommandEventHandler(TextTestEventHandler::OnText),
                    NULL,
                    &handler);

    // notice that SetValue() generates an event even if the text didn't change
    m_text->SetValue("");
    CPPUNIT_ASSERT_EQUAL( 1, handler.GetEvents() );

    m_text->SetValue("foo");
    CPPUNIT_ASSERT_EQUAL( 1, handler.GetEvents() );

    m_text->SetValue("foo");
    CPPUNIT_ASSERT_EQUAL( 1, handler.GetEvents() );

    m_text->ChangeValue("bar");
    CPPUNIT_ASSERT_EQUAL( 0, handler.GetEvents() );

    m_text->AppendText("bar");
    CPPUNIT_ASSERT_EQUAL( 1, handler.GetEvents() );

    m_text->Replace(3, 6, "baz");
    CPPUNIT_ASSERT_EQUAL( 1, handler.GetEvents() );

    m_text->Remove(0, 3);
    CPPUNIT_ASSERT_EQUAL( 1, handler.GetEvents() );

    m_text->WriteText("foo");
    CPPUNIT_ASSERT_EQUAL( 1, handler.GetEvents() );

    m_text->Clear();
    CPPUNIT_ASSERT_EQUAL( 1, handler.GetEvents() );
}

void TextCtrlTestCase::AssertSelection(int from, int to, const char *sel)
{
    CPPUNIT_ASSERT( m_text->HasSelection() );

    long fromReal,
         toReal;
    m_text->GetSelection(&fromReal, &toReal);
    CPPUNIT_ASSERT_EQUAL( from, fromReal );
    CPPUNIT_ASSERT_EQUAL( to, toReal );
    CPPUNIT_ASSERT_EQUAL( sel, m_text->GetStringSelection() );

    CPPUNIT_ASSERT_EQUAL( from, m_text->GetInsertionPoint() );
}

void TextCtrlTestCase::Selection()
{
    m_text->SetValue("0123456789");

    m_text->SetSelection(2, 4);
    AssertSelection(2, 4, "23"); // not "234"!

    m_text->SetSelection(3, -1);
    AssertSelection(3, 10, "3456789");

    m_text->SelectAll();
    AssertSelection(0, 10, "0123456789");

    m_text->SetSelection(0, 0);
    CPPUNIT_ASSERT( !m_text->HasSelection() );
}
