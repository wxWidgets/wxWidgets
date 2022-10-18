///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/textctrltest.cpp
// Purpose:     wxTextCtrl unit test
// Author:      Vadim Zeitlin
// Created:     2007-09-25
// Copyright:   (c) 2007 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#if wxUSE_TEXTCTRL


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/textctrl.h"
#endif // WX_PRECOMP

#include "wx/platinfo.h"
#include "wx/scopedptr.h"
#include "wx/uiaction.h"

#if wxUSE_CLIPBOARD
    #include "wx/clipbrd.h"
    #include "wx/dataobj.h"
#endif // wxUSE_CLIPBOARD

#ifdef __WXGTK__
    #include "wx/stopwatch.h"
#endif

#include "wx/private/localeset.h"

#include "textentrytest.h"
#include "testableframe.h"
#include "asserthelper.h"

static const int TEXT_HEIGHT = 200;

#if defined(__WXMSW__) && !defined(__WXUNIVERSAL__)
#define wxHAS_2CHAR_NEWLINES 1
#else
#define wxHAS_2CHAR_NEWLINES 0
#endif

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class TextCtrlTestCase : public TextEntryTestCase, public CppUnit::TestCase
{
public:
    TextCtrlTestCase() { }

    virtual void setUp() override;
    virtual void tearDown() override;

private:
    virtual wxTextEntry *GetTestEntry() const override { return m_text; }
    virtual wxWindow *GetTestWindow() const override { return m_text; }

    #define SINGLE_AND_MULTI_TESTS() \
        WXUISIM_TEST( ReadOnly ); \
        CPPUNIT_TEST( StreamInput ); \
        CPPUNIT_TEST( Redirector )

    CPPUNIT_TEST_SUITE( TextCtrlTestCase );
        // These tests run for single line text controls.
        wxTEXT_ENTRY_TESTS();
        WXUISIM_TEST( MaxLength );
        CPPUNIT_TEST( PositionToXYSingleLine );
        CPPUNIT_TEST( XYToPositionSingleLine );
        CPPUNIT_TEST( HitTestSingleLine );
        SINGLE_AND_MULTI_TESTS();

        // Now switch to the multi-line text controls.
        CPPUNIT_TEST( PseudoTestSwitchToMultiLineStyle );

        // Rerun the text entry tests not specific to single line controls for
        // multiline ones now.
        wxTEXT_ENTRY_TESTS();
        SINGLE_AND_MULTI_TESTS();


        // All tests from now on are for multi-line controls only.
        CPPUNIT_TEST( MultiLineReplace );
        //WXUISIM_TEST( ProcessEnter );
        WXUISIM_TEST( Url );
        CPPUNIT_TEST( Style );
        CPPUNIT_TEST( FontStyle );
        CPPUNIT_TEST( Lines );
#if wxUSE_LOG
        CPPUNIT_TEST( LogTextCtrl );
#endif // wxUSE_LOG
        CPPUNIT_TEST( LongText );
        CPPUNIT_TEST( PositionToCoords );
        CPPUNIT_TEST( PositionToCoordsRich );
        CPPUNIT_TEST( PositionToCoordsRich2 );
        CPPUNIT_TEST( PositionToXYMultiLine );
        CPPUNIT_TEST( XYToPositionMultiLine );
#if wxUSE_RICHEDIT
        CPPUNIT_TEST( PositionToXYMultiLineRich );
        CPPUNIT_TEST( XYToPositionMultiLineRich );
        CPPUNIT_TEST( PositionToXYMultiLineRich2 );
        CPPUNIT_TEST( XYToPositionMultiLineRich2 );
#endif // wxUSE_RICHEDIT
    CPPUNIT_TEST_SUITE_END();

    void PseudoTestSwitchToMultiLineStyle()
    {
        ms_style = wxTE_MULTILINE;
    }

    void MultiLineReplace();
    void ReadOnly();
    void MaxLength();
    void StreamInput();
    void Redirector();
    void HitTestSingleLine();
    //void ProcessEnter();
    void Url();
    void Style();
    void FontStyle();
    void Lines();
#if wxUSE_LOG
    void LogTextCtrl();
#endif // wxUSE_LOG
    void LongText();
    void PositionToCoords();
    void PositionToCoordsRich();
    void PositionToCoordsRich2();
    void PositionToXYMultiLine();
    void XYToPositionMultiLine();
#if wxUSE_RICHEDIT
    void PositionToXYMultiLineRich();
    void XYToPositionMultiLineRich();
    void PositionToXYMultiLineRich2();
    void XYToPositionMultiLineRich2();
#endif // wxUSE_RICHEDIT
    void PositionToXYSingleLine();
    void XYToPositionSingleLine();

    void DoPositionToCoordsTestWithStyle(long style);
    void DoPositionToXYMultiLine(long style);
    void DoXYToPositionMultiLine(long style);

    // Create the control with the following styles added to ms_style which may
    // (or not) already contain wxTE_MULTILINE.
    void CreateText(long extraStyles);

    wxTextCtrl *m_text;

    static long ms_style;

    wxDECLARE_NO_COPY_CLASS(TextCtrlTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( TextCtrlTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( TextCtrlTestCase, "TextCtrlTestCase" );

// ----------------------------------------------------------------------------
// test initialization
// ----------------------------------------------------------------------------

// This is 0 initially and set to wxTE_MULTILINE later to allow running the
// same tests for both single and multi line controls.
long TextCtrlTestCase::ms_style = 0;

void TextCtrlTestCase::CreateText(long extraStyles)
{
    const long style = ms_style | extraStyles;
    const int h = (style & wxTE_MULTILINE) ? TEXT_HEIGHT : -1;
    m_text = new wxTextCtrl(wxTheApp->GetTopWindow(), wxID_ANY, "",
                            wxDefaultPosition, wxSize(400, h),
                            style);
}

void TextCtrlTestCase::setUp()
{
    CreateText(0);
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
}

void TextCtrlTestCase::ReadOnly()
{
#if wxUSE_UIACTIONSIMULATOR
    // we need a read only control for this test so recreate it
    delete m_text;
    CreateText(wxTE_READONLY);

    EventCounter updated(m_text, wxEVT_TEXT);

    m_text->SetFocus();

    wxUIActionSimulator sim;
    sim.Text("abcdef");
    wxYield();

    CPPUNIT_ASSERT_EQUAL("", m_text->GetValue());
    CPPUNIT_ASSERT_EQUAL(0, updated.GetCount());

    // SetEditable() is supposed to override wxTE_READONLY
    m_text->SetEditable(true);

#if defined(__WXOSX__) || defined(__WXUNIVERSAL__)
    // a ready only text field might not have been focusable at all
    m_text->SetFocus();
#endif

    sim.Text("abcdef");
    wxYield();

    CPPUNIT_ASSERT_EQUAL("abcdef", m_text->GetValue());
    CPPUNIT_ASSERT_EQUAL(6, updated.GetCount());
#endif
}

void TextCtrlTestCase::MaxLength()
{
#if wxUSE_UIACTIONSIMULATOR
    EventCounter updated(m_text, wxEVT_TEXT);
    EventCounter maxlen(m_text, wxEVT_TEXT_MAXLEN);

    m_text->SetFocus();
    wxYield();
    m_text->SetMaxLength(10);

    wxUIActionSimulator sim;
    sim.Text("abcdef");
    wxYield();

    CPPUNIT_ASSERT_EQUAL(0, maxlen.GetCount());

    sim.Text("ghij");
    wxYield();

    CPPUNIT_ASSERT_EQUAL(0, maxlen.GetCount());
    CPPUNIT_ASSERT_EQUAL(10, updated.GetCount());

    maxlen.Clear();
    updated.Clear();

    sim.Text("k");
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, maxlen.GetCount());
    CPPUNIT_ASSERT_EQUAL(0, updated.GetCount());

    maxlen.Clear();
    updated.Clear();

    m_text->SetMaxLength(0);

    sim.Text("k");
    wxYield();

    CPPUNIT_ASSERT_EQUAL(0, maxlen.GetCount());
    CPPUNIT_ASSERT_EQUAL(1, updated.GetCount());
#endif
}

void TextCtrlTestCase::StreamInput()
{
#ifndef __WXOSX__
    {
        // Ensure we use decimal point and not a comma.
        wxCLocaleSetter setCLocale;

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

void TextCtrlTestCase::HitTestSingleLine()
{
#ifdef __WXQT__
    WARN("Does not work under WxQt");
#else
    m_text->ChangeValue("Hit me");

    // We don't know the size of the text borders, so we can't really do any
    // exact tests, just try to verify that the results are roughly as
    // expected.
    const wxSize sizeChar = m_text->GetTextExtent("X");
    const int yMid = sizeChar.y / 2;

    long pos = -1;

#ifdef __WXGTK__
    wxYield();
#endif

    // Hitting a point near the left side of the control should find one of the
    // first few characters under it.
    SECTION("Normal")
    {
        REQUIRE( m_text->HitTest(wxPoint(2*sizeChar.x, yMid), &pos) == wxTE_HT_ON_TEXT );
        CHECK( pos >= 0 );
        CHECK( pos < 3 );
    }

    // Hitting a point well beyond the end of the text shouldn't find any valid
    // character.
    SECTION("Beyond")
    {
        REQUIRE( m_text->HitTest(wxPoint(20*sizeChar.x, yMid), &pos) == wxTE_HT_BEYOND );
        CHECK( pos == m_text->GetLastPosition() );
    }

    // Making the control scroll, by ensuring that its contents is too long to
    // show inside its window, should change the hit test result for the same
    // position as used above.
    SECTION("Scrolled")
    {
        m_text->ChangeValue(wxString(200, 'X'));
        m_text->SetInsertionPointEnd();

    #ifdef __WXGTK__
        // wxGTK must be given an opportunity to lay the text out.
        for ( wxStopWatch sw; sw.Time() < 50; )
            wxYield();
    #endif

        REQUIRE( m_text->HitTest(wxPoint(2*sizeChar.x, yMid), &pos) == wxTE_HT_ON_TEXT );
        CHECK( pos > 3 );

        // Using negative coordinates works even under Xvfb, so test at least
        // for this -- however this only works in wxGTK, not wxMSW.
#ifdef __WXGTK__
        REQUIRE( m_text->HitTest(wxPoint(-2*sizeChar.x, yMid), &pos) == wxTE_HT_ON_TEXT );
        CHECK( pos > 3 );
#endif // __WXGTK__
    }
#endif
}

#if 0
void TextCtrlTestCase::ProcessEnter()
{
#if wxUSE_UIACTIONSIMULATOR
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_text, wxEVT_TEXT_ENTER);

    m_text->SetFocus();

    wxUIActionSimulator sim;
    sim.Char(WXK_RETURN);
    wxYield();

    CPPUNIT_ASSERT_EQUAL(0, frame->GetEventCount(wxEVT_TEXT_ENTER));

    // we need a text control with wxTE_PROCESS_ENTER for this test
    delete m_text;
    CreateText(wxTE_PROCESS_ENTER);

    m_text->SetFocus();

    sim.Char(WXK_RETURN);
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_TEXT_ENTER));
#endif
}
#endif

void TextCtrlTestCase::Url()
{
#if wxUSE_UIACTIONSIMULATOR && defined(__WXMSW__) && !defined(__WXUNIVERSAL__)
    // For some reason, this test sporadically fails when run in AppVeyor or
    // GitHub Actions CI environments, even though it passes locally.
    if ( IsAutomaticTest() )
        return;

    delete m_text;
    CreateText(wxTE_RICH | wxTE_AUTO_URL);

    EventCounter url(m_text, wxEVT_TEXT_URL);

    m_text->AppendText("http://www.wxwidgets.org");

    wxUIActionSimulator sim;
    sim.MouseMove(m_text->ClientToScreen(wxPoint(5, 5)));
    sim.MouseClick();
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, url.GetCount());
#endif
}

