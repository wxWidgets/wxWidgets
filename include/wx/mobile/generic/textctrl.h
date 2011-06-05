/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/generic/textctrl.h
// Purpose:     wxMoTextCtrl class
// Author:      Julian Smart
// Modified by:
// Created:     16/05/09
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_GENERIC_TEXTCTRL_H_
#define _WX_MOBILE_GENERIC_TEXTCTRL_H_

#include "wx/textctrl.h"

/**
    @class wxMoTextCtrl

    A single or multi-line text control.

    @category{wxMobile}
*/

class WXDLLEXPORT wxMoTextCtrl : public wxTextCtrl
{
public:
    /// Default constructor.
    wxMoTextCtrl() { }

    /// Constructor.
    wxMoTextCtrl(wxWindow *parent, wxWindowID id,
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

    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMoTextCtrl)
};

#endif
    // _WX_MOBILE_GENERIC_TEXTCTRL_H_
