///////////////////////////////////////////////////////////////////////////////
// Name:        tests/window/setsize.cpp
// Purpose:     Tests for SetSize() and related wxWindow methods
// Author:      Vadim Zeitlin
// Created:     2008-05-25
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
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
    #include "wx/window.h"
#endif // WX_PRECOMP

#include "wx/scopedptr.h"

#include "asserthelper.h"

// ----------------------------------------------------------------------------
// tests helpers
// ----------------------------------------------------------------------------

namespace
{

// Helper class overriding DoGetBestSize() for testing purposes.
class MyWindow : public wxWindow
{
public:
    MyWindow(wxWindow* parent)
        : wxWindow(parent, wxID_ANY)
    {
    }

protected:
    virtual wxSize DoGetBestSize() const wxOVERRIDE { return wxSize(50, 250); }
};

} // anonymous namespace

// ----------------------------------------------------------------------------
// tests themselves
// ----------------------------------------------------------------------------

TEST_CASE("wxWindow::SetSize", "[window][size]")
{
    wxScopedPtr<wxWindow> w(new MyWindow(wxTheApp->GetTopWindow()));

    SECTION("Simple")
    {
        const wxSize size(127, 35);
        w->SetSize(size);
        CHECK( size == w->GetSize() );
    }

    SECTION("With min size")
    {
        w->SetMinSize(wxSize(100, 100));

        const wxSize size(200, 50);
        w->SetSize(size);
        CHECK( size == w->GetSize() );
    }
}

TEST_CASE("wxWindow::GetBestSize", "[window][size][best-size]")
{
    wxScopedPtr<wxWindow> w(new MyWindow(wxTheApp->GetTopWindow()));

    CHECK( wxSize(50, 250) == w->GetBestSize() );

    w->SetMinSize(wxSize(100, 100));
    CHECK( wxSize(100, 250) == w->GetBestSize() );

    w->SetMaxSize(wxSize(200, 200));
    CHECK( wxSize(100, 200) == w->GetBestSize() );
}
