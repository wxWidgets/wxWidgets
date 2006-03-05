/////////////////////////////////////////////////////////////////////////////
// Name:        src/mgl/display.cpp
// Purpose:     MGL Implementation of wxDisplay class
// Author:      Wlodzimierz ABX Skiba
// Modified by:
// Created:     05/03/2006
// RCS-ID:      $Id$
// Copyright:   (c) Wlodzimierz Skiba
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_DISPLAY

#ifndef WX_PRECOMP
    #include "wx/gdicmn.h"
#endif

#include "wx/display.h"

/* static */
int wxDisplayBase::GetFromPoint ( const wxPoint& WXUNUSED(pt) )
{
    // TODO
    return wxNOT_FOUND;
}

/* static */
size_t wxDisplayBase::GetCount()
{
    // TODO
    return 1;
}

// ----------------------------------------------------------------------------
// wxDisplay ctor/dtor
// ----------------------------------------------------------------------------

wxDisplay::wxDisplay ( size_t n )
         : wxDisplayBase ( n )
{
}

wxDisplay::~wxDisplay()
{
}

bool wxDisplay::IsOk() const
{
    // TODO
    return m_index < GetCount();
}

wxRect wxDisplay::GetGeometry() const
{
    wxRect rect;
    // TODO
    return rect;
}

wxString wxDisplay::GetName() const
{
    // TODO
    return wxEmptyString;
}

wxArrayVideoModes wxDisplay::GetModes(const wxVideoMode& WXUNUSED(modeMatch)) const
{
    wxArrayVideoModes modes;
    // TODO
    return modes;
}

wxVideoMode wxDisplay::GetCurrentMode() const
{
    wxVideoMode mode;
    // TODO
    return mode;
}

bool wxDisplay::ChangeMode(const wxVideoMode& WXUNUSED(mode))
{
    // TODO
    return false;
}

bool wxDisplay::IsPrimary() const
{
    // TODO
    return false;
}

#endif // wxUSE_DISPLAY
