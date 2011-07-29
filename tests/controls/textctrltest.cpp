///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/textctrltest.cpp
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

#if wxUSE_TEXTCTRL

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/textctrl.h"
#endif // WX_PRECOMP

#include "wx/scopeguard.h"

#include "textentrytest.h"
#include "testableframe.h"
#include "asserthelper.h"
#include "wx/uiaction.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class TextCtrlTestCase : public TextEntryTestCase, public CppUnit::TestCase
{
public:
    TextCtrlTestCase() { }

    virtual void setUp();
    virtual void tearDown();

private:
    virtual wxTextEntry *GetTestEntry() const { return m_text; }
    virtual wxWindow *GetTestWindow() const { return m_text; }

    CPPUNIT_TEST_SUITE( TextCtrlTestCase );
        wxTEXT_ENTRY_TESTS();
        CPPUNIT_TEST( MultiLineReplace );
        WXUISIM_TEST( ReadOnly );
        WXUISIM_TEST( MaxLength );
        CPPUNIT_TEST( StreamInput );
        CPPUNIT_TEST( Redirector );
        //WXUISIM_TEST( ProcessEnter );
        WXUISIM_TEST( Url );
        CPPUNIT_TEST( Style );
        CPPUNIT_TEST( Lines );
        CPPUNIT_TEST( LogTextCtrl );
        CPPUNIT_TEST( PositionToCoords );
        CPPUNIT_TEST( PositionToCoordsRich );
        CPPUNIT_TEST( PositionToCoordsRich2 );
    CPPUNIT_TEST_SUITE_END();

    void MultiLineReplace();
    void ReadOnly();
    void MaxLength();
    void StreamInput();
    void Redirector();
    //void ProcessEnter();
    void Url();
    void Style();
    void Lines();
    void LogTextCtrl();
    void PositionToCoords();
    void PositionToCoordsRich();
    void PositionToCoordsRich2();

    void DoPositionToCoordsTestWithStyle(long style);

    wxTextCtrl *m_text;

    DECLARE_NO_COPY_CLASS(TextCtrlTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( TextCtrlTestCase );

// also include in its own registry so that these tests can be run alone
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
    wxDELETE(m_text);
}

// ----------------------------------------------------------------------------
// tests themselves
// ----------------------------------------------------------------------------

void TextCtrlTestCase::MultiLineReplace()
{
    // we need a multiline control for this test so recreate it
    delete m_text;
    m_text = new wxTextCtrl(wxTheApp->GetTopWindow(), wxID_ANY, "",
                            wxDefaultPosition, wxDefaultSize,
                            wxTE_MULTILINE);

    m_text->SetValue("Hello replace\n"
                    "0123456789012");
    m_text->SetInsertionPoint(0);

    m_text->Replace(6, 13, "changed");

    CPPUNIT_ASSERT_EQUAL("Hello changed\n"
                         "0123456789012",
                         m_text->GetValue());
    CPPUNIT_ASSERT_EQUAL(13, m_text->GetInsertionPoint());

    m_text->Replace(13, -1, "");
    CPPUNIT_ASSERT_EQUAL("Hello changed", m_text->GetValue());
    CPPUNIT_ASSERT_EQUAL(13, m_text->GetInsertionPoint());

    delete m_text;
    m_text = new wxTextCtrl(wxTheApp->GetTopWindow(), wxID_ANY);
}

void TextCtrlTestCase::ReadOnly()
{
#if wxUSE_UIACTIONSIMULATOR
    // we need a read only control for this test so recreate it
    delete m_text;
    m_text = new wxTextCtrl(wxTheApp->GetTopWindow(), wxID_ANY, "",
                            wxDefaultPosition, wxDefaultSize,
                            wxTE_READONLY);

    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_text, wxEVT_COMMAND_TEXT_UPDATED);

    m_text->SetFocus();

    wxUIActionSimulator sim;
    sim.Text("abcdef");
    wxYield();

    CPPUNIT_ASSERT_EQUAL("", m_text->GetValue());
    CPPUNIT_ASSERT_EQUAL(0, frame->GetEventCount());

    // SetEditable() is supposed to override wxTE_READONLY
    m_text->SetEditable(true);

    sim.Text("abcdef");
    wxYield();

    CPPUNIT_ASSERT_EQUAL("abcdef", m_text->GetValue());
    CPPUNIT_ASSERT_EQUAL(6, frame->GetEventCount());

    delete m_text;
    m_text = new wxTextCtrl(wxTheApp->GetTopWindow(), wxID_ANY);
#endif
}

