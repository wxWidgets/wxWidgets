/////////////////////////////////////////////////////////////////////////////
// Name:        icon.cpp
// Author:      Vaclav Slavik
// Id:          $Id$
// Copyright:   (c) 2001 Vaclav Slavik
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "icon.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/icon.h"

//-----------------------------------------------------------------------------
// wxIcon
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxIcon, wxBitmap)

wxIcon::wxIcon(const char **bits, int WXUNUSED(width), int WXUNUSED(height)) :
    wxBitmap(bits)
{
}

wxIcon::wxIcon(char **bits, int WXUNUSED(width), int WXUNUSED(height)) :
    wxBitmap(bits)
{
}

wxIcon::wxIcon(const wxIcon& icon) : wxBitmap()
{
    Ref(icon);
}

wxIcon& wxIcon::operator = (const wxIcon& icon)
{
    if (*this == icon) 
        return (*this);
    Ref(icon);
    return *this;
}

void wxIcon::CopyFromBitmap(const wxBitmap& bmp)
{
    wxIcon *icon = (wxIcon*)(&bmp);
    *this = *icon;
}

