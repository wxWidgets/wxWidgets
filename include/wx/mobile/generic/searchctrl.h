/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/generic/searchctrl.h
// Purpose:     wxMoSearchCtrl class
// Author:      Julian Smart
// Modified by:
// Created:     16/05/09
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWidgets Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_GENERIC_SEARCHCTRL_H_
#define _WX_MOBILE_GENERIC_SEARCHCTRL_H_

#include "wx/srchctrl.h"

/**
    @class wxMoSearchCtrl

    A search control.

    @category{wxMobile}
*/

class WXDLLEXPORT wxMoSearchCtrl: public wxSearchCtrl
{
public:
    /// Default constructor.
    wxMoSearchCtrl() { }

    /// Constructor.
    wxMoSearchCtrl(wxWindow *parent, wxWindowID id,
               const wxString& value = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxTextCtrlNameStr)
    {
        Init();

        Create(parent, id, value, pos, size, style, validator, name);
    }

private:

    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMoSearchCtrl)
};

#endif
    // _WX_MOBILE_GENERIC_SEARCHCTRL_H_
