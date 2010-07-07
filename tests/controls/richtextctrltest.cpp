///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/richtextctrltest.cpp
// Purpose:     wxRichTextCtrl unit test
// Author:      Steven Lamerton
// Created:     2010-07-07
// RCS-ID:      $Id$
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/richtext/richtextctrl.h"
#include "wx/richtext/richtextstyles.h"
#include "testableframe.h"
#include "wx/uiaction.h"

class RichTextCtrlTestCase : public CppUnit::TestCase
{
public:
    RichTextCtrlTestCase() { }

    void setUp();
    void tearDown();

private:
    CPPUNIT_TEST_SUITE( RichTextCtrlTestCase );
        CPPUNIT_TEST( Character );
        CPPUNIT_TEST( Delete );
        CPPUNIT_TEST( Return );
        CPPUNIT_TEST( Style );
        CPPUNIT_TEST( BufferReset );
        CPPUNIT_TEST( Url );
        CPPUNIT_TEST( Text );
    CPPUNIT_TEST_SUITE_END();

    void Character();
    void Delete();
    void Return();
    void Style();
    void BufferReset();
    void Url();
    void Text();

    wxRichTextCtrl* m_rich;

    DECLARE_NO_COPY_CLASS(RichTextCtrlTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( RichTextCtrlTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( RichTextCtrlTestCase, "RichTextCtrlTestCase" );

void RichTextCtrlTestCase::setUp()
{
    m_rich = new wxRichTextCtrl(wxTheApp->GetTopWindow(), wxID_ANY, "",
                                wxDefaultPosition, wxSize(400, 200));
}

void RichTextCtrlTestCase::tearDown()
{
    wxDELETE(m_rich);
}

void RichTextCtrlTestCase::Character()
{
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_rich, wxEVT_COMMAND_RICHTEXT_CHARACTER);
    EventCounter count1(m_rich, wxEVT_COMMAND_RICHTEXT_CONTENT_INSERTED);

    m_rich->SetFocus();

    wxUIActionSimulator sim;
    sim.Text("abcdef");
    wxYield();

    CPPUNIT_ASSERT_EQUAL(6, frame->GetEventCount(wxEVT_COMMAND_RICHTEXT_CHARACTER));
    CPPUNIT_ASSERT_EQUAL(6, frame->GetEventCount(wxEVT_COMMAND_RICHTEXT_CONTENT_INSERTED));

    //As these are not characters they shouldn't count
    sim.Char(WXK_RETURN);
    sim.Char(WXK_SHIFT);
    wxYield();

    CPPUNIT_ASSERT_EQUAL(0, frame->GetEventCount(wxEVT_COMMAND_RICHTEXT_CHARACTER));
    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_COMMAND_RICHTEXT_CONTENT_INSERTED));
}

void RichTextCtrlTestCase::Delete()
{
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_rich, wxEVT_COMMAND_RICHTEXT_DELETE);
    EventCounter count1(m_rich, wxEVT_COMMAND_RICHTEXT_CONTENT_DELETED);

    m_rich->SetFocus();

    wxUIActionSimulator sim;
    sim.Text("abcdef");
    sim.Char(WXK_BACK);
    sim.Char(WXK_DELETE);
    wxYield();

    CPPUNIT_ASSERT_EQUAL(2, frame->GetEventCount(wxEVT_COMMAND_RICHTEXT_DELETE));
    //Only one as the delete doesn't delete anthing
    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_COMMAND_RICHTEXT_CONTENT_DELETED)); 
}

void RichTextCtrlTestCase::Return()
{
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_rich, wxEVT_COMMAND_RICHTEXT_RETURN);

    m_rich->SetFocus();

    wxUIActionSimulator sim;
    sim.Char(WXK_RETURN);
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount());
}

void RichTextCtrlTestCase::Style()
{
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_rich, wxEVT_COMMAND_RICHTEXT_STYLE_CHANGED);

    m_rich->SetValue("Sometext");
    m_rich->SetStyle(0, 8, wxTextAttr(*wxRED, *wxWHITE));

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_COMMAND_RICHTEXT_STYLE_CHANGED));
}

void RichTextCtrlTestCase::BufferReset()
{
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_rich, wxEVT_COMMAND_RICHTEXT_BUFFER_RESET);

    m_rich->AppendText("more text!");
    m_rich->SetValue("");

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount());

    m_rich->AppendText("more text!");
    m_rich->Clear();

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount());

    //We expect a buffer reset here as setvalue clears the existing text
    m_rich->SetValue("replace");
    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount());
}

void RichTextCtrlTestCase::Url()
{
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_rich, wxEVT_COMMAND_TEXT_URL);

    m_rich->BeginURL("http://www.wxwidgets.org");
    m_rich->WriteText("http://www.wxwidgets.org");
    m_rich->EndURL();

    wxUIActionSimulator sim;
    sim.MouseMove(m_rich->ClientToScreen(wxPoint(5, 5)));
    sim.MouseClick();
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount());
}

void RichTextCtrlTestCase::Text()
{
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_rich, wxEVT_COMMAND_TEXT_UPDATED);

    m_rich->SetFocus();

    wxUIActionSimulator sim;
    sim.Text("abcdef");
    wxYield();

    CPPUNIT_ASSERT_EQUAL("abcdef", m_rich->GetValue());
    CPPUNIT_ASSERT_EQUAL(6, frame->GetEventCount());
}
