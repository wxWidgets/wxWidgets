/////////////////////////////////////////////////////////////////////////////
// Name:        utils.cpp
// Purpose:
// Author:      Vaclav Slavik
// Id:          $Id$
// Copyright:   (c) 2001 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/utils.h"
#include "wx/string.h"

#include "wx/intl.h"
#include "wx/log.h"

#include "wx/process.h"

#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <mgraph.hpp>

#ifdef __UNIX__
#include "wx/unix/execute.h"
#endif

//----------------------------------------------------------------------------
// misc.
//----------------------------------------------------------------------------

void wxBell()
{
    // FIXME_MGL
}

// ----------------------------------------------------------------------------
// display characterstics
// ----------------------------------------------------------------------------

extern MGLDevCtx *g_displayDC;

void wxDisplaySize( int *width, int *height )
{
    wxASSERT_MSG( g_displayDC, wxT("MGL display DC not created yet.") );
    if (width) *width = g_displayDC->sizex();
    if (height) *height = g_displayDC->sizey();
}

void wxGetMousePosition( int* x, int* y )
{
#if 0 // FIXME_MGL
    gdk_window_get_pointer( (GdkWindow*) NULL, x, y, (GdkModifierType*) NULL );
#endif
}

bool wxColourDisplay()
{
    wxASSERT_MSG( g_displayDC, wxT("MGL display DC not created yet.") );
    
    return (wxDisplayDepth() > 1);
}

int wxDisplayDepth()
{
    wxASSERT_MSG( g_displayDC, wxT("MGL display DC not created yet.") );

    return g_displayDC->getBitsPerPixel();
}

int wxGetOsVersion(int *majorVsn, int *minorVsn)
{
#if 0 // FIXME_MGL
        // FIXME_MGL : wxGetOsVersion, too
  if (majorVsn) *majorVsn = GTK_MAJOR_VERSION;
  if (minorVsn) *minorVsn = GTK_MINOR_VERSION;

  return wxGTK;
#endif
}


#ifdef __UNIX__

int wxAddProcessCallback(wxEndProcessData *proc_data, int fd)
{
#if 0 // FIXME_MGL -do we need it at all?
    int tag = gdk_input_add(fd,
                            GDK_INPUT_READ,
                            GTK_EndProcessDetector,
                            (gpointer)proc_data);

    return tag;
#endif
}

#endif