void TextCtrlTestCase::Style()
{
#if !defined(__WXOSX__) && !defined(__WXQT__)
    delete m_text;
    // We need wxTE_RICH under windows for style support
    CreateText(wxTE_MULTILINE|wxTE_RICH);

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
    if ( !m_text->GetStyle(3, style) )
    {
        WARN("Retrieving text style not supported, skipping test.");
        return;
    }

    CHECK( style.GetTextColour() == *wxRED );
    CHECK( style.GetBackgroundColour() == *wxWHITE );

    // And then setting the style
    REQUIRE( m_text->SetStyle(15, 18, style) );

    REQUIRE( m_text->GetStyle(17, style) );
    CHECK( style.GetTextColour() == *wxRED );
    CHECK( style.GetBackgroundColour() == *wxWHITE );
#else
    WARN("Does not work under WxQt or OSX");
#endif
}

void TextCtrlTestCase::FontStyle()
{
    // We need wxTE_RICH under MSW and wxTE_MULTILINE under GTK for style
    // support so recreate the control with these styles.
    delete m_text;
    CreateText(wxTE_RICH);

    // Check that we get back the same font from GetStyle() after setting it
    // with SetDefaultStyle().
    wxFont fontIn(14,
                  wxFONTFAMILY_DEFAULT,
                  wxFONTSTYLE_NORMAL,
                  wxFONTWEIGHT_NORMAL);
    wxTextAttr attrIn;
    attrIn.SetFont(fontIn);
    if ( !m_text->SetDefaultStyle(attrIn) )
    {
        // Skip the test if the styles are not supported.
        return;
    }

    m_text->AppendText("Default font size 14");

    wxTextAttr attrOut;
    if ( !m_text->GetStyle(5, attrOut) )
    {
        WARN("Retrieving text style not supported, skipping test.");
        return;
    }

    CPPUNIT_ASSERT( attrOut.HasFont() );

    wxFont fontOut = attrOut.GetFont();
#ifdef __WXMSW__
    // Under MSW we get back an encoding in the font even though we hadn't
    // specified it originally. It's not really a problem but we need this hack
    // to prevent the assert below from failing because of it.
    fontOut.SetEncoding(fontIn.GetEncoding());
#endif
    CPPUNIT_ASSERT_EQUAL( fontIn, fontOut );


    // Also check the same for SetStyle().
    fontIn.SetPointSize(10);
    fontIn.SetWeight(wxFONTWEIGHT_BOLD);
    attrIn.SetFont(fontIn);
    m_text->SetStyle(0, 6, attrIn);

    m_text->GetStyle(4, attrOut);
    CPPUNIT_ASSERT( attrOut.HasFont() );

    fontOut = attrOut.GetFont();
#ifdef __WXMSW__
    fontOut.SetEncoding(fontIn.GetEncoding());
#endif
    CPPUNIT_ASSERT_EQUAL( fontIn, fontOut );
}

