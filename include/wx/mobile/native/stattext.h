/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/native/stattext.h
// Purpose:     wxMobile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_NATIVE_STATTEXT_H_
#define _WX_MOBILE_NATIVE_STATTEXT_H_

#include "wx/stattext.h"

/**
 @class wxMoStaticText
 
 A control showing a text string.
 
 @category{wxMobile}
 */

class WXDLLEXPORT wxMoStaticText : public wxStaticText
{
public:
    /// Default constructor.
    wxMoStaticText() { }
    
    /// Constructor.
    wxMoStaticText(wxWindow *parent,
                   wxWindowID id,
                   const wxString& label,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxString& name = wxStaticTextNameStr)
    {
        Create(parent, id, label, pos, size, style, name);
    }
    
protected:
    
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMoStaticText)
};

#endif
    // _WX_MOBILE_NATIVE_STATTEXT_H_
