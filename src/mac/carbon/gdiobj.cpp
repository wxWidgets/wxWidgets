/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/gdiobj.cpp
// Purpose:     wxGDIObject class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/gdiobj.h"

#ifndef WX_PRECOMP
    #include "wx/gdicmn.h"
#endif

#include "wx/mac/private.h"

class wxStockGDIMac: public wxStockGDI
{
public:
    wxStockGDIMac();

    virtual const wxFont* GetFont(Item item);

private:
    typedef wxStockGDI super;
};

static wxStockGDIMac gs_wxStockGDIMac_instance;

wxStockGDIMac::wxStockGDIMac()
{
    // Override default instance
    ms_instance = this;
}

const wxFont* wxStockGDIMac::GetFont(Item item)
{
    wxFont* font = static_cast<wxFont*>(ms_stockObject[item]);
    if (font == NULL)
    {
        switch (item)
        {
        case FONT_NORMAL:
            font = new wxFont;
            font->MacCreateThemeFont(kThemeSystemFont);
            break;
        case FONT_SMALL:
            font = new wxFont;
            font->MacCreateThemeFont(kThemeSmallSystemFont);
            break;
        default:
            font = const_cast<wxFont*>(super::GetFont(item));
            break;
        }
        ms_stockObject[item] = font;
    }
    return font;
}
