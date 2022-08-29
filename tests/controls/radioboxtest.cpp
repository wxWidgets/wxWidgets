///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/radioboxtest.cpp
// Purpose:     wxRadioBox unit test
// Author:      Steven Lamerton
// Created:     2010-07-14
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_RADIOBOX


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/radiobox.h"
#endif // WX_PRECOMP

#include "wx/scopedptr.h"
#include "wx/tooltip.h"

class RadioBoxTestCase : public CppUnit::TestCase
{
public:
    RadioBoxTestCase() { }

    void setUp() wxOVERRIDE;
    void tearDown() wxOVERRIDE;

private:
    CPPUNIT_TEST_SUITE( RadioBoxTestCase );
        CPPUNIT_TEST( FindString );
        CPPUNIT_TEST( RowColCount );
        CPPUNIT_TEST( Enable );
        CPPUNIT_TEST( Show );
        CPPUNIT_TEST( HelpText );
        CPPUNIT_TEST( ToolTip );
        CPPUNIT_TEST( Selection );
        CPPUNIT_TEST( Count );
        CPPUNIT_TEST( SetString );
    CPPUNIT_TEST_SUITE_END();

    void FindString();
    void RowColCount();
    void Enable();
    void Show();
    void HelpText();
    void ToolTip();
    void Selection();
    void Count();
    void SetString();

    wxRadioBox* m_radio;

