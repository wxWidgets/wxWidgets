///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/searchctrltest.cpp
// Purpose:     wxSearchCtrl unit test
// Author:      Vadim Zeitlin
// Created:     2013-01-20
// Copyright:   (c) 2013 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_SEARCHCTRL


#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/srchctrl.h"

#include "testwindow.h"

class SearchCtrlTestCase
{
public:
    SearchCtrlTestCase()
        : m_search(new wxSearchCtrl(wxTheApp->GetTopWindow(), wxID_ANY))
    {
    }

    ~SearchCtrlTestCase()
    {
        delete m_search;
    }

    void CheckStringSelection(const char *sel)
    {
        wxTextEntry * const entry = m_search;
        CHECK( sel == entry->GetStringSelection() );
    }

    void AssertSelection(int from, int to, const char *sel)
    {
        wxTextEntry * const entry = m_search;

        CHECK( entry->HasSelection() );

        long fromReal,
             toReal;
        entry->GetSelection(&fromReal, &toReal);
        CHECK( from == fromReal );
        CHECK( to == toReal );

        CHECK( from == entry->GetInsertionPoint() );

        CheckStringSelection(sel);
    }

protected:
    wxSearchCtrl* const m_search;
};

#define SEARCH_CTRL_TEST_CASE(name, tags) \
    TEST_CASE_METHOD(SearchCtrlTestCase, name, tags)

// TODO OS X test only passes when run solo ...
#ifndef __WXOSX__
SEARCH_CTRL_TEST_CASE("wxSearchCtrl::Focus", "[wxSearchCtrl][focus]")
{
    m_search->SetFocus();
    CHECK_FOCUS_IS( m_search );
}
#endif // !__WXOSX__

SEARCH_CTRL_TEST_CASE("wxSearchCtrl::ChangeValue", "[wxSearchCtrl][text]")
{
    CHECK( m_search->GetValue() == wxString() );

    m_search->ChangeValue("foo");
    CHECK( m_search->GetValue() == "foo" );

    m_search->Clear();
    CHECK( m_search->GetValue() == "" );
}

SEARCH_CTRL_TEST_CASE("wxSearchCtrl::SetValue", "[wxSearchCtrl][set_value]")
{
  // Work around bug with hint implementation in wxGTK2.
#if defined(__WXGTK__) && !defined(__WXGTK3__)
  m_search->Clear();
#endif
  CHECK( m_search->IsEmpty() );

  m_search->SetValue("foo");
  CHECK( m_search->GetValue() == "foo" );

  m_search->SetValue("");
  CHECK( m_search->IsEmpty() );

  m_search->SetValue("hi");
  CHECK( "hi" ==  m_search->GetValue() );

  m_search->SetValue("bye");
  CHECK( "bye" == m_search->GetValue() );
}

SEARCH_CTRL_TEST_CASE("wxSearchCtrl::Selection", "[wxSearchCtrl][selection]")
{
  wxTextEntry * const entry = m_search;

  entry->SetValue("0123456789");

  entry->SetSelection(2, 4);
  AssertSelection(2, 4, "23"); // not "234"!

  entry->SetSelection(3, -1);
  AssertSelection(3, 10, "3456789");

  entry->SelectAll();
  AssertSelection(0, 10, "0123456789");

  entry->SetSelection(0, 0);
  CHECK( !entry->HasSelection() );
}

#endif // wxUSE_SEARCHCTRL
