///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/styledtextctrltest.cpp
// Purpose:     wxStyledTextCtrl unit test
// Author:      New Pagodi
// Created:     2019-03-10
// Copyright:   (c) 2019 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_STC

#if defined(WXUSINGDLL)


#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/stc/stc.h"
#include "wx/uiaction.h"

#include "testwindow.h"

#if defined(__WXOSX_COCOA__) || defined(__WXMSW__) || defined(__WXGTK__)

class StcPopupWindowsTestCase
{
public:
    StcPopupWindowsTestCase()
        : m_stc(new wxStyledTextCtrl(wxTheApp->GetTopWindow(), wxID_ANY))
    {
        m_focusAlwaysRetained=true;
        m_calltipClickReceived=false;

        m_stc->Bind(wxEVT_KILL_FOCUS,
                    &StcPopupWindowsTestCase::OnKillSTCFocus, this);
        m_stc->Bind(wxEVT_STC_CALLTIP_CLICK,
                    &StcPopupWindowsTestCase::OnCallTipClick, this);
    }

    ~StcPopupWindowsTestCase()
    {
        delete m_stc;
    }

    void OnKillSTCFocus(wxFocusEvent& WXUNUSED(event))
    {
        m_focusAlwaysRetained=false;
    }

    void OnCallTipClick(wxStyledTextEvent& WXUNUSED(event))
    {
        m_calltipClickReceived=true;
    }

protected:
    wxStyledTextCtrl* const m_stc;
    bool m_focusAlwaysRetained;
    bool m_calltipClickReceived;
};

// This set of tests is used to verify that an autocompletion popup does not
// take focus from its parent styled text control.
TEST_CASE_METHOD(StcPopupWindowsTestCase,
                 "wxStyledTextCtrl::AutoComp",
                 "[wxStyledTextCtrl][focus]")
{
    m_stc->SetFocus();
    m_focusAlwaysRetained = true;
    m_stc->AutoCompShow(0,"ability able about above abroad absence absent");

#if wxUSE_UIACTIONSIMULATOR
    // Pressing the tab key should cause the current entry in the list to be
    // entered into the styled text control. However with GTK+, characters sent
    // with the UI simulator seem to arrive too late, so select the current
    // entry with a double click instead.

    wxUIActionSimulator sim;

#ifdef __WXGTK__
    wxPoint zeroPosition = m_stc->PointFromPosition(0);
    int textHt = m_stc->TextHeight(0);
    int textWd = m_stc->TextWidth(0,"ability");
    wxPoint autoCompPoint(zeroPosition.x + textWd/2,
                            zeroPosition.y + textHt + textHt/2);
    wxPoint scrnPoint = m_stc->ClientToScreen(autoCompPoint);
    sim.MouseMove(scrnPoint);
    sim.MouseDblClick();
#else
    sim.Char(WXK_TAB);
#endif // __WXGTK__
    ::wxYield();
    CHECK( m_stc->GetText() == "ability" );
#endif //wxUSE_UIACTIONSIMULATOR

    if ( m_stc->AutoCompActive() )
        m_stc->AutoCompCancel();

    CHECK_FOCUS_IS( m_stc );

    // Unfortunately under GTK we do get focus loss events, at least sometimes
    // (and actually more often than not, especially with GTK2, but this
    // happens with GTK3 too).
#ifndef __WXGTK__
    CHECK( m_focusAlwaysRetained );
#endif // !__WXGTK__
}

