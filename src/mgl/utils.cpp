/////////////////////////////////////////////////////////////////////////////
// Name:        utils.cpp
// Purpose:
// Author:      Vaclav Slavik
// Id:          $Id$
// Copyright:   (c) 2001 SciTech Software, Inc. (www.scitechsoft.com)
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

#include "wx/mgl/private.h"

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

void wxDisplaySize(int *width, int *height)
{
    wxASSERT_MSG( g_displayDC, wxT("You must call wxApp::SetDisplayMode before using this function") );
    if (width) *width = g_displayDC->sizex()+1;
    if (height) *height = g_displayDC->sizey()+1;
}

void wxDisplaySizeMM(int *width, int *height)
{
    wxASSERT_MSG( g_displayDC, wxT("You must call wxApp::SetDisplayMode before using this function") );
    if ( width ) 
        *width = (g_displayDC->sizex()+1) * 25/72;
    if ( height ) 
        *height = (g_displayDC->sizey()+1) * 25/72;
    // FIXME_MGL -- what about returning *real* monitor dimensions?
}

void wxClientDisplayRect(int *x, int *y, int *width, int *height)
{
    if ( x ) *x = 0;
    if ( y ) *y = 0;
    wxDisplaySize(width, height);
    // FIXME_MGL - windowed version needs different handling
}

bool wxColourDisplay()
{
    wxASSERT_MSG( g_displayDC, wxT("You must call wxApp::SetDisplayMode before using this function") );
    
    return (wxDisplayDepth() > 1);
}

int wxDisplayDepth()
{
    wxASSERT_MSG( g_displayDC, wxT("You must call wxApp::SetDisplayMode before using this function") );

    return g_displayDC->getBitsPerPixel();
}

int wxGetOsVersion(int *majorVsn, int *minorVsn)
{
  if ( majorVsn )
      *majorVsn = MGL_RELEASE_MAJOR;
  if ( minorVsn )
      *minorVsn = MGL_RELEASE_MINOR;

#if defined(__UNIX__)
  return wxMGL_UNIX;
#elif defined(__OS2__)
  return wxMGL_OS2;
#elif defined(__WIN32__)
  return wxMGL_WIN32;
#endif
}


void wxGetMousePosition(int* x, int* y)
{
    MS_getPos(x, y);
}

wxPoint wxGetMousePosition()
{
    wxPoint pt;
    wxGetMousePosition(&pt.x, &pt.y);
    return pt;
}



#ifdef __UNIX__

int wxAddProcessCallback(wxEndProcessData *proc_data, int fd)
{
    wxFAIL_MSG(wxT("wxAddProcessCallback not implemented in wxMGL!"));
    return 0;
#if 0 // FIXME_MGL -do we need it at all?
    int tag = gdk_input_add(fd,
                            GDK_INPUT_READ,
                            GTK_EndProcessDetector,
                            (gpointer)proc_data);

    return tag;
#endif
}

#endif
