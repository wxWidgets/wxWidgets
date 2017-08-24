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

static const int TEXT_HEIGHT = 200;

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
        SINGLE_AND_MULTI_TESTS();

        // Now switch to the multi-line text controls.
        CPPUNIT_TEST( PseudoTestSwitchToMultiLineStyle );

        // Rerun some of the tests above. Notice that not all of them pass, so
        // we can't just use wxTEXT_ENTRY_TESTS() here. For some of them it's
        // normal, e.g. Hint() test isn't supposed to work for multi-line
        // controls. Others, such as InsertionPoint() and TextChangeEvents()
        // don't pass neither but this could be a bug.
        CPPUNIT_TEST( SetValue );
        CPPUNIT_TEST( Selection );
        CPPUNIT_TEST( Replace );
        WXUISIM_TEST( Editable );
        CPPUNIT_TEST( CopyPaste );
        CPPUNIT_TEST( UndoRedo );

        SINGLE_AND_MULTI_TESTS();


        // All tests from now on are for multi-line controls only.
        CPPUNIT_TEST( MultiLineReplace );
        //WXUISIM_TEST( ProcessEnter );
        WXUISIM_TEST( Url );
        CPPUNIT_TEST( Style );
        CPPUNIT_TEST( FontStyle );
        CPPUNIT_TEST( Lines );
        CPPUNIT_TEST( LogTextCtrl );
        CPPUNIT_TEST( LongText );
        CPPUNIT_TEST( PositionToCoords );
        CPPUNIT_TEST( PositionToCoordsRich );
        CPPUNIT_TEST( PositionToCoordsRich2 );
        CPPUNIT_TEST( PositionToXYMultiLine );
        CPPUNIT_TEST( XYToPositionMultiLine );
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
    //void ProcessEnter();
    void Url();
    void Style();
    void FontStyle();
    void Lines();
    void LogTextCtrl();
    void LongText();
    void PositionToCoords();
    void PositionToCoordsRich();
    void PositionToCoordsRich2();
    void PositionToXYMultiLine();
    void XYToPositionMultiLine();
    void PositionToXYSingleLine();
    void XYToPositionSingleLine();

    void DoPositionToCoordsTestWithStyle(long style);

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
    wxSize size;
    if ( ms_style == wxTE_MULTILINE )
        size = wxSize(400, TEXT_HEIGHT);

    m_text = new wxTextCtrl(wxTheApp->GetTopWindow(), wxID_ANY, "",
                            wxDefaultPosition, size,
                            ms_style | extraStyles);
}

void TextCtrlTestCase::setUp()
{
    CreateText(ms_style);
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
#if wxUSE_UIACTIONSIMULATOR && defined(__WXMSW__)
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
#ifndef __WXOSX__
    delete m_text;
    // We need wxTE_RICH under windows for style support
    CreateText(wxTE_RICH);

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
    m_text->GetStyle(5, attrOut);

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
    // This currently doesn't work neither in wxGTK, wxUniv, or wxOSX/Cocoa, see
    // #12366, where GetNumberOfLines() always returns the number of logical,
    // not physical, lines.
    m_text->AppendText("\n" + wxString(50, '1') + ' ' + wxString(50, '2'));
#if defined(__WXGTK__) || defined(__WXOSX_COCOA__) || defined(__WXUNIVERSAL__)
    CPPUNIT_ASSERT_EQUAL(6, m_text->GetNumberOfLines());
#else
    CPPUNIT_ASSERT(m_text->GetNumberOfLines() > 6);
#endif
}

void TextCtrlTestCase::LogTextCtrl()
{
    CPPUNIT_ASSERT(m_text->IsEmpty());

    wxLogTextCtrl* logtext = new wxLogTextCtrl(m_text);

    wxLog* old = wxLog::SetActiveTarget(logtext);

    logtext->LogText("text");

    delete wxLog::SetActiveTarget(old);

    CPPUNIT_ASSERT(!m_text->IsEmpty());
}

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
    CreateText(style);

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

