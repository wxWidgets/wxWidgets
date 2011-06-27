/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/native/statbmp.h
// Purpose:     wxMobile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_NATIVE_STATBMP_H_
#define _WX_MOBILE_NATIVE_STATBMP_H_

#include "wx/statbmp.h"

extern WXDLLEXPORT_DATA(const char) wxStaticBitmapNameStr[];

/**
 @class wxMoStaticBitmap
 
 A control showing an icon or a bitmap.
 
 @category{wxMobile}
 */

class WXDLLEXPORT wxMoStaticBitmap : public wxStaticBitmap
{
public:
    /// Default constructor.
    wxMoStaticBitmap() { }
    
    /// Constructor.
    wxMoStaticBitmap(wxWindow *parent,
                     wxWindowID id,
                     const wxImage& label,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = 0,
                     const wxString& name = wxStaticBitmapNameStr)
    {
        Create(parent, id, label, pos, size, style, name);
    }
    
protected:
private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMoStaticBitmap)
};

#endif
    // _WX_MOBILE_NATIVE_STATBMP_H_