void TextCtrlTestCase::Lines()
{
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

    // Verify that wrapped lines count as (at least) lines (but it can be more
    // if it's wrapped more than once).
    //
    // This currently works neither in wxGTK, wxUniv, nor wxOSX/Cocoa, see
    // #12366, where GetNumberOfLines() always returns the number of logical,
    // not physical, lines.
    m_text->AppendText("\n" + wxString(50, '1') + ' ' + wxString(50, '2'));
#if defined(__WXGTK__) || defined(__WXOSX_COCOA__) || defined(__WXUNIVERSAL__) || defined(__WXQT__)
    CPPUNIT_ASSERT_EQUAL(6, m_text->GetNumberOfLines());
#else
    CPPUNIT_ASSERT(m_text->GetNumberOfLines() > 6);
#endif
}

#if wxUSE_LOG
void TextCtrlTestCase::LogTextCtrl()
{
    CPPUNIT_ASSERT(m_text->IsEmpty());

    wxLogTextCtrl* logtext = new wxLogTextCtrl(m_text);

    wxLog* old = wxLog::SetActiveTarget(logtext);

    logtext->LogText("text");

    delete wxLog::SetActiveTarget(old);

    CPPUNIT_ASSERT(!m_text->IsEmpty());
}
#endif // wxUSE_LOG

