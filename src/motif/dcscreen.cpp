/////////////////////////////////////////////////////////////////////////////
// Name:        dcscreen.cpp
// Purpose:     wxScreenDC class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dcscreen.h"
#endif

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

bool wxScreenDC::StartDrawingOnTop(wxWindow* window)
{
    // TODO
    return FALSE;
}

bool wxScreenDC::StartDrawingOnTop(wxRect* rect = NULL)
{
    // TODO
    return FALSE;
}

bool wxScreenDC::EndDrawingOnTop()
{
    // TODO
    return TRUE;
}