void TextCtrlTestCase::PositionToXYMultiLine()
{
    delete m_text;
    CreateText(wxTE_MULTILINE|wxTE_DONTWRAP);

    bool ok;
    wxString text;
    // empty field

    m_text->Clear();
    const long numChars_0 = 0;
    CPPUNIT_ASSERT_EQUAL( m_text->GetLastPosition(), numChars_0 );
    struct { long x, y; } coords_0[numChars_0+1] =
        { { 0, 0 } };

    for ( long i = 0; i < (long)WXSIZEOF(coords_0); i++ )
    {
        long x, y;
        ok = m_text->PositionToXY(i, &x, &y);
        CPPUNIT_ASSERT_EQUAL( ok, true );
        CPPUNIT_ASSERT_EQUAL( x, coords_0[i].x );
        CPPUNIT_ASSERT_EQUAL( y, coords_0[i].y );
    }
    ok = m_text->PositionToXY(WXSIZEOF(coords_0), NULL, NULL);
    CPPUNIT_ASSERT_EQUAL( ok, false );

    // one line
    text = wxS("1234");
    m_text->SetValue(text);
    const long numChars_1 = 4;
    wxASSERT( numChars_1 == text.Length() );
    CPPUNIT_ASSERT_EQUAL( m_text->GetLastPosition(), numChars_1 );
    struct { long x, y; } coords_1[numChars_1+1] =
        { { 0, 0 }, { 1, 0 }, { 2, 0}, { 3, 0 }, { 4, 0 } };

    for ( long i = 0; i < (long)WXSIZEOF(coords_1); i++ )
    {
        long x, y;
        ok = m_text->PositionToXY(i, &x, &y);
        CPPUNIT_ASSERT_EQUAL( ok, true );
        CPPUNIT_ASSERT_EQUAL( x, coords_1[i].x );
        CPPUNIT_ASSERT_EQUAL( y, coords_1[i].y );
    }
    ok = m_text->PositionToXY(WXSIZEOF(coords_1), NULL, NULL);
    CPPUNIT_ASSERT_EQUAL( ok, false );

    // few lines
    text = wxS("123\nab\nX");
    m_text->SetValue(text);
#if defined(__WXMSW__)
    // Take into account that every new line mark occupies
    // two characters, not one.
    const long numChars_2 = 8 + 2;
#else
    const long numChars_2 = 8;
#endif // WXMSW/!WXMSW
    CPPUNIT_ASSERT_EQUAL( m_text->GetLastPosition(), numChars_2 );
#if defined(__WXMSW__)
    // Note: Two new line characters refer to the same X-Y position.
    struct { long x, y; } coords_2[numChars_2 + 1] =
    { { 0, 0 }, { 1, 0 }, { 2, 0 }, { 3, 0 }, { 3, 0 },
      { 0, 1 }, { 1, 1 }, { 2, 1 }, { 2, 1 },
      { 0, 2 }, { 1, 2 } };
#else
    struct { long x, y; } coords_2[numChars_2 + 1] =
    { { 0, 0 }, { 1, 0 }, { 2, 0 }, { 3, 0 },
      { 0, 1 }, { 1, 1 }, { 2, 1 },
      { 0, 2 }, { 1, 2 } };
#endif // WXMSW/!WXMSW

    for ( long i = 0; i < (long)WXSIZEOF(coords_2); i++ )
    {
        long x, y;
        ok = m_text->PositionToXY(i, &x, &y);
        CPPUNIT_ASSERT_EQUAL( ok, true );
        CPPUNIT_ASSERT_EQUAL( x, coords_2[i].x );
        CPPUNIT_ASSERT_EQUAL( y, coords_2[i].y );
    }
    ok = m_text->PositionToXY(WXSIZEOF(coords_2), NULL, NULL);
    CPPUNIT_ASSERT_EQUAL( ok, false );

    // only empty lines
    text = wxS("\n\n\n");
    m_text->SetValue(text);
#if defined(__WXMSW__)
    // Take into account that every new line mark occupies
    // two characters, not one.
    const long numChars_3 = 3 + 3;
#else
    const long numChars_3 = 3;
#endif // WXMSW/!WXMSW
    CPPUNIT_ASSERT_EQUAL( m_text->GetLastPosition(), numChars_3 );
#if defined(__WXMSW__)
    // Note: Two new line characters refer to the same X-Y position.
    struct { long x, y; } coords_3[numChars_3 + 1] =
        { { 0, 0 }, { 0, 0 },
          { 0, 1 }, { 0, 1 },
          { 0, 2 }, { 0, 2 },
          { 0, 3 } };
#else
    struct { long x, y; } coords_3[numChars_3+1] =
        { { 0, 0 },
          { 0, 1 },
          { 0, 2 },
          { 0, 3 } };
#endif // WXMSW/!WXMSW

    for ( long i = 0; i < (long)WXSIZEOF(coords_3); i++ )
    {
        long x, y;
        ok = m_text->PositionToXY(i, &x, &y);
        CPPUNIT_ASSERT_EQUAL( ok, true );
        CPPUNIT_ASSERT_EQUAL( x, coords_3[i].x );
        CPPUNIT_ASSERT_EQUAL( y, coords_3[i].y );
    }
    ok = m_text->PositionToXY(WXSIZEOF(coords_3), NULL, NULL);
    CPPUNIT_ASSERT_EQUAL( ok, false );

    // mixed empty/non-empty lines
    text = wxS("123\na\n\nX\n\n");
    m_text->SetValue(text);
#if defined(__WXMSW__)
    // Take into account that every new line mark occupies
    // two characters, not one.
    const long numChars_4 = 10 + 5;
#else
    const long numChars_4 = 10;
#endif // WXMSW/!WXMSW
    CPPUNIT_ASSERT_EQUAL( m_text->GetLastPosition(), numChars_4 );
#if defined(__WXMSW__)
    // Note: Two new line characters refer to the same X-Y position.
    struct { long x, y; } coords_4[numChars_4 + 1] =
        { { 0, 0 }, { 1, 0 }, { 2, 0 }, { 3, 0 }, { 3, 0 },
          { 0, 1 }, { 1, 1 }, { 1, 1 },
          { 0, 2 }, { 0, 2 },
          { 0, 3 }, { 1, 3 }, { 1, 3 },
          { 0, 4 }, { 0, 4 },
          { 0, 5 } };
#else
    struct { long x, y; } coords_4[numChars_4+1] =
        { { 0, 0 }, { 1, 0 }, { 2, 0 }, { 3, 0 },
          { 0, 1 }, { 1, 1 },
          { 0, 2 },
          { 0, 3 }, { 1, 3 },
          { 0, 4 },
          { 0, 5 } };
#endif // WXMSW/!WXMSW

    for ( long i = 0; i < (long)WXSIZEOF(coords_4); i++ )
    {
        long x, y;
        ok = m_text->PositionToXY(i, &x, &y);
        CPPUNIT_ASSERT_EQUAL( ok, true );
        CPPUNIT_ASSERT_EQUAL( x, coords_4[i].x );
        CPPUNIT_ASSERT_EQUAL( y, coords_4[i].y );
    }
    ok = m_text->PositionToXY(WXSIZEOF(coords_4), NULL, NULL);
    CPPUNIT_ASSERT_EQUAL( ok, false );
}

