/////////////////////////////////////////////////////////////////////////////
// Name:        src/palmos/dcscreen.cpp
// Purpose:     wxScreenDC class
// Author:      William Osborne
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id: 
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "dcscreen.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
   #include "wx/string.h"
   #include "wx/window.h"
#endif

#include "wx/dcscreen.h"

IMPLEMENT_DYNAMIC_CLASS(wxScreenDC, wxWindowDC)

// Create a DC representing the whole screen
wxScreenDC::wxScreenDC()
{
}

void wxScreenDC::DoGetSize(int *width, int *height) const
{
}