    wxDECLARE_NO_COPY_CLASS(RadioBoxTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( RadioBoxTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( RadioBoxTestCase, "RadioBoxTestCase" );

void RadioBoxTestCase::setUp()
{
    wxArrayString choices;
    choices.push_back("item 0");
    choices.push_back("item 1");
    choices.push_back("item 2");

    m_radio = new wxRadioBox(wxTheApp->GetTopWindow(), wxID_ANY, "RadioBox",
                             wxDefaultPosition, wxDefaultSize, choices);
}

void RadioBoxTestCase::tearDown()
{
    wxTheApp->GetTopWindow()->DestroyChildren();
}

void RadioBoxTestCase::FindString()
{
    CPPUNIT_ASSERT_EQUAL(wxNOT_FOUND, m_radio->FindString("not here"));
    CPPUNIT_ASSERT_EQUAL(1, m_radio->FindString("item 1"));
    CPPUNIT_ASSERT_EQUAL(2, m_radio->FindString("ITEM 2"));
    CPPUNIT_ASSERT_EQUAL(wxNOT_FOUND, m_radio->FindString("ITEM 2", true));
}

void RadioBoxTestCase::RowColCount()
{
#ifndef __WXGTK__
    wxArrayString choices;
    choices.push_back("item 0");
    choices.push_back("item 1");
    choices.push_back("item 2");

    m_radio = new wxRadioBox(wxTheApp->GetTopWindow(), wxID_ANY, "RadioBox",
                             wxDefaultPosition, wxDefaultSize, choices, 2);

    CPPUNIT_ASSERT_EQUAL(2, m_radio->GetColumnCount());
    CPPUNIT_ASSERT_EQUAL(2, m_radio->GetRowCount());

    m_radio = new wxRadioBox(wxTheApp->GetTopWindow(), wxID_ANY, "RadioBox",
                             wxDefaultPosition, wxDefaultSize, choices, 1,
                             wxRA_SPECIFY_ROWS);

    CPPUNIT_ASSERT_EQUAL(3, m_radio->GetColumnCount());
    CPPUNIT_ASSERT_EQUAL(1, m_radio->GetRowCount());
#endif
}

void RadioBoxTestCase::Enable()
{
#ifndef __WXOSX__
    m_radio->Enable(false);

    CPPUNIT_ASSERT(!m_radio->IsItemEnabled(0));

    m_radio->Enable(1, true);

    CPPUNIT_ASSERT(!m_radio->IsItemEnabled(0));
    CPPUNIT_ASSERT(m_radio->IsItemEnabled(1));
    CPPUNIT_ASSERT(!m_radio->IsItemEnabled(2));

    m_radio->Enable(true);

    CPPUNIT_ASSERT(m_radio->IsItemEnabled(0));
    CPPUNIT_ASSERT(m_radio->IsItemEnabled(1));
    CPPUNIT_ASSERT(m_radio->IsItemEnabled(2));

    m_radio->Enable(0, false);

    CPPUNIT_ASSERT(!m_radio->IsItemEnabled(0));
    CPPUNIT_ASSERT(m_radio->IsItemEnabled(1));
    CPPUNIT_ASSERT(m_radio->IsItemEnabled(2));
#endif
}

void RadioBoxTestCase::Show()
{
    m_radio->Show(false);

    CPPUNIT_ASSERT(!m_radio->IsItemShown(0));

    m_radio->Show(1, true);

    CPPUNIT_ASSERT(!m_radio->IsItemShown(0));
    CPPUNIT_ASSERT(m_radio->IsItemShown(1));
    CPPUNIT_ASSERT(!m_radio->IsItemShown(2));

    m_radio->Show(true);

    CPPUNIT_ASSERT(m_radio->IsItemShown(0));
    CPPUNIT_ASSERT(m_radio->IsItemShown(1));
    CPPUNIT_ASSERT(m_radio->IsItemShown(2));

    m_radio->Show(0, false);

    CPPUNIT_ASSERT(!m_radio->IsItemShown(0));
    CPPUNIT_ASSERT(m_radio->IsItemShown(1));
    CPPUNIT_ASSERT(m_radio->IsItemShown(2));
}

void RadioBoxTestCase::HelpText()
{
    CPPUNIT_ASSERT_EQUAL(wxEmptyString, m_radio->GetItemHelpText(0));

    m_radio->SetItemHelpText(1, "Item 1 help");

    CPPUNIT_ASSERT_EQUAL("Item 1 help", m_radio->GetItemHelpText(1));

    m_radio->SetItemHelpText(1, "");

    CPPUNIT_ASSERT_EQUAL(wxEmptyString, m_radio->GetItemHelpText(1));
}

void RadioBoxTestCase::ToolTip()
{
#if defined (__WXMSW__) || defined(__WXGTK__)
    //GetItemToolTip returns null if there is no tooltip set
    CPPUNIT_ASSERT(!m_radio->GetItemToolTip(0));

    m_radio->SetItemToolTip(1, "Item 1 help");

    CPPUNIT_ASSERT_EQUAL("Item 1 help", m_radio->GetItemToolTip(1)->GetTip());

    m_radio->SetItemToolTip(1, "");

    //However if we set a blank tip this does count as a tooltip
    CPPUNIT_ASSERT(!m_radio->GetItemToolTip(1));
#endif
}

void RadioBoxTestCase::Selection()
{
    //Until other item containers the first item is selected by default
    CPPUNIT_ASSERT_EQUAL(0, m_radio->GetSelection());
    CPPUNIT_ASSERT_EQUAL("item 0", m_radio->GetStringSelection());

    m_radio->SetSelection(1);

    CPPUNIT_ASSERT_EQUAL(1, m_radio->GetSelection());
    CPPUNIT_ASSERT_EQUAL("item 1", m_radio->GetStringSelection());

    m_radio->SetStringSelection("item 2");

    CPPUNIT_ASSERT_EQUAL(2, m_radio->GetSelection());
    CPPUNIT_ASSERT_EQUAL("item 2", m_radio->GetStringSelection());
}

void RadioBoxTestCase::Count()
{
    //A trivial test for the item count as items can neither
    //be added or removed
    CPPUNIT_ASSERT_EQUAL(3, m_radio->GetCount());
    CPPUNIT_ASSERT(!m_radio->IsEmpty());
}

void RadioBoxTestCase::SetString()
{
    m_radio->SetString(0, "new item 0");
    m_radio->SetString(2, "");

    CPPUNIT_ASSERT_EQUAL("new item 0", m_radio->GetString(0));
    CPPUNIT_ASSERT_EQUAL("", m_radio->GetString(2));
}

TEST_CASE("RadioBox::NoItems", "[radiobox]")
{
    wxScopedPtr<wxRadioBox>
        radio(new wxRadioBox(wxTheApp->GetTopWindow(), wxID_ANY, "Empty",
                             wxDefaultPosition, wxDefaultSize,
                             0, NULL,
                             1, wxRA_SPECIFY_COLS));

    CHECK( radio->GetCount() == 0 );
    CHECK( radio->IsEmpty() );

    CHECK_NOTHROW( radio->SetFocus() );
}

#endif // wxUSE_RADIOBOX