// This test is used to verify that a call tip receives mouse clicks. However
// the clicks do sent with the UI simulator do not seem to be received on 
// cocoa for some reason, so skip the test there for now.
#if !defined(__WXOSX_COCOA__)
TEST_CASE_METHOD(StcPopupWindowsTestCase,
                 "wxStyledTextCtrl::Calltip",
                 "[wxStyledTextCtrl][focus]")
{
    m_stc->SetFocus();
    m_calltipClickReceived = false;
    m_focusAlwaysRetained = true;

    wxString calltipText = "This is a calltip.";
    m_stc->CallTipShow(0,calltipText);

#if wxUSE_UIACTIONSIMULATOR
    wxUIActionSimulator sim;
    wxPoint zeroPosition = m_stc->PointFromPosition(0);
    int textHt = m_stc->TextHeight(0);
    int textWd = m_stc->TextWidth(0,calltipText);

    // zeroPosition is the top left of position 0 and the call tip should have
    // roughly the same height as textHt (there seems to be some extra padding
    // that makes it a little taller, but it's roughly the same height),
    // so (zeroPosition.x+textWd/2,zeroPosition.y+textHt+textHt/2) should
    // be the middle of the calltip.
    wxPoint calltipMidPoint(zeroPosition.x + textWd/2,
                            zeroPosition.y + textHt + textHt/2);
    wxPoint scrnPoint = m_stc->ClientToScreen(calltipMidPoint);
    sim.MouseMove(scrnPoint);
    sim.MouseClick();
    ::wxYield();

    CHECK( m_calltipClickReceived );
#endif // wxUSE_UIACTIONSIMULATOR

    if ( m_stc->CallTipActive() )
        m_stc->CallTipCancel();

    // Verify that clicking the call tip did not take focus from the STC.
    //
    // Unfortunately this test fails for unknown reasons under Xvfb (but only
    // there).
    if ( !IsRunningUnderXVFB() )
        CHECK_FOCUS_IS( m_stc );

    // With wxGTK there is the same problem here as in the test above.
#ifndef __WXGTK__
    CHECK( m_focusAlwaysRetained );
#endif // !__WXGTK__
}

#endif // !defined(__WXOSX_COCOA__)

static const int TEXT_HEIGHT = 200;

#if defined(__WXMSW__) && !defined(__WXUNIVERSAL__)
#define wxHAS_2CHAR_NEWLINES 1
#else
#define wxHAS_2CHAR_NEWLINES 0
#endif

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class StyledTextCtrlTestCase : public CppUnit::TestCase
{
public:
    StyledTextCtrlTestCase() { }

    virtual void setUp() override;
    virtual void tearDown() override;

private:

    CPPUNIT_TEST_SUITE( StyledTextCtrlTestCase );
        CPPUNIT_TEST( PositionToXYMultiLine );
        CPPUNIT_TEST( XYToPositionMultiLine );
    CPPUNIT_TEST_SUITE_END();

    void PositionToXYMultiLine();
    void XYToPositionMultiLine();

    void DoPositionToXYMultiLine(long style);
    void DoXYToPositionMultiLine(long style);

    // Create the control with the following styles added to ms_style which may
    // (or not) already contain wxTE_MULTILINE.
    void CreateText(long extraStyles);

    wxStyledTextCtrl *m_text;

    static long ms_style;

    wxDECLARE_NO_COPY_CLASS(StyledTextCtrlTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( StyledTextCtrlTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( StyledTextCtrlTestCase, "StyledTextCtrlTestCase" );

// ----------------------------------------------------------------------------
// test initialization
// ----------------------------------------------------------------------------

// This is 0 initially and set to wxTE_MULTILINE later to allow running the
// same tests for both single and multi line controls.
long StyledTextCtrlTestCase::ms_style = 0;

void StyledTextCtrlTestCase::CreateText(long extraStyles)
{
    const long style = ms_style | extraStyles;
    const int h = (style & wxTE_MULTILINE) ? TEXT_HEIGHT : -1;
    m_text = new wxStyledTextCtrl(wxTheApp->GetTopWindow(), wxID_ANY,
                                  wxDefaultPosition, wxSize(400, h),
                                  style);
}

void StyledTextCtrlTestCase::setUp()
{
    CreateText(0);
}

void StyledTextCtrlTestCase::tearDown()
{
    wxDELETE(m_text);
}

// ----------------------------------------------------------------------------
// tests themselves
// ----------------------------------------------------------------------------

void StyledTextCtrlTestCase::PositionToXYMultiLine()
{
    DoPositionToXYMultiLine(0);
}

void StyledTextCtrlTestCase::DoPositionToXYMultiLine(long style)
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

void StyledTextCtrlTestCase::XYToPositionMultiLine()
{
    DoXYToPositionMultiLine(0);
}

void StyledTextCtrlTestCase::DoXYToPositionMultiLine(long style)
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

#endif // defined(__WXOSX_COCOA__) || defined(__WXMSW__) || defined(__WXGTK__)

#endif // WXUSINGDLL

#endif // wxUSE_STC

