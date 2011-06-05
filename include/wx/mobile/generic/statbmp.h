/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/generic/statbmp.h
// Purpose:     wxMoStaticBitmap class
// Author:      Julian Smart
// Modified by:
// Created:     16/05/09
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWidgets Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_GENERIC_STATBMP_H_
#define _WX_MOBILE_GENERIC_STATBMP_H_

#include "wx/statbmp.h"

extern WXDLLEXPORT_DATA(const wxChar) wxStaticBitmapNameStr[];

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
                   const wxGDIImage& label,
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
    // _WX_MOBILE_GENERIC_STATBMP_H_