void TextCtrlTestCase::MaxLength()
{
#if wxUSE_UIACTIONSIMULATOR
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_text, wxEVT_COMMAND_TEXT_UPDATED);
    EventCounter count1(m_text, wxEVT_COMMAND_TEXT_MAXLEN);

    m_text->SetFocus();
    m_text->SetMaxLength(10);

    wxUIActionSimulator sim;
    sim.Text("abcdef");
    wxYield();

    CPPUNIT_ASSERT_EQUAL(0, frame->GetEventCount(wxEVT_COMMAND_TEXT_MAXLEN));

    sim.Text("ghij");
    wxYield();

    CPPUNIT_ASSERT_EQUAL(0, frame->GetEventCount(wxEVT_COMMAND_TEXT_MAXLEN));
    CPPUNIT_ASSERT_EQUAL(10, frame->GetEventCount(wxEVT_COMMAND_TEXT_UPDATED));

    sim.Text("k");
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_COMMAND_TEXT_MAXLEN));
    CPPUNIT_ASSERT_EQUAL(0, frame->GetEventCount(wxEVT_COMMAND_TEXT_UPDATED));

    m_text->SetMaxLength(0);

    sim.Text("k");
    wxYield();

    CPPUNIT_ASSERT_EQUAL(0, frame->GetEventCount(wxEVT_COMMAND_TEXT_MAXLEN));
    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_COMMAND_TEXT_UPDATED));
#endif
}

void TextCtrlTestCase::StreamInput()
{
#ifndef __WXOSX__
    {
        // Ensure we use decimal point and not a comma.
        char * const locOld = setlocale(LC_NUMERIC, "C");
        wxON_BLOCK_EXIT2( setlocale, (int)LC_NUMERIC, locOld );

        *m_text << "stringinput"
                << 10
                << 1000L
                << 3.14f
                << 2.71
                << 'a'
                << L'b';
    }

    CPPUNIT_ASSERT_EQUAL("stringinput1010003.142.71ab", m_text->GetValue());

    m_text->SetValue("");

#if wxHAS_TEXT_WINDOW_STREAM

    std::ostream stream(m_text);

    // We don't test a wide character as this is not a wide stream
    stream << "stringinput"
           << 10
           << 1000L
           << 3.14f
           << 2.71
           << 'a';

    stream.flush();

    CPPUNIT_ASSERT_EQUAL("stringinput1010003.142.71a", m_text->GetValue());

#endif // wxHAS_TEXT_WINDOW_STREAM
#endif // !__WXOSX__
}

void TextCtrlTestCase::Redirector()
{
#if wxHAS_TEXT_WINDOW_STREAM && wxUSE_STD_IOSTREAM

    wxStreamToTextRedirector redirect(m_text);

    std::cout << "stringinput"
              << 10
              << 1000L
              << 3.14f
              << 2.71
              << 'a';

    CPPUNIT_ASSERT_EQUAL("stringinput1010003.142.71a", m_text->GetValue());

#endif
}

#if 0
void TextCtrlTestCase::ProcessEnter()
{
#if wxUSE_UIACTIONSIMULATOR
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_text, wxEVT_COMMAND_TEXT_ENTER);

    m_text->SetFocus();

    wxUIActionSimulator sim;
    sim.Char(WXK_RETURN);
    wxYield();

    CPPUNIT_ASSERT_EQUAL(0, frame->GetEventCount(wxEVT_COMMAND_TEXT_ENTER));

    // we need a text control with wxTE_PROCESS_ENTER for this test
    delete m_text;
    m_text = new wxTextCtrl(wxTheApp->GetTopWindow(), wxID_ANY, "",
                            wxDefaultPosition, wxDefaultSize,
                            wxTE_PROCESS_ENTER);

    m_text->SetFocus();

    sim.Char(WXK_RETURN);
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_COMMAND_TEXT_ENTER));
#endif
}
#endif

