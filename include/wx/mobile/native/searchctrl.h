/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/native/searchctrl.h
// Purpose:     wxMobile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_NATIVE_SEARCHCTRL_H_
#define _WX_MOBILE_NATIVE_SEARCHCTRL_H_

// FIXME LV wxUSE_NOTEBOOK is reset to 0 in include/wx/osx/iphone/chkconf.h
// as unimplemented capability.
#undef _WX_SEARCHCTRL_H_BASE_
#undef wxUSE_SEARCHCTRL
#define wxUSE_SEARCHCTRL 1

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
    // _WX_MOBILE_NATIVE_SEARCHCTRL_H_