void TextCtrlTestCase::LongText()
{
    // This test is only possible under MSW as in the other ports
    // SetMaxLength() can't be used with multi line text controls.
#ifdef __WXMSW__
    delete m_text;
    CreateText(wxTE_MULTILINE|wxTE_DONTWRAP);

    const int numLines = 1000;
    const int lenPattern = 100;
    int i;

    // Pattern for the line.
    wxChar linePattern[lenPattern+1];
    for (i = 0; i < lenPattern - 1; i++)
    {
        linePattern[i] = wxChar('0' + i % 10);
    }
    linePattern[WXSIZEOF(linePattern) - 1] = wxChar('\0');

    // Fill the control.
    m_text->SetMaxLength(15000);
    for (i = 0; i < numLines; i++)
    {
        m_text->AppendText(wxString::Format(wxT("[%3d] %s\n"), i, linePattern));
    }

    // Check the content.
    for (i = 0; i < numLines; i++)
    {
        wxString pattern = wxString::Format(wxT("[%3d] %s"), i, linePattern);
        wxString line = m_text->GetLineText(i);
        CPPUNIT_ASSERT_EQUAL( line, pattern );
    }
#endif // __WXMSW__
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
    delete m_text;
    CreateText(style|wxTE_MULTILINE);

    // Asking for invalid index should fail.
    WX_ASSERT_FAILS_WITH_ASSERT( m_text->PositionToCoords(1) );

    // Getting position shouldn't return wxDefaultPosition except if the method
    // is not implemented at all in the current port.
    const wxPoint pos0 = m_text->PositionToCoords(0);
    if ( pos0 == wxDefaultPosition )
    {
#if ( wxHAS_2CHAR_NEWLINES ) || defined(__WXGTK__)
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

    const int pos = m_text->GetInsertionPoint();

    // wxGTK needs to yield here to update the text control.
#ifdef __WXGTK__
    wxStopWatch sw;
    while ( m_text->PositionToCoords(0).y == 0 ||
                m_text->PositionToCoords(pos).y > TEXT_HEIGHT )
    {
        if ( sw.Time() > 1000 )
        {
            FAIL("Timed out waiting for wxTextCtrl update.");
            break;
        }

        wxYield();
    }
#endif // __WXGTK__

    wxPoint coords = m_text->PositionToCoords(0);
    INFO("First position coords = " << coords);
    CPPUNIT_ASSERT( coords.y < 0 );

    coords = m_text->PositionToCoords(pos);
    INFO("Position is " << pos << ", coords = " << coords);
    CPPUNIT_ASSERT( coords.y <= TEXT_HEIGHT );
}

void TextCtrlTestCase::PositionToXYMultiLine()
{
    DoPositionToXYMultiLine(0);
}

#if wxUSE_RICHEDIT
void TextCtrlTestCase::PositionToXYMultiLineRich()
{
    DoPositionToXYMultiLine(wxTE_RICH);
}

void TextCtrlTestCase::PositionToXYMultiLineRich2()
{
    DoPositionToXYMultiLine(wxTE_RICH2);
}
#endif // wxUSE_RICHEDIT

void TextCtrlTestCase::DoPositionToXYMultiLine(long style)
{
    delete m_text;
    CreateText(style|wxTE_MULTILINE|wxTE_DONTWRAP);

#if wxHAS_2CHAR_NEWLINES
    const bool isRichEdit = (style & (wxTE_RICH | wxTE_RICH2)) != 0;
#endif

    typedef struct { long x, y; } XYPos;
    bool ok;
    wxString text;

    // empty field
    m_text->Clear();
    const long numChars_0 = 0;
    wxASSERT(numChars_0 == text.length());
    XYPos coords_0[numChars_0+1] =
        { { 0, 0 } };

    CPPUNIT_ASSERT_EQUAL( numChars_0, m_text->GetLastPosition() );
    for ( long i = 0; i < (long)WXSIZEOF(coords_0); i++ )
    {
        long x, y;
        ok = m_text->PositionToXY(i, &x, &y);
        CPPUNIT_ASSERT_EQUAL( true, ok );
        CPPUNIT_ASSERT_EQUAL( coords_0[i].x, x );
        CPPUNIT_ASSERT_EQUAL( coords_0[i].y, y );
    }
    ok = m_text->PositionToXY(WXSIZEOF(coords_0), nullptr, nullptr);
    CPPUNIT_ASSERT_EQUAL( false, ok );

    // one line
    text = wxS("1234");
    m_text->SetValue(text);
    const long numChars_1 = 4;
    wxASSERT( numChars_1 == text.length() );
    XYPos coords_1[numChars_1+1] =
        { { 0, 0 }, { 1, 0 }, { 2, 0}, { 3, 0 }, { 4, 0 } };

    CPPUNIT_ASSERT_EQUAL( numChars_1, m_text->GetLastPosition() );
    for ( long i = 0; i < (long)WXSIZEOF(coords_1); i++ )
    {
        long x, y;
        ok = m_text->PositionToXY(i, &x, &y);
        CPPUNIT_ASSERT_EQUAL( true, ok );
        CPPUNIT_ASSERT_EQUAL( coords_1[i].x, x );
        CPPUNIT_ASSERT_EQUAL( coords_1[i].y, y );
    }
    ok = m_text->PositionToXY(WXSIZEOF(coords_1), nullptr, nullptr);
    CPPUNIT_ASSERT_EQUAL( false, ok );

    // few lines
    text = wxS("123\nab\nX");
    m_text->SetValue(text);

#if wxHAS_2CHAR_NEWLINES
    // Take into account that every new line mark occupies
    // two characters, not one.
    const long numChars_msw_2 = 8 + 2;
    // Note: Two new line characters refer to the same X-Y position.
    XYPos coords_2_msw[numChars_msw_2 + 1] =
        { { 0, 0 },{ 1, 0 },{ 2, 0 },{ 3, 0 },{ 3, 0 },
          { 0, 1 },{ 1, 1 },{ 2, 1 },{ 2, 1 },
          { 0, 2 },{ 1, 2 } };
#endif // WXMSW

    const long numChars_2 = 8;
    wxASSERT(numChars_2 == text.length());
    XYPos coords_2[numChars_2 + 1] =
        { { 0, 0 }, { 1, 0 }, { 2, 0 }, { 3, 0 },
          { 0, 1 }, { 1, 1 }, { 2, 1 },
          { 0, 2 }, { 1, 2 } };

    const long &ref_numChars_2 =
#if wxHAS_2CHAR_NEWLINES
        isRichEdit ? numChars_2 : numChars_msw_2;
#else
        numChars_2;
#endif

    XYPos *ref_coords_2 =
#if wxHAS_2CHAR_NEWLINES
        isRichEdit ? coords_2 : coords_2_msw;
#else
        coords_2;
#endif

    CPPUNIT_ASSERT_EQUAL( ref_numChars_2, m_text->GetLastPosition() );
    for ( long i = 0; i < ref_numChars_2+1; i++ )
    {
        long x, y;
        ok = m_text->PositionToXY(i, &x, &y);
        CPPUNIT_ASSERT_EQUAL( true, ok );
        CPPUNIT_ASSERT_EQUAL( ref_coords_2[i].x, x );
        CPPUNIT_ASSERT_EQUAL( ref_coords_2[i].y, y );
    }
    ok = m_text->PositionToXY(ref_numChars_2 + 1, nullptr, nullptr);
    CPPUNIT_ASSERT_EQUAL( false, ok );

    // only empty lines
    text = wxS("\n\n\n");
    m_text->SetValue(text);

#if wxHAS_2CHAR_NEWLINES
    // Take into account that every new line mark occupies
    // two characters, not one.
    const long numChars_msw_3 = 3 + 3;
    // Note: Two new line characters refer to the same X-Y position.
    XYPos coords_3_msw[numChars_msw_3 + 1] =
        { { 0, 0 },{ 0, 0 },
          { 0, 1 },{ 0, 1 },
          { 0, 2 },{ 0, 2 },
          { 0, 3 } };
#endif // WXMSW

    const long numChars_3 = 3;
    wxASSERT(numChars_3 == text.length());
    XYPos coords_3[numChars_3+1] =
        { { 0, 0 },
          { 0, 1 },
          { 0, 2 },
          { 0, 3 } };

    const long &ref_numChars_3 =
#if wxHAS_2CHAR_NEWLINES
        isRichEdit ? numChars_3 : numChars_msw_3;
#else
        numChars_3;
#endif

    XYPos *ref_coords_3 =
#if wxHAS_2CHAR_NEWLINES
        isRichEdit ? coords_3 : coords_3_msw;
#else
        coords_3;
#endif

    CPPUNIT_ASSERT_EQUAL( ref_numChars_3, m_text->GetLastPosition() );
    for ( long i = 0; i < ref_numChars_3+1; i++ )
    {
        long x, y;
        ok = m_text->PositionToXY(i, &x, &y);
        CPPUNIT_ASSERT_EQUAL( true, ok );
        CPPUNIT_ASSERT_EQUAL( ref_coords_3[i].x, x );
        CPPUNIT_ASSERT_EQUAL( ref_coords_3[i].y, y );
    }
    ok = m_text->PositionToXY(ref_numChars_3 + 1, nullptr, nullptr);
    CPPUNIT_ASSERT_EQUAL( false, ok );

    // mixed empty/non-empty lines
    text = wxS("123\na\n\nX\n\n");
    m_text->SetValue(text);

#if wxHAS_2CHAR_NEWLINES
    // Take into account that every new line mark occupies
    // two characters, not one.
    const long numChars_msw_4 = 10 + 5;
    // Note: Two new line characters refer to the same X-Y position.
    XYPos coords_4_msw[numChars_msw_4 + 1] =
        { { 0, 0 },{ 1, 0 },{ 2, 0 },{ 3, 0 },{ 3, 0 },
          { 0, 1 },{ 1, 1 },{ 1, 1 },
          { 0, 2 },{ 0, 2 },
          { 0, 3 },{ 1, 3 },{ 1, 3 },
          { 0, 4 },{ 0, 4 },
          { 0, 5 } };
#endif // WXMSW

    const long numChars_4 = 10;
    wxASSERT(numChars_4 == text.length());
    XYPos coords_4[numChars_4+1] =
        { { 0, 0 }, { 1, 0 }, { 2, 0 }, { 3, 0 },
          { 0, 1 }, { 1, 1 },
          { 0, 2 },
          { 0, 3 }, { 1, 3 },
          { 0, 4 },
          { 0, 5 } };

    const long &ref_numChars_4 =
#if wxHAS_2CHAR_NEWLINES
        isRichEdit ? numChars_4 : numChars_msw_4;
#else
        numChars_4;
#endif

    XYPos *ref_coords_4 =
#if wxHAS_2CHAR_NEWLINES
        isRichEdit ? coords_4 : coords_4_msw;
#else
        coords_4;
#endif

    CPPUNIT_ASSERT_EQUAL( ref_numChars_4, m_text->GetLastPosition() );
    for ( long i = 0; i < ref_numChars_4+1; i++ )
    {
        long x, y;
        ok = m_text->PositionToXY(i, &x, &y);
        CPPUNIT_ASSERT_EQUAL( true, ok );
        CPPUNIT_ASSERT_EQUAL( ref_coords_4[i].x, x );
        CPPUNIT_ASSERT_EQUAL( ref_coords_4[i].y, y  );
    }
    ok = m_text->PositionToXY(ref_numChars_4 + 1, nullptr, nullptr);
    CPPUNIT_ASSERT_EQUAL( false, ok );
}

void TextCtrlTestCase::XYToPositionMultiLine()
{
    DoXYToPositionMultiLine(0);
}

#if wxUSE_RICHEDIT
void TextCtrlTestCase::XYToPositionMultiLineRich()
{
    DoXYToPositionMultiLine(wxTE_RICH);
}

void TextCtrlTestCase::XYToPositionMultiLineRich2()
{
    DoXYToPositionMultiLine(wxTE_RICH2);
}
#endif // wxUSE_RICHEDIT

void TextCtrlTestCase::DoXYToPositionMultiLine(long style)
{
    delete m_text;
    CreateText(style|wxTE_MULTILINE|wxTE_DONTWRAP);

#if wxHAS_2CHAR_NEWLINES
    const bool isRichEdit = (style & (wxTE_RICH | wxTE_RICH2)) != 0;
#endif

    wxString text;
    // empty field
    m_text->Clear();
    const long maxLineLength_0 = 0+1;
    const long numLines_0 = 1;
    CPPUNIT_ASSERT_EQUAL( numLines_0, m_text->GetNumberOfLines() );
    long pos_0[numLines_0+1][maxLineLength_0+1] =
        { {  0, -1 },
          { -1, -1 } };
    for ( long y = 0; y < numLines_0+1; y++ )
        for( long x = 0; x < maxLineLength_0+1; x++ )
        {
            long p = m_text->XYToPosition(x, y);
            INFO("x=" << x << ", y=" << y);
            CPPUNIT_ASSERT_EQUAL( pos_0[y][x], p );
        }

    // one line
    text = wxS("1234");
    m_text->SetValue(text);
    const long maxLineLength_1 = 4+1;
    const long numLines_1 = 1;
    CPPUNIT_ASSERT_EQUAL( numLines_1, m_text->GetNumberOfLines() );
    long pos_1[numLines_1+1][maxLineLength_1+1] =
        { {  0,  1,  2,  3,  4, -1 },
          { -1, -1, -1, -1, -1, -1 } };
    for ( long y = 0; y < numLines_1+1; y++ )
        for( long x = 0; x < maxLineLength_1+1; x++ )
        {
            long p = m_text->XYToPosition(x, y);
            INFO("x=" << x << ", y=" << y);
            CPPUNIT_ASSERT_EQUAL( pos_1[y][x], p  );
        }

    // few lines
    text = wxS("123\nab\nX");
    m_text->SetValue(text);
    const long maxLineLength_2 = 4;
    const long numLines_2 = 3;
    CPPUNIT_ASSERT_EQUAL( numLines_2, m_text->GetNumberOfLines() );
#if wxHAS_2CHAR_NEWLINES
    // Note: New lines are occupied by two characters.
    long pos_2_msw[numLines_2 + 1][maxLineLength_2 + 1] =
        { {  0,  1,  2,  3, -1 },   // New line occupies positions 3, 4
          {  5,  6,  7, -1, -1 },   // New line occupies positions 7, 8
          {  9, 10, -1, -1, -1 },
          { -1, -1, -1, -1, -1 } };
#endif // WXMSW
    long pos_2[numLines_2+1][maxLineLength_2+1] =
        { {  0,  1,  2,  3, -1 },
          {  4,  5,  6, -1, -1 },
          {  7,  8, -1, -1, -1 },
          { -1, -1, -1, -1, -1 } };

    long (&ref_pos_2)[numLines_2 + 1][maxLineLength_2 + 1] =
#if wxHAS_2CHAR_NEWLINES
        isRichEdit ? pos_2 : pos_2_msw;
#else
        pos_2;
#endif

    for ( long y = 0; y < numLines_2+1; y++ )
        for( long x = 0; x < maxLineLength_2+1; x++ )
        {
            long p = m_text->XYToPosition(x, y);
            INFO("x=" << x << ", y=" << y);
            CPPUNIT_ASSERT_EQUAL( ref_pos_2[y][x], p );
        }

    // only empty lines
    text = wxS("\n\n\n");
    m_text->SetValue(text);
    const long maxLineLength_3 = 1;
    const long numLines_3 = 4;
    CPPUNIT_ASSERT_EQUAL( numLines_3, m_text->GetNumberOfLines() );
#if wxHAS_2CHAR_NEWLINES
    // Note: New lines are occupied by two characters.
    long pos_3_msw[numLines_3 + 1][maxLineLength_3 + 1] =
        { {  0, -1 },    // New line occupies positions 0, 1
          {  2, -1 },    // New line occupies positions 2, 3
          {  4, -1 },    // New line occupies positions 4, 5
          {  6, -1 },
          { -1, -1 } };
#endif // WXMSW
    long pos_3[numLines_3+1][maxLineLength_3+1] =
        { {  0, -1 },
          {  1, -1 },
          {  2, -1 },
          {  3, -1 },
          { -1, -1 } };

    long (&ref_pos_3)[numLines_3 + 1][maxLineLength_3 + 1] =
#if wxHAS_2CHAR_NEWLINES
        isRichEdit ? pos_3 : pos_3_msw;
#else
        pos_3;
#endif

    for ( long y = 0; y < numLines_3+1; y++ )
        for( long x = 0; x < maxLineLength_3+1; x++ )
        {
            long p = m_text->XYToPosition(x, y);
            INFO("x=" << x << ", y=" << y);
            CPPUNIT_ASSERT_EQUAL( ref_pos_3[y][x], p );
        }

    // mixed empty/non-empty lines
    text = wxS("123\na\n\nX\n\n");
    m_text->SetValue(text);
    const long maxLineLength_4 = 4;
    const long numLines_4 = 6;
    CPPUNIT_ASSERT_EQUAL( numLines_4, m_text->GetNumberOfLines() );
#if wxHAS_2CHAR_NEWLINES
    // Note: New lines are occupied by two characters.
    long pos_4_msw[numLines_4 + 1][maxLineLength_4 + 1] =
        { {  0,  1,  2,  3, -1 },    // New line occupies positions 3, 4
          {  5,  6, -1, -1, -1 },    // New line occupies positions 6, 7
          {  8, -1, -1, -1, -1 },    // New line occupies positions 8, 9
          { 10, 11, -1, -1, -1 },    // New line occupies positions 11, 12
          { 13, -1, -1, -1, -1 },    // New line occupies positions 13, 14
          { 15, -1, -1, -1, -1 },
          { -1, -1, -1, -1, -1 } };
#endif // WXMSW
    long pos_4[numLines_4+1][maxLineLength_4+1] =
        { {  0,  1,  2,  3, -1 },
          {  4,  5, -1, -1, -1 },
          {  6, -1, -1, -1, -1 },
          {  7,  8, -1, -1, -1 },
          {  9, -1, -1, -1, -1 },
          { 10, -1, -1, -1, -1 },
          { -1, -1, -1, -1, -1 } };

    long (&ref_pos_4)[numLines_4 + 1][maxLineLength_4 + 1] =
#if wxHAS_2CHAR_NEWLINES
        isRichEdit ? pos_4 : pos_4_msw;
#else
        pos_4;
#endif

    for ( long y = 0; y < numLines_4+1; y++ )
        for( long x = 0; x < maxLineLength_4+1; x++ )
        {
            long p = m_text->XYToPosition(x, y);
            INFO("x=" << x << ", y=" << y);
            CPPUNIT_ASSERT_EQUAL( ref_pos_4[y][x], p );
        }
}

void TextCtrlTestCase::PositionToXYSingleLine()
{
    delete m_text;
    CreateText(wxTE_DONTWRAP);

    bool ok;
    wxString text;
    // empty field
    m_text->Clear();
    const long numChars_0 = 0;
    CPPUNIT_ASSERT_EQUAL( numChars_0, m_text->GetLastPosition() );
    for ( long i = 0; i <= numChars_0; i++ )
    {
        long x0, y0;
        ok = m_text->PositionToXY(i, &x0, &y0);
        CPPUNIT_ASSERT_EQUAL( true, ok );
        CPPUNIT_ASSERT_EQUAL( i, x0 );
        CPPUNIT_ASSERT_EQUAL( 0, y0 );
    }
    ok = m_text->PositionToXY(numChars_0+1, nullptr, nullptr);
    CPPUNIT_ASSERT_EQUAL( false, ok );

    // pure one line
    text = wxS("1234");
    m_text->SetValue(text);
    const long numChars_1 = text.length();
    CPPUNIT_ASSERT_EQUAL( numChars_1, m_text->GetLastPosition() );
    for ( long i = 0; i <= numChars_1; i++ )
    {
        long x1, y1;
        ok = m_text->PositionToXY(i, &x1, &y1);
        CPPUNIT_ASSERT_EQUAL( true, ok );
        CPPUNIT_ASSERT_EQUAL( i, x1 );
        CPPUNIT_ASSERT_EQUAL( 0, y1 );
    }
    ok = m_text->PositionToXY(numChars_1+1, nullptr, nullptr);
    CPPUNIT_ASSERT_EQUAL( false, ok );

    // with new line characters
    text = wxS("123\nab\nX");
    m_text->SetValue(text);
    const long numChars_2 = text.length();
    CPPUNIT_ASSERT_EQUAL( numChars_2, m_text->GetLastPosition() );
    for ( long i = 0; i <= numChars_2; i++ )
    {
        long x2, y2;
        ok = m_text->PositionToXY(i, &x2, &y2);
        CPPUNIT_ASSERT_EQUAL( true, ok );
        CPPUNIT_ASSERT_EQUAL( i, x2 );
        CPPUNIT_ASSERT_EQUAL( 0, y2 );
    }
    ok = m_text->PositionToXY(numChars_2+1, nullptr, nullptr);
    CPPUNIT_ASSERT_EQUAL( false, ok );
}

void TextCtrlTestCase::XYToPositionSingleLine()
{
    delete m_text;
    CreateText(wxTE_DONTWRAP);

    wxString text;
    // empty field
    m_text->Clear();
    CPPUNIT_ASSERT_EQUAL( 1, m_text->GetNumberOfLines() );
    for( long x = 0; x < m_text->GetLastPosition()+2; x++ )
    {
        long p0 = m_text->XYToPosition(x, 0);
        if ( x <= m_text->GetLastPosition() )
            CPPUNIT_ASSERT_EQUAL( x, p0 );
        else
            CPPUNIT_ASSERT_EQUAL( -1, p0 );

        p0 = m_text->XYToPosition(x, 1);
        CPPUNIT_ASSERT_EQUAL( -1, p0 );
    }

    // pure one line
    text = wxS("1234");
    m_text->SetValue(text);
    CPPUNIT_ASSERT_EQUAL( 1, m_text->GetNumberOfLines() );
    for( long x = 0; x < m_text->GetLastPosition()+2; x++ )
    {
        long p1 = m_text->XYToPosition(x, 0);
        if ( x <= m_text->GetLastPosition() )
            CPPUNIT_ASSERT_EQUAL( x, p1 );
        else
            CPPUNIT_ASSERT_EQUAL( -1, p1 );

        p1 = m_text->XYToPosition(x, 1);
        CPPUNIT_ASSERT_EQUAL( -1, p1 );
    }

    // with new line characters
    text = wxS("123\nab\nX");
    m_text->SetValue(text);
    CPPUNIT_ASSERT_EQUAL( 1, m_text->GetNumberOfLines() );
    for( long x = 0; x < m_text->GetLastPosition()+2; x++ )
    {
        long p2 = m_text->XYToPosition(x, 0);
        if ( x <= m_text->GetLastPosition() )
            CPPUNIT_ASSERT_EQUAL( x, p2 );
        else
            CPPUNIT_ASSERT_EQUAL( -1, p2 );

        p2 = m_text->XYToPosition(x, 1);
        CPPUNIT_ASSERT_EQUAL( -1, p2 );
    }
}

TEST_CASE("wxTextCtrl::ProcessEnter", "[wxTextCtrl][enter]")
{
    class TextCtrlCreator : public TextLikeControlCreator
    {
    public:
        explicit TextCtrlCreator(int styleToAdd = 0)
            : m_styleToAdd(styleToAdd)
        {
        }

        virtual wxControl* Create(wxWindow* parent, int style) const override
        {
            return new wxTextCtrl(parent, wxID_ANY, wxString(),
                                  wxDefaultPosition, wxDefaultSize,
                                  style | m_styleToAdd);
        }

        virtual TextLikeControlCreator* CloneAsMultiLine() const override
        {
            return new TextCtrlCreator(wxTE_MULTILINE);
        }

    private:
        int m_styleToAdd;
    };

    TestProcessEnter(TextCtrlCreator());
}

TEST_CASE("wxTextCtrl::GetBestSize", "[wxTextCtrl][best-size]")
{
    struct GetBestSizeFor
    {
        wxSize operator()(const wxString& text) const
        {
            wxScopedPtr<wxTextCtrl>
                t(new wxTextCtrl(wxTheApp->GetTopWindow(), wxID_ANY, text,
                                 wxDefaultPosition, wxDefaultSize,
                                 wxTE_MULTILINE));
            return t->GetBestSize();
        }
    } getBestSizeFor;

    wxString s;
    const wxSize sizeEmpty = getBestSizeFor(s);

    // Empty control should have some reasonable vertical size.
    CHECK( sizeEmpty.y > 0 );

    s += "1\n2\n3\n4\n5\n";
    const wxSize sizeMedium = getBestSizeFor(s);

    // Control with a few lines of text in it should be taller.
    CHECK( sizeMedium.y > sizeEmpty.y );

    s += "6\n7\n8\n9\n10\n";
    const wxSize sizeLong = getBestSizeFor(s);

    // And a control with many lines in it should be even more so.
    CHECK( sizeLong.y > sizeMedium.y );

    s += s;
    s += s;
    s += s;
    const wxSize sizeVeryLong = getBestSizeFor(s);

    // However there is a cutoff at 10 lines currently, so anything longer than
    // that should still have the same best size.
    CHECK( sizeVeryLong.y == sizeLong.y );
}

#if wxUSE_CLIPBOARD

TEST_CASE("wxTextCtrl::LongPaste", "[wxTextCtrl][clipboard][paste]")
{
    long style = 0;

    SECTION("Plain")
    {
        style = wxTE_MULTILINE;
    }

    // wxTE_RICH[2] style only makes any different under MSW, so don't bother
    // testing it under the other platforms.
#ifdef __WXMSW__
    SECTION("Rich")
    {
        style = wxTE_MULTILINE | wxTE_RICH;
    }

    SECTION("Rich v2")
    {
        style = wxTE_MULTILINE | wxTE_RICH2;
    }
#endif // __WXMSW__

    if ( !style )
    {
        // This can happen when explicitly selecting just a single section to
        // execute -- this code still runs even if the corresponding section is
        // skipped, so we have to explicitly skip it too in this case.
        return;
    }

    wxScopedPtr<wxTextCtrl>
        text(new wxTextCtrl(wxTheApp->GetTopWindow(), wxID_ANY, wxString(),
                            wxDefaultPosition, wxDefaultSize, style));

    // This could actually be much higher, but it makes the test proportionally
    // slower, so use a relatively small (but still requiring more space than
    // the default maximum length under MSW) number here.
    const int NUM_LINES = 10000;

    {
        wxClipboardLocker lock;

        // Build a longish string.
        wxString s;
        s.reserve(NUM_LINES*5 + 10);
        for ( int n = 0; n < NUM_LINES; ++n )
        {
            s += wxString::Format("%04d\n", n);
        }

        s += "THE END";

        wxTheClipboard->AddData(new wxTextDataObject(s));
    }

    text->ChangeValue("THE BEGINNING\n");
    text->SetInsertionPointEnd();
    text->Paste();

    const int numLines = text->GetNumberOfLines();

    CHECK( numLines == NUM_LINES + 2 );
    CHECK( text->GetLineText(numLines - 1) == "THE END" );
}

#endif // wxUSE_CLIPBOARD

TEST_CASE("wxTextCtrl::EventsOnCreate", "[wxTextCtrl][event]")
{
    wxWindow* const parent = wxTheApp->GetTopWindow();

    EventCounter updated(parent, wxEVT_TEXT);

    wxScopedPtr<wxTextCtrl> text(new wxTextCtrl(parent, wxID_ANY, "Hello"));

    // Creating the control shouldn't result in any wxEVT_TEXT events.
    CHECK( updated.GetCount() == 0 );

    // Check that modifying using SetValue() it does generate the event, just
    // to verify that this test works (there are more detailed tests for this
    // in TextEntryTestCase::TextChangeEvents()).
    text->SetValue("Bye");
    CHECK( updated.GetCount() == 1 );
}

TEST_CASE("wxTextCtrl::InitialCanUndo", "[wxTextCtrl][undo]")
{
    wxWindow* const parent = wxTheApp->GetTopWindow();

    const long styles[] = { 0, wxTE_RICH, wxTE_RICH2 };

    for ( size_t n = 0; n < WXSIZEOF(styles); n++ )
    {
        const long style = styles[n];

#ifdef __MINGW32_TOOLCHAIN__
        if ( style == wxTE_RICH2 )
        {
            // We can't call ITextDocument::Undo() in wxMSW code when using
            // MinGW32, so this test would always fail with it.
            WARN("Skipping test known to fail with MinGW-32.");
        }
        continue;
#endif // __MINGW32_TOOLCHAIN__

        INFO("wxTextCtrl with style " << style);

        wxScopedPtr<wxTextCtrl> text(new wxTextCtrl(parent, wxID_ANY, "",
                                                    wxDefaultPosition,
                                                    wxDefaultSize,
                                                    style));

        CHECK( !text->CanUndo() );
    }
}

// This test would always fail with MinGW-32 for the same reason as described
// above.
#ifndef __MINGW32_TOOLCHAIN__

TEST_CASE("wxTextCtrl::EmptyUndoBuffer", "[wxTextCtrl][undo]")
{
    if ( wxIsRunningUnderWine() )
    {
        // Wine doesn't implement EM_GETOLEINTERFACE and related stuff currently
        WARN("Skipping test known to fail under Wine.");
        return;
    }

    wxScopedPtr<wxTextCtrl> text(new wxTextCtrl(wxTheApp->GetTopWindow(),
                                                wxID_ANY, "",
                                                wxDefaultPosition,
                                                wxDefaultSize,
                                                wxTE_MULTILINE | wxTE_RICH2));

    text->AppendText("foo");

    if ( !text->CanUndo() )
    {
        WARN("Skipping test as Undo() is not supported on this platform.");
        return;
    }

    text->EmptyUndoBuffer();

    CHECK_FALSE( text->CanUndo() );

    CHECK_NOTHROW( text->Undo() );

    CHECK( text->GetValue() == "foo" );
}

#endif // __MINGW32_TOOLCHAIN__

#endif //wxUSE_TEXTCTRL
