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


wxIconRefData::wxIconRefData()
{
    // TODO: init icon handle
}

wxIconRefData::~wxIconRefData()
{
    // TODO: destroy icon handle
}

wxIcon::wxIcon()
{
}

wxIcon::wxIcon(const char WXUNUSED(bits)[], int WXUNUSED(width), int WXUNUSED(height))
{
}

wxIcon::wxIcon(const wxString& icon_file, long flags,
    int desiredWidth, int desiredHeight)

{
    LoadFile(icon_file, flags, desiredWidth, desiredHeight);
}

wxIcon::~wxIcon()
{
}

bool wxIcon::LoadFile(const wxString& filename, long type,
    int desiredWidth, int desiredHeight)
{
  UnRef();

  m_refData = new wxIconRefData;

  wxBitmapHandler *handler = FindHandler(type);

  if ( handler )
	return handler->LoadFile(this, filename, type, desiredWidth, desiredHeight);
  else
	return FALSE;
}

