/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/native/textctrl.h
// Purpose:     wxMobile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_NATIVE_TEXTCTRL_H_
#define _WX_MOBILE_NATIVE_TEXTCTRL_H_

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
    wxMoTextCtrl();

    /// Constructor.
    wxMoTextCtrl(wxWindow *parent,
                 wxWindowID id,
                 const wxString& value = wxEmptyString,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxValidator& validator = wxDefaultValidator,
                 const wxString& name = wxTextCtrlNameStr);
    
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxTextCtrlNameStr);
    
protected:
    void Init();

private:

    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMoTextCtrl)
};

#endif
    // _WX_MOBILE_NATIVE_TEXTCTRL_H_
