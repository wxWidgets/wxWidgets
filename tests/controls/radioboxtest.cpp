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

#include "wx/tooltip.h"

#include <memory>

class RadioBoxTestCase
{
protected:
    RadioBoxTestCase();
    ~RadioBoxTestCase();

    wxRadioBox* m_radio;

    wxDECLARE_NO_COPY_CLASS(RadioBoxTestCase);
};

RadioBoxTestCase::RadioBoxTestCase()
{
    wxArrayString choices;
    choices.push_back("item 0");
    choices.push_back("item 1");
    choices.push_back("item 2");

    m_radio = new wxRadioBox(wxTheApp->GetTopWindow(), wxID_ANY, "RadioBox",
                             wxDefaultPosition, wxDefaultSize, choices);
}

RadioBoxTestCase::~RadioBoxTestCase()
{
    wxTheApp->GetTopWindow()->DestroyChildren();
}

TEST_CASE_METHOD(RadioBoxTestCase, "RadioBox::FindString", "[radiobox][find]")
{
    CHECK( m_radio->FindString("not here") == wxNOT_FOUND );
    CHECK( m_radio->FindString("item 1") == 1 );
    CHECK( m_radio->FindString("ITEM 2") == 2 );
    CHECK( m_radio->FindString("ITEM 2", true) == wxNOT_FOUND );
}

TEST_CASE_METHOD(RadioBoxTestCase, "RadioBox::RowColCount", "[radiobox]")
{
#ifndef __WXGTK__
    wxArrayString choices;
    choices.push_back("item 0");
    choices.push_back("item 1");
    choices.push_back("item 2");

    m_radio = new wxRadioBox(wxTheApp->GetTopWindow(), wxID_ANY, "RadioBox",
                             wxDefaultPosition, wxDefaultSize, choices, 2);

    CHECK( m_radio->GetColumnCount() == 2 );
    CHECK( m_radio->GetRowCount() == 2 );

    m_radio = new wxRadioBox(wxTheApp->GetTopWindow(), wxID_ANY, "RadioBox",
                             wxDefaultPosition, wxDefaultSize, choices, 1,
                             wxRA_SPECIFY_ROWS);

    CHECK( m_radio->GetColumnCount() == 3 );
    CHECK( m_radio->GetRowCount() == 1 );
#endif
}

TEST_CASE_METHOD(RadioBoxTestCase, "RadioBox::Enable", "[radiobox][enable]")
{
#ifndef __WXOSX__
    m_radio->Enable(false);

    CHECK(!m_radio->IsItemEnabled(0));

    m_radio->Enable(1, true);

    CHECK(!m_radio->IsItemEnabled(0));
    CHECK(m_radio->IsItemEnabled(1));
    CHECK(!m_radio->IsItemEnabled(2));

    m_radio->Enable(true);

    CHECK(m_radio->IsItemEnabled(0));
    CHECK(m_radio->IsItemEnabled(1));
    CHECK(m_radio->IsItemEnabled(2));

    m_radio->Enable(0, false);

    CHECK(!m_radio->IsItemEnabled(0));
    CHECK(m_radio->IsItemEnabled(1));
    CHECK(m_radio->IsItemEnabled(2));
#endif
}

TEST_CASE_METHOD(RadioBoxTestCase, "RadioBox::Show", "[radiobox][show]")
{
    m_radio->Show(false);

    CHECK(!m_radio->IsItemShown(0));

    m_radio->Show(1, true);

    CHECK(!m_radio->IsItemShown(0));
    CHECK(m_radio->IsItemShown(1));
    CHECK(!m_radio->IsItemShown(2));

    m_radio->Show(true);

    CHECK(m_radio->IsItemShown(0));
    CHECK(m_radio->IsItemShown(1));
    CHECK(m_radio->IsItemShown(2));

    m_radio->Show(0, false);

    CHECK(!m_radio->IsItemShown(0));
    CHECK(m_radio->IsItemShown(1));
    CHECK(m_radio->IsItemShown(2));
}

TEST_CASE_METHOD(RadioBoxTestCase, "RadioBox::HelpText", "[radiobox][help]")
{
    CHECK( m_radio->GetItemHelpText(0) == wxEmptyString );

    m_radio->SetItemHelpText(1, "Item 1 help");

    CHECK( m_radio->GetItemHelpText(1) == "Item 1 help" );

    m_radio->SetItemHelpText(1, "");

    CHECK( m_radio->GetItemHelpText(1) == wxEmptyString );
}

TEST_CASE_METHOD(RadioBoxTestCase, "RadioBox::ToolTip", "[radiobox][tooltip]")
{
#if defined (__WXMSW__) || defined(__WXGTK__)
    //GetItemToolTip returns null if there is no tooltip set
    CHECK(!m_radio->GetItemToolTip(0));

    m_radio->SetItemToolTip(1, "Item 1 help");

    CHECK( m_radio->GetItemToolTip(1)->GetTip() == "Item 1 help" );

    m_radio->SetItemToolTip(1, "");

    //However if we set a blank tip this does count as a tooltip
    CHECK(!m_radio->GetItemToolTip(1));
#endif
}

TEST_CASE_METHOD(RadioBoxTestCase, "RadioBox::Selection", "[radiobox][selection]")
{
    //Until other item containers the first item is selected by default
    CHECK( m_radio->GetSelection() == 0 );
    CHECK( m_radio->GetStringSelection() == "item 0" );

    m_radio->SetSelection(1);

    CHECK( m_radio->GetSelection() == 1 );
    CHECK( m_radio->GetStringSelection() == "item 1" );

    m_radio->SetStringSelection("item 2");

    CHECK( m_radio->GetSelection() == 2 );
    CHECK( m_radio->GetStringSelection() == "item 2" );
}

TEST_CASE_METHOD(RadioBoxTestCase, "RadioBox::Count", "[radiobox]")
{
    //A trivial test for the item count as items can neither
    //be added or removed
    CHECK( m_radio->GetCount() == 3 );
    CHECK(!m_radio->IsEmpty());
}

TEST_CASE_METHOD(RadioBoxTestCase, "RadioBox::SetString", "[radiobox]")
{
    m_radio->SetString(0, "new item 0");
    m_radio->SetString(2, "");

    CHECK( m_radio->GetString(0) == "new item 0" );
    CHECK( m_radio->GetString(2) == "" );
}

TEST_CASE("RadioBox::NoItems", "[radiobox]")
{
    std::unique_ptr<wxRadioBox>
        radio(new wxRadioBox(wxTheApp->GetTopWindow(), wxID_ANY, "Empty",
                             wxDefaultPosition, wxDefaultSize,
                             0, nullptr,
                             1, wxRA_SPECIFY_COLS));

    CHECK( radio->GetCount() == 0 );
    CHECK( radio->IsEmpty() );

    CHECK_NOTHROW( radio->SetFocus() );
}

#endif // wxUSE_RADIOBOX
