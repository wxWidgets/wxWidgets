/////////////////////////////////////////////////////////////////////////////
// Name:        gtk/window.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
    #pragma implementation "window.h"
#endif

#include "wx/defs.h"
#include "wx/window.h"

IMPLEMENT_DYNAMIC_CLASS(wxWindowMGL, wxWindowBase)

wxWindow *g_captureWindow = NULL;
wxWindow *g_focusWindow = NULL;

/* static */
wxWindow *wxWindowBase::GetCapture()
{
    return (wxWindow *)g_captureWindow;
}

wxWindow *wxWindowBase::FindFocus()
{
    // the cast is necessary when we compile in wxUniversal mode
    return (wxWindow *)g_focusWindow;
}