void TextCtrlTestCase::XYToPositionMultiLine()
{
    delete m_text;
    CreateText(wxTE_MULTILINE|wxTE_DONTWRAP);

    wxString text;
    // empty field
    m_text->Clear();
    const long maxLineLength_0 = 0;
    const long numLines_0 = 1;
    CPPUNIT_ASSERT_EQUAL( m_text->GetNumberOfLines(), numLines_0 );
    long pos_0[numLines_0+1][maxLineLength_0+1] =
        { { 0 } };
    for ( long y = 0; y < numLines_0; y++ )
        for( long x = 0; x < maxLineLength_0+1; x++ )
        {
            long p = m_text->XYToPosition(x, y);
            CPPUNIT_ASSERT_EQUAL( p, pos_0[y][x] );
        }

    // one line
    text = wxS("1234");
    m_text->SetValue(text);
    const long maxLineLength_1 = 4;
    const long numLines_1 = 1;
    CPPUNIT_ASSERT_EQUAL( m_text->GetNumberOfLines(), numLines_1 );
    long pos_1[numLines_1+1][maxLineLength_1+1] =
        { {  0,  1,  2,  3,  4 },
          { -1, -1, -1, -1, -1 } };
    for ( long y = 0; y < numLines_1; y++ )
        for( long x = 0; x < maxLineLength_1+1; x++ )
        {
            long p = m_text->XYToPosition(x, y);
            CPPUNIT_ASSERT_EQUAL( p, pos_1[y][x] );
        }

    // few lines
    text = wxS("123\nab\nX");
    m_text->SetValue(text);
    const long maxLineLength_2 = 4;
    const long numLines_2 = 3;
    CPPUNIT_ASSERT_EQUAL( m_text->GetNumberOfLines(), numLines_2 );
#if defined(__WXMSW__)
    // Note: New lines are occupied by two characters.
    long pos_2[numLines_2 + 1][maxLineLength_2 + 1] =
        { { 0,  1,  2,  3, -1 },    // New line occupies positions 3, 4
          { 5,  6,  7, -1, -1 },    // New line occupies positions 7, 8
          { 9, 10, -1, -1, -1 } };
#else
    long pos_2[numLines_2+1][maxLineLength_2+1] =
        { {  0,  1,  2,  3, -1 },
          {  4,  5,  6, -1, -1 },
          {  7, -1, -1, -1, -1 } };
#endif // WXMSW/!WXMSW

    for ( long y = 0; y < numLines_2; y++ )
        for( long x = 0; x < maxLineLength_2+1; x++ )
        {
            long p = m_text->XYToPosition(x, y);
            CPPUNIT_ASSERT_EQUAL( p, pos_2[y][x] );
        }

    // only empty lines
    text = wxS("\n\n\n");
    m_text->SetValue(text);
    const long maxLineLength_3 = 1;
    const long numLines_3 = 4;
    CPPUNIT_ASSERT_EQUAL( m_text->GetNumberOfLines(), numLines_3 );
#if defined(__WXMSW__)
    // Note: New lines are occupied by two characters.
    long pos_3[numLines_3 + 1][maxLineLength_3 + 1] =
        { {  0, -1 },    // New line occupies positions 0, 1
          {  2, -1 },    // New line occupies positions 2, 3
          {  4, -1 },    // New line occupies positions 4, 5
          {  6, -1 },
          { -1, -1 } };
#else
    long pos_3[numLines_3+1][maxLineLength_3+1] =
        { {  0, -1 },
          {  1, -1 },
          {  2, -1 },
          { -1, -1 },
          { -1, -1 } };
#endif // WXMSW/!WXMSW

    for ( long y = 0; y < numLines_3; y++ )
        for( long x = 0; x < maxLineLength_3+1; x++ )
        {
            long p = m_text->XYToPosition(x, y);
            CPPUNIT_ASSERT_EQUAL( p, pos_3[y][x] );
        }

    // mixed empty/non-empty lines
    text = wxS("123\na\n\nX\n\n");
    m_text->SetValue(text);
    const long maxLineLength_4 = 4;
    const long numLines_4 = 6;
    CPPUNIT_ASSERT_EQUAL( m_text->GetNumberOfLines(), numLines_4 );
#if defined(__WXMSW__)
    // Note: New lines are occupied by two characters.
    long pos_4[numLines_4 + 1][maxLineLength_4 + 1] =
        { {  0,  1,  2,  3, -1 },    // New line occupies positions 3, 4
          {  5,  6, -1, -1, -1 },    // New line occupies positions 6, 7
          {  8, -1, -1, -1, -1 },    // New line occupies positions 8, 9
          { 10, 11, -1, -1, -1 },    // New line occupies positions 11, 12
          { 13, -1, -1, -1, -1 },    // New line occupies positions 13, 14
          { 15, -1, -1, -1, -1 },
          { -1, -1, -1, -1, -1 } };
#else
    long pos_4[numLines_4+1][maxLineLength_4+1] =
        { {  0,  1,  2,  3, -1 },
          {  4,  5, -1, -1, -1 },
          {  6, -1, -1, -1, -1 },
          {  7,  8, -1, -1, -1 },
          {  9, -1, -1, -1, -1 },
          { -1, -1, -1, -1, -1 },
          { -1, -1, -1, -1, -1 } };
#endif // WXMSW/!WXMSW

    for ( long y = 0; y < numLines_4; y++ )
        for( long x = 0; x < maxLineLength_4+1; x++ )
        {
            long p = m_text->XYToPosition(x, y);
            CPPUNIT_ASSERT_EQUAL( p, pos_4[y][x] );
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
    CPPUNIT_ASSERT_EQUAL( m_text->GetLastPosition(), numChars_0 );
    for ( long i = 0; i <= numChars_0; i++ )
    {
        long x0, y0;
        ok = m_text->PositionToXY(i, &x0, &y0);
        CPPUNIT_ASSERT_EQUAL( ok, true );
        CPPUNIT_ASSERT_EQUAL( x0, i );
        CPPUNIT_ASSERT_EQUAL( y0, 0 );
    }
    ok = m_text->PositionToXY(numChars_0+1, NULL, NULL);
    CPPUNIT_ASSERT_EQUAL( ok, false );

    // pure one line
    text = wxS("1234");
    m_text->SetValue(text);
    const long numChars_1 = text.Length();
    CPPUNIT_ASSERT_EQUAL( m_text->GetLastPosition(), numChars_1 );
    for ( long i = 0; i <= numChars_1; i++ )
    {
        long x1, y1;
        ok = m_text->PositionToXY(i, &x1, &y1);
        CPPUNIT_ASSERT_EQUAL( ok, true );
        CPPUNIT_ASSERT_EQUAL( x1, i );
        CPPUNIT_ASSERT_EQUAL( y1, 0 );
    }
    ok = m_text->PositionToXY(numChars_1+1, NULL, NULL);
    CPPUNIT_ASSERT_EQUAL( ok, false );

    // with new line characters
    text = wxS("123\nab\nX");
    m_text->SetValue(text);
    const long numChars_2 = text.Length();
    CPPUNIT_ASSERT_EQUAL( m_text->GetLastPosition(), numChars_2 );
    for ( long i = 0; i <= numChars_2; i++ )
    {
        long x2, y2;
        ok = m_text->PositionToXY(i, &x2, &y2);
        CPPUNIT_ASSERT_EQUAL( ok, true );
        CPPUNIT_ASSERT_EQUAL( x2, i );
        CPPUNIT_ASSERT_EQUAL( y2, 0 );
    }
    ok = m_text->PositionToXY(numChars_2+1, NULL, NULL);
    CPPUNIT_ASSERT_EQUAL( ok, false );
}

void TextCtrlTestCase::XYToPositionSingleLine()
{
    delete m_text;
    CreateText(wxTE_DONTWRAP);

    wxString text;
    // empty field
    m_text->Clear();
    CPPUNIT_ASSERT_EQUAL( m_text->GetNumberOfLines(), 1 );
    for( long x = 0; x < m_text->GetLastPosition()+2; x++ )
    {
        long p0 = m_text->XYToPosition(x, 0);
        if ( x <= m_text->GetLastPosition() )
            CPPUNIT_ASSERT_EQUAL( p0, x );
        else
            CPPUNIT_ASSERT_EQUAL( p0, -1 );

        p0 = m_text->XYToPosition(x, 1);
        CPPUNIT_ASSERT_EQUAL( p0, -1 );
    }

    // pure one line
    text = wxS("1234");
    m_text->SetValue(text);
    CPPUNIT_ASSERT_EQUAL( m_text->GetNumberOfLines(), 1 );
    for( long x = 0; x < m_text->GetLastPosition()+2; x++ )
    {
        long p1 = m_text->XYToPosition(x, 0);
        if ( x <= m_text->GetLastPosition() )
            CPPUNIT_ASSERT_EQUAL( p1, x );
        else
            CPPUNIT_ASSERT_EQUAL( p1, -1 );

        p1 = m_text->XYToPosition(x, 1);
        CPPUNIT_ASSERT_EQUAL( p1, -1 );
    }

    // with new line characters
    text = wxS("123\nab\nX");
    m_text->SetValue(text);
    CPPUNIT_ASSERT_EQUAL( m_text->GetNumberOfLines(), 1 );
    for( long x = 0; x < m_text->GetLastPosition()+2; x++ )
    {
        long p2 = m_text->XYToPosition(x, 0);
        if ( x <= m_text->GetLastPosition() )
            CPPUNIT_ASSERT_EQUAL( p2, x );
        else
            CPPUNIT_ASSERT_EQUAL( p2, -1 );

        p2 = m_text->XYToPosition(x, 1);
        CPPUNIT_ASSERT_EQUAL( p2, -1 );
    }
}

#endif //wxUSE_TEXTCTRL
