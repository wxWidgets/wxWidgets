/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_webctrl_g.cpp
// Purpose:     wxMoWebCtrl class
// Author:      Julian Smart
// Modified by:
// Created:     16/05/09
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/dcclient.h"

#include "wx/mobile/generic/webctrl.h"
#include "wx/mobile/generic/utils.h"

#include "wx/arrimpl.cpp"

#ifdef __WXMSW__
#include "wx/mobile/web/iehtmlwin.h"
#endif

extern WXDLLEXPORT_DATA(const wxChar) wxWebCtrlNameStr[] = wxT("WebCtrl");

IMPLEMENT_DYNAMIC_CLASS(wxMoWebCtrl, wxControl)

BEGIN_EVENT_TABLE(wxMoWebCtrl, wxControl)
    EVT_SIZE(wxMoWebCtrl::OnSize)
END_EVENT_TABLE()

bool wxMoWebCtrl::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxValidator& validator,
                      const wxString& name)
{
#ifdef __WXMSW__
    // Can't get rid of border in web control, so eliminate at least in the container.
    style &= ~wxBORDER_MASK;
    style |= wxBORDER_NONE;
#endif

    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    SetBackgroundColour(wxColour(200, 200, 200));
    SetForegroundColour(*wxWHITE);

#ifdef __WXMSW__
    m_htmlWindow = new wxIEHtmlWin(this, wxID_ANY, wxDefaultPosition,
        wxDefaultSize, wxBORDER_NONE|wxWANTS_CHARS);
#elif defined(__WXMAC__) && wxUSE_WEBKIT
    m_htmlWindow = new wxWebKitCtrl(this, wxID_ANY, wxDefaultPosition,
        wxDefaultSize, wxBORDER_NONE);
#else
    m_htmlWindow = new wxHtmlWindow(this, wxID_ANY, wxDefaultPosition,
        wxDefaultSize, wxBORDER_NONE);
#endif
    
    SetInitialSize(size);

    m_htmlWindow->SetSize(GetClientSize());

    return true;
}

wxMoWebCtrl::~wxMoWebCtrl()
{
}

void wxMoWebCtrl::Init()
{
    m_htmlWindow = NULL;
    m_userScaling = false;
}

wxSize wxMoWebCtrl::DoGetBestSize() const
{
    int width = 100;
    int height = 100;
    
    wxSize best(width, height);
    CacheBestSize(best);
    return best;
}

bool wxMoWebCtrl::SetBackgroundColour(const wxColour &colour)
{
    if ( !wxControl::SetBackgroundColour(colour) )
    {
        // nothing to do
        return false;
    }

    Refresh();

    return true;
}

bool wxMoWebCtrl::SetForegroundColour(const wxColour &colour)
{
    if ( !wxControl::SetForegroundColour(colour) )
    {
        // nothing to do
        return false;
    }

    Refresh();

    return true;
}

bool wxMoWebCtrl::Enable(bool enable)
{
    wxControl::Enable(enable);
    Refresh();
    return true;
}

bool wxMoWebCtrl::SetFont(const wxFont& font)
{
    wxControl::SetFont(font);

    Refresh();

    return true;
}

void wxMoWebCtrl::OnSize(wxSizeEvent& WXUNUSED(event))
{
    if (m_htmlWindow)
    {
        m_htmlWindow->SetSize(GetClientSize());
    }
}

bool wxMoWebCtrl::LoadURL(const wxString& url)
{
#ifdef __WXMSW__
    m_htmlWindow->LoadUrl(url);
    return true;
#elif defined(__WXMAC__) && wxUSE_WEBKIT
    return m_htmlWindow->LoadURL(url);
#else
    return m_htmlWindow->LoadPage(url);
#endif
}

bool wxMoWebCtrl::CanGoBack()
{
#ifdef __WXMSW__
    return true;
#elif defined(__WXMAC__) && wxUSE_WEBKIT
    return m_htmlWindow->CanGoBack();
#else
    return m_htmlWindow->HistoryCanBack();
#endif
}

bool wxMoWebCtrl::CanGoForward()
{
#ifdef __WXMSW__
    return true;
#elif defined(__WXMAC__) && wxUSE_WEBKIT
    return m_htmlWindow->CanGoForward();
#else
    return m_htmlWindow->HistoryCanForward();
#endif
}

bool wxMoWebCtrl::GoBack()
{
#ifdef __WXMSW__
    return m_htmlWindow->GoBack();
#elif defined(__WXMAC__) && wxUSE_WEBKIT
    return m_htmlWindow->GoBack();
#else
    return m_htmlWindow->HistoryBack();
#endif
}

bool wxMoWebCtrl::GoForward()
{
#ifdef __WXMSW__
    return m_htmlWindow->GoForward();
#elif defined(__WXMAC__) && wxUSE_WEBKIT
    return m_htmlWindow->GoForward();
#else
    return m_htmlWindow->HistoryForward();
#endif
}
