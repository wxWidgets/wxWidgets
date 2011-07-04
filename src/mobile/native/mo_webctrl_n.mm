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

extern WXDLLEXPORT_DATA(const wxChar) wxWebCtrlNameStr[] = wxT("WebCtrl");

IMPLEMENT_DYNAMIC_CLASS(wxMoWebCtrl, wxWebKitCtrl)

BEGIN_EVENT_TABLE(wxMoWebCtrl, wxWebKitCtrl)
    EVT_SIZE(wxMoWebCtrl::OnSize)
END_EVENT_TABLE()

/// Default constructor.
wxMoWebCtrl::wxMoWebCtrl()
{
    Init();
}

/// Constructor.
wxMoWebCtrl::wxMoWebCtrl(wxWindow *parent,
                         wxWindowID id,
                         const wxPoint& pos,
                         const wxSize& size,
                         long style,
                         const wxValidator& validator,
                         const wxString& name)
{
    Init();
    Create(parent, id, pos, size, style, validator, name);
}

bool wxMoWebCtrl::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxValidator& validator,
                      const wxString& name)
{
    return wxWebKitCtrl::Create(parent, id, wxEmptyString, pos, size, style, validator, name);
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
