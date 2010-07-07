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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/textctrl.h"
#endif // WX_PRECOMP

#include "textentrytest.h"
#include "testableframe.h"
#include "wx/uiaction.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class TextCtrlTestCase : public TextEntryTestCase
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
        CPPUNIT_TEST( ReadOnly );
        CPPUNIT_TEST( MaxLength );
        CPPUNIT_TEST( StreamInput );
        CPPUNIT_TEST( Redirector );
        //CPPUNIT_TEST( ProcessEnter );
        CPPUNIT_TEST( Url );
        CPPUNIT_TEST( Style );
    CPPUNIT_TEST_SUITE_END();

    void MultiLineReplace();
    void ReadOnly();
    void MaxLength();
    void StreamInput();
    void Redirector();
    //void ProcessEnter();
    void Url();
    void Style();

    wxTextCtrl *m_text;

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

    //SetEditable is suuposed to override wxTE_READONLY
    m_text->SetEditable(true);

    sim.Text("abcdef");
    wxYield();

    CPPUNIT_ASSERT_EQUAL("abcdef", m_text->GetValue());
    CPPUNIT_ASSERT_EQUAL(6, frame->GetEventCount());

    delete m_text;
    m_text = new wxTextCtrl(wxTheApp->GetTopWindow(), wxID_ANY);
}

void TextCtrlTestCase::MaxLength()
{
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
}

void TextCtrlTestCase::StreamInput()
{
    *m_text << "stringinput"
            << 10
            << 1000L
            << 3.14f
            << 2.71
            << 'a'
            << L'b';

    CPPUNIT_ASSERT_EQUAL("stringinput1010003.142.71ab", m_text->GetValue());

    m_text->SetValue("");

#if wxHAS_TEXT_WINDOW_STREAM 

    std::ostream stream(m_text);

    //We don't test a wide character as this is not a wide stream
    stream << "stringinput"
           << 10
           << 1000L
           << 3.14f
           << 2.71
           << 'a';

    stream.flush();

    CPPUNIT_ASSERT_EQUAL("stringinput1010003.142.71a", m_text->GetValue());

#endif
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
}
#endif

void TextCtrlTestCase::Url()
{
#if defined(__WXMSW__) || defined(__WXGTK__)
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
    delete m_text;
    //We need wxTE_RICH under windows for style support
    m_text = new wxTextCtrl(wxTheApp->GetTopWindow(), wxID_ANY, "",
                            wxDefaultPosition, wxDefaultSize, wxTE_RICH);

    //Red text on a white background
    m_text->SetDefaultStyle(wxTextAttr(*wxRED, *wxWHITE));

    CPPUNIT_ASSERT_EQUAL(m_text->GetDefaultStyle().GetTextColour().GetRGB(), 
                         wxColour(*wxRED).GetRGB());

    CPPUNIT_ASSERT_EQUAL(m_text->GetDefaultStyle().GetBackgroundColour().GetRGB(), 
                         wxColour(*wxWHITE).GetRGB());

    m_text->AppendText("red on white ");

    //Red text on a grey backgroud
    m_text->SetDefaultStyle(wxTextAttr(wxNullColour, *wxLIGHT_GREY));

    CPPUNIT_ASSERT_EQUAL(m_text->GetDefaultStyle().GetTextColour().GetRGB(), 
                         wxColour(*wxRED).GetRGB());

    CPPUNIT_ASSERT_EQUAL(m_text->GetDefaultStyle().GetBackgroundColour().GetRGB(), 
                         wxColour(*wxLIGHT_GREY).GetRGB());

    m_text->AppendText("red on grey ");

    //Blue text on a grey backgroud
    m_text->SetDefaultStyle(wxTextAttr(*wxBLUE));


    CPPUNIT_ASSERT_EQUAL(m_text->GetDefaultStyle().GetTextColour().GetRGB(), 
                         wxColour(*wxBLUE).GetRGB());

    CPPUNIT_ASSERT_EQUAL(m_text->GetDefaultStyle().GetBackgroundColour().GetRGB(), 
                         wxColour(*wxLIGHT_GREY).GetRGB());

    m_text->AppendText("blue on grey");

    //Get getting the style at a specific location
    wxTextAttr style;

    //We have to check that styles are supported
    if(m_text->GetStyle(3, style))
    {
        CPPUNIT_ASSERT_EQUAL(style.GetTextColour().GetRGB(), 
                             wxColour(*wxRED).GetRGB());

        CPPUNIT_ASSERT_EQUAL(style.GetBackgroundColour().GetRGB(), 
                             wxColour(*wxWHITE).GetRGB());
    }

    //And then setting the style
    if(m_text->SetStyle(15, 18, style))
    {
        m_text->GetStyle(17, style);

        CPPUNIT_ASSERT_EQUAL(style.GetTextColour().GetRGB(), 
                             wxColour(*wxRED).GetRGB());

        CPPUNIT_ASSERT_EQUAL(style.GetBackgroundColour().GetRGB(), 
                             wxColour(*wxWHITE).GetRGB());
    }
}
