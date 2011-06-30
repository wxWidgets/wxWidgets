/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_searchctrl_n.mm
// Purpose:     wxMoSearchCtrl class
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

#include "wx/mobile/native/searchctrl.h"

IMPLEMENT_DYNAMIC_CLASS(wxMoSearchCtrl, wxSearchCtrl)


wxMoSearchCtrl::wxMoSearchCtrl()
{

}

/// Constructor.
wxMoSearchCtrl::wxMoSearchCtrl(wxWindow *parent,
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

bool wxMoSearchCtrl::Create(wxWindow *parent,
                            wxWindowID id,
                            const wxString& value,
                            const wxPoint& pos,
                            const wxSize& size,
                            long style,
                            const wxValidator& validator,
                            const wxString& name)
{
    return wxSearchCtrl::Create(parent, id, value, pos, size, style, validator, name);
}
