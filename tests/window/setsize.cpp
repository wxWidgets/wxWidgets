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
    #include "wx/frame.h"
    #include "wx/window.h"
#endif // WX_PRECOMP

#include "wx/scopedptr.h"
#include "wx/stopwatch.h"

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

// Class used to check if we received the (first) paint event.
class WaitForPaint
{
public:
    // Note that we have to use a pointer here, i.e. we can't just store the
    // flag inside the class itself because it's going to be cloned inside wx
    // and querying the flag of the original copy is not going to work.
    explicit WaitForPaint(bool* painted)
        : m_painted(*painted)
    {
        m_painted = false;
    }

    void operator()(wxPaintEvent& event)
    {
        event.Skip();
        m_painted = true;
    }

private:
    bool& m_painted;
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

TEST_CASE("wxWindow::MovePreservesSize", "[window][size][move]")
{
    wxScopedPtr<wxWindow>
        w(new wxFrame(wxTheApp->GetTopWindow(), wxID_ANY, "Test child frame"));

    // Unfortunately showing the window is asynchronous, at least when using
    // X11, so we have to wait for some time before retrieving its true
    // geometry. And it's not clear how long should we wait, so we do it until
    // we get the first paint event -- by then the window really should have
    // its final size.
    bool painted;
    WaitForPaint waitForPaint(&painted);
    w->Bind(wxEVT_PAINT, waitForPaint);

    w->Show();

    wxStopWatch sw;
    while ( !painted )
    {
        wxYield();

        if ( sw.Time() > 250 )
        {
            WARN("Didn't get a paint event until timeout expiration");
            break;
        }
    }

    const wxRect rectOrig = w->GetRect();

    // Check that moving the window doesn't change its size.
    w->Move(rectOrig.GetPosition() + wxPoint(100, 100));
    CHECK( w->GetSize() == rectOrig.GetSize() );
}
