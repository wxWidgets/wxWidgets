/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/icon.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/icon.h"

IMPLEMENT_DYNAMIC_CLASS( wxIcon, wxBitmap )

wxIcon::wxIcon()
{
}

wxIcon::wxIcon( const char * const *bits )
{
}

#ifdef wxNEEDS_CHARPP
wxIcon::wxIcon( char **bits )
{
}
#endif


wxIcon::wxIcon( const wxString& filename, wxBitmapType type,
    int desiredWidth, int desiredHeight )
{
}


wxIcon::wxIcon( const wxIconLocation &location )
{
}

void wxIcon::CopyFromBitmap( const wxBitmap &bitmap )
{
}


wxGDIRefData *wxIcon::CreateGDIRefData() const
{
    return NULL;
}

wxGDIRefData *wxIcon::CloneGDIRefData(const wxGDIRefData *data) const
{
    return NULL;
}


