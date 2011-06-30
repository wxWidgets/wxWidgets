/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/genertic/mo_textctrl_n.mm
// Purpose:     wxMoTextCtrl class
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

#include "wx/mobile/native/textctrl.h"

IMPLEMENT_DYNAMIC_CLASS(wxMoTextCtrl, wxTextCtrl)

/// Default constructor.
wxMoTextCtrl::wxMoTextCtrl()
{
    Init();
}

void wxMoTextCtrl::Init()
{
    
}

/// Constructor.
wxMoTextCtrl::wxMoTextCtrl(wxWindow *parent,
                           wxWindowID id,
                           const wxString& value,
                           const wxPoint& pos,
                           const wxSize& size,
                           long style,
                           const wxValidator& validator,
                           const wxString& name)
{
    Init();
    
    Create(parent, id, value, pos, size, style, validator, name);
}

bool wxMoTextCtrl::Create(wxWindow *parent,
                          wxWindowID id,
                          const wxString& value,
                          const wxPoint& pos,
                          const wxSize& size,
                          long style,
                          const wxValidator& validator,
                          const wxString& name)
{
    return wxTextCtrl::Create(parent, id, value, pos, size, style, validator, name);
}
