/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_webctrl_n.mm
// Purpose:     wxMoWebCtrl class
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/dcclient.h"

#include "wx/mobile/native/webctrl.h"
#include "wx/mobile/native/utils.h"

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
	// FIXME stub

    return true;
}

wxMoWebCtrl::~wxMoWebCtrl()
{
}

void wxMoWebCtrl::Init()
{
    // FIXME stub
}

wxSize wxMoWebCtrl::DoGetBestSize() const
{
    // FIXME stub

	wxSize empty(1, 1);
	return empty;
}

bool wxMoWebCtrl::SetBackgroundColour(const wxColour &colour)
{
    // FIXME stub

	return true;
}

bool wxMoWebCtrl::SetForegroundColour(const wxColour &colour)
{
    // FIXME stub

	return true;
}

bool wxMoWebCtrl::Enable(bool enable)
{
    // FIXME stub

    return true;
}

bool wxMoWebCtrl::SetFont(const wxFont& font)
{
    // FIXME stub

    return true;
}

void wxMoWebCtrl::OnSize(wxSizeEvent& WXUNUSED(event))
{
    // FIXME stub
}

bool wxMoWebCtrl::LoadURL(const wxString& url)
{
	// FIXME stub
	return true;
}

bool wxMoWebCtrl::CanGoBack()
{
	// FIXME stub
	return true;
}

bool wxMoWebCtrl::CanGoForward()
{
	// FIXME stub
	return true;
}

bool wxMoWebCtrl::GoBack()
{
	// FIXME stub
	return true;
}

bool wxMoWebCtrl::GoForward()
{
	// FIXME stub
	return true;
}