void TextCtrlTestCase::Url()
{
#if wxUSE_UIACTIONSIMULATOR && defined(__WXMSW__)
    delete m_text;
    m_text = new wxTextCtrl(wxTheApp->GetTopWindow(), wxID_ANY, "",
                            wxDefaultPosition, wxDefaultSize,
                            wxTE_MULTILINE | wxTE_RICH | wxTE_AUTO_URL);

    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_text, wxEVT_COMMAND_TEXT_URL);

    m_text->AppendText("http://www.wxwidgets.org");

    wxUIActionSimulator sim;
    sim.MouseMove(m_text->ClientToScreen(wxPoint(5, 5)));
    sim.MouseClick();
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount());
#endif
}

void TextCtrlTestCase::Style()
{
#ifndef __WXOSX__
    delete m_text;
    // We need wxTE_RICH under windows for style support
    m_text = new wxTextCtrl(wxTheApp->GetTopWindow(), wxID_ANY, "",
                            wxDefaultPosition, wxDefaultSize, wxTE_RICH);

    // Red text on a white background
    m_text->SetDefaultStyle(wxTextAttr(*wxRED, *wxWHITE));

    CPPUNIT_ASSERT_EQUAL(m_text->GetDefaultStyle().GetTextColour(), *wxRED);
    CPPUNIT_ASSERT_EQUAL(m_text->GetDefaultStyle().GetBackgroundColour(),
                         *wxWHITE);

    m_text->AppendText("red on white ");

    // Red text on a grey background
    m_text->SetDefaultStyle(wxTextAttr(wxNullColour, *wxLIGHT_GREY));

    CPPUNIT_ASSERT_EQUAL(m_text->GetDefaultStyle().GetTextColour(), *wxRED);
    CPPUNIT_ASSERT_EQUAL(m_text->GetDefaultStyle().GetBackgroundColour(),
                         *wxLIGHT_GREY);

    m_text->AppendText("red on grey ");

    // Blue text on a grey background
    m_text->SetDefaultStyle(wxTextAttr(*wxBLUE));


    CPPUNIT_ASSERT_EQUAL(m_text->GetDefaultStyle().GetTextColour(), *wxBLUE);
    CPPUNIT_ASSERT_EQUAL(m_text->GetDefaultStyle().GetBackgroundColour(),
                         *wxLIGHT_GREY);

    m_text->AppendText("blue on grey");

    // Get getting the style at a specific location
    wxTextAttr style;

    // We have to check that styles are supported
    if(m_text->GetStyle(3, style))
    {
        CPPUNIT_ASSERT_EQUAL(style.GetTextColour(), *wxRED);
        CPPUNIT_ASSERT_EQUAL(style.GetBackgroundColour(), *wxWHITE);
    }

    // And then setting the style
    if(m_text->SetStyle(15, 18, style))
    {
        m_text->GetStyle(17, style);

        CPPUNIT_ASSERT_EQUAL(style.GetTextColour(), *wxRED);
        CPPUNIT_ASSERT_EQUAL(style.GetBackgroundColour(), *wxWHITE);
    }
#endif
}

void TextCtrlTestCase::Lines()
{
#ifndef __WXOSX__
    delete m_text;
    m_text = new wxTextCtrl(wxTheApp->GetTopWindow(), wxID_ANY, "",
                            wxDefaultPosition, wxSize(400, 200), wxTE_MULTILINE | wxTE_DONTWRAP);

    m_text->SetValue("line1\nline2\nlong long line 3");
    m_text->Refresh();
    m_text->Update();

    CPPUNIT_ASSERT_EQUAL(3, m_text->GetNumberOfLines());
    CPPUNIT_ASSERT_EQUAL(5, m_text->GetLineLength(0));
    CPPUNIT_ASSERT_EQUAL("line2", m_text->GetLineText(1));
    CPPUNIT_ASSERT_EQUAL(16, m_text->GetLineLength(2));

    m_text->AppendText("\n\nMore text on line 5");

    CPPUNIT_ASSERT_EQUAL(5, m_text->GetNumberOfLines());
    CPPUNIT_ASSERT_EQUAL(0, m_text->GetLineLength(3));
    CPPUNIT_ASSERT_EQUAL("", m_text->GetLineText(3));
#endif
}

