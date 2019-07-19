///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/searchctrltest.cpp
// Purpose:     wxSearchCtrl unit test
// Author:      Vadim Zeitlin
// Created:     2013-01-20
// Copyright:   (c) 2013 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_SEARCHCTRL

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

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
}

#endif // wxUSE_SEARCHCTRL
