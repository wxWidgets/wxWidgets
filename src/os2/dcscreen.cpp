/////////////////////////////////////////////////////////////////////////////
// Name:        dcscreen.cpp
// Purpose:     wxScreenDC class
// Author:      David Webster
// Modified by:
// Created:     10/14/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#define INCL_DEV
#define INCL_GPI
#define INCL_PM
#include<os2.h>

#ifndef WX_PRECOMP
   #include "wx/string.h"
   #include "wx/window.h"
#endif

#include "wx/os2/private.h"

#include "wx/dcscreen.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxScreenDC, wxWindowDC)
#endif

// Create a DC representing the whole screen
wxScreenDC::wxScreenDC()
{
    // TODO
}

wxScreenDC::~wxScreenDC()
{
    // TODO
}