void TextCtrlTestCase::LogTextCtrl()
{
    delete m_text;
    m_text = new wxTextCtrl(wxTheApp->GetTopWindow(), wxID_ANY, "",
                            wxDefaultPosition, wxSize(400, 200),
                            wxTE_MULTILINE);

    CPPUNIT_ASSERT(m_text->IsEmpty());

    wxLogTextCtrl* logtext = new wxLogTextCtrl(m_text);

    wxLog* old = wxLog::SetActiveTarget(logtext);

    logtext->LogText("text");

    delete wxLog::SetActiveTarget(old);

    CPPUNIT_ASSERT(!m_text->IsEmpty());
}

void TextCtrlTestCase::PositionToCoords()
{
    DoPositionToCoordsTestWithStyle(0);
}

void TextCtrlTestCase::PositionToCoordsRich()
{
    DoPositionToCoordsTestWithStyle(wxTE_RICH);
}

void TextCtrlTestCase::PositionToCoordsRich2()
{
    DoPositionToCoordsTestWithStyle(wxTE_RICH2);
}

void TextCtrlTestCase::DoPositionToCoordsTestWithStyle(long style)
{
    static const int TEXT_HEIGHT = 200;

    delete m_text;
    m_text = new wxTextCtrl(wxTheApp->GetTopWindow(), wxID_ANY, "",
                            wxDefaultPosition, wxSize(400, TEXT_HEIGHT),
                            wxTE_MULTILINE | style);

    // Asking for invalid index should fail.
    WX_ASSERT_FAILS_WITH_ASSERT( m_text->PositionToCoords(1) );

    // Getting position shouldn't return wxDefaultPosition except if the method
    // is not implemented at all in the current port.
    const wxPoint pos0 = m_text->PositionToCoords(0);
    if ( pos0 == wxDefaultPosition )
    {
#if defined(__WXMSW__) || defined(__WXGTK20__)
        CPPUNIT_FAIL( "PositionToCoords() unexpectedly failed." );
#endif
        return;
    }

    CPPUNIT_ASSERT(pos0.x >= 0);
    CPPUNIT_ASSERT(pos0.y >= 0);


    m_text->SetValue("Hello");
    wxYield(); // Let GTK layout the control correctly.

    // Position of non-first character should be positive.
    const long posHello4 = m_text->PositionToCoords(4).x;
    CPPUNIT_ASSERT( posHello4 > 0 );

    // Asking for position beyond the last character should succeed and return
    // reasonable result.
    CPPUNIT_ASSERT( m_text->PositionToCoords(5).x > posHello4 );

    // But asking for the next position should fail.
    WX_ASSERT_FAILS_WITH_ASSERT( m_text->PositionToCoords(6) );

    // Test getting the coordinates of the last character when it is in the
    // beginning of a new line to exercise MSW code which has specific logic
    // for it.
    m_text->AppendText("\n");
    const wxPoint posLast = m_text->PositionToCoords(m_text->GetLastPosition());
    CPPUNIT_ASSERT_EQUAL( pos0.x, posLast.x );
    CPPUNIT_ASSERT( posLast.y > 0 );


    // Add enough contents to the control to make sure it has a scrollbar.
    m_text->SetValue("First line" + wxString(50, '\n') + "Last line");
    m_text->SetInsertionPoint(0);
    wxYield(); // Let GTK layout the control correctly.

    // This shouldn't change anything for the first position coordinates.
    CPPUNIT_ASSERT_EQUAL( pos0, m_text->PositionToCoords(0) );

    // And the last one must be beyond the window boundary and so not be
    // visible -- but getting its coordinate should still work.
    CPPUNIT_ASSERT
    (
        m_text->PositionToCoords(m_text->GetLastPosition()).y > TEXT_HEIGHT
    );


    // Now make it scroll to the end and check that the first position now has
    // negative offset as its above the visible part of the window while the
    // last position is in its bounds.
    m_text->SetInsertionPointEnd();

    CPPUNIT_ASSERT( m_text->PositionToCoords(0).y < 0 );
    CPPUNIT_ASSERT
    (
        m_text->PositionToCoords(m_text->GetInsertionPoint()).y <= TEXT_HEIGHT
    );
}


#endif //wxUSE_TEXTCTRL
