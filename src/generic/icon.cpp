/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/icon.cpp
// Purpose:     wxIcon implementation for ports where it's same as wxBitmap
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include "wx/icon.h"

//-----------------------------------------------------------------------------
// wxIcon
//-----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxIcon, wxBitmap);

wxIcon::wxIcon(const char* const* bits) :
    wxBitmap( bits )
{
}

wxIcon::wxIcon() :  wxBitmap()
{
}

void wxIcon::CopyFromBitmap(const wxBitmap& bmp)
{
    const wxIcon* icon = static_cast<const wxIcon*>(&bmp);
    *this = *icon;
}
