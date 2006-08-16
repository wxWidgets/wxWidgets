/////////////////////////////////////////////////////////////////////////////
// Name:        src/os2/gdiobj.cpp
// Purpose:     wxGDIObject class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/gdiobj.h"

#ifndef WX_PRECOMP
    #include "wx/font.h"
    #include "wx/gdicmn.h"
#endif

class wxStockGDIPM: public wxStockGDI
{
public:
    wxStockGDIPM();

    virtual const wxFont* GetFont(Item item);

private:
    typedef wxStockGDI super;
};

static wxStockGDIPM gs_wxStockGDIPM_instance;

wxStockGDIPM::wxStockGDIPM()
{
    // Override default instance
    ms_instance = this;
}

const wxFont* wxStockGDIPM::GetFont(Item item)
{
    wxFont* font = wx_static_cast(wxFont*, ms_stockObject[item]);
    if (font == NULL)
    {
        const int fontSize = 12;
        switch (item)
        {
        case FONT_NORMAL:
            font = new wxFont(fontSize, wxMODERN, wxNORMAL, wxBOLD);
            break;
        case FONT_SMALL:
            font = new wxFont(fontSize - 4, wxSWISS, wxNORMAL, wxNORMAL);
            break;
        default:
            font = wx_const_cast(wxFont*, super::GetFont(item));
            break;
        }
        ms_stockObject[item] = font;
    }
    return font;
}
