/////////////////////////////////////////////////////////////////////////////
// Name:        icon.cpp
// Purpose:     wxIcon class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "icon.h"
#endif

#include "wx/icon.h"

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxIcon, wxBitmap)
#endif

/*
 * Icons
 */


wxIcon::wxIcon()
{
}

wxIcon::wxIcon(const char **xpm)
{
}

wxIcon::wxIcon(const char WXUNUSED(bits)[], int WXUNUSED(width), int WXUNUSED(height))
{
}

wxIcon::wxIcon(const wxString& icon_file, int flags,
    int desiredWidth, int desiredHeight)

{
    LoadFile(icon_file, (wxBitmapType)flags, desiredWidth, desiredHeight);
}

wxIcon::~wxIcon()
{
}

bool wxIcon::LoadFile(const wxString& filename, wxBitmapType type,
    int desiredWidth, int desiredHeight)
{
  UnRef();

  return FALSE;
}

void wxIcon::CopyFromBitmap(const wxBitmap& icno)
{
}

