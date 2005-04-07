///////////////////////////////////////////////////////////////////////////
// Name:        displayx11.cpp
// Purpose:     Unix/X11 implementation of wxDisplay class
// Author:      Brian Victor
// Modified by:
// Created:     12/05/02
// RCS-ID:      $Id$
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "display.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#include "wx/display.h"
#include "wx/intl.h"
#include "wx/log.h"

#ifndef WX_PRECOMP
  #include "wx/dynarray.h"
  #include "wx/gdicmn.h"
  #include "wx/string.h"
  #include "wx/utils.h"
#endif /* WX_PRECOMP */

#if wxUSE_DISPLAY

size_t wxDisplayBase::GetCount()
{
    return 1;
}

int wxDisplayBase::GetFromPoint(const wxPoint &p)
{
    return -1;
}

wxDisplay::wxDisplay(size_t index) 
: wxDisplayBase ( index )
{
}

wxDisplay::~wxDisplay()
{
}

wxRect wxDisplay::GetGeometry() const
{
    wxRect         vRect(0,0,0,0);

    return vRect;
}

int wxDisplay::GetDepth() const
{
  return 24;
}

wxString wxDisplay::GetName() const
{
  return wxEmptyString;
}


wxArrayVideoModes wxDisplay::GetModes(const wxVideoMode& mode) const
{
  wxArrayVideoModes modes;
  return modes;
}

wxVideoMode wxDisplay::GetCurrentMode() const
{
    // Not implemented
    return wxVideoMode();
}

bool wxDisplay::ChangeMode(const wxVideoMode& mode)
{
    // Not implemented
    return false;
}

#endif /* wxUSE_DISPLAY */
