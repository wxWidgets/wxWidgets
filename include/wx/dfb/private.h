/////////////////////////////////////////////////////////////////////////////
// Name:        wx/dfb/private.h
// Purpose:     private helpers for wxDFB implementation
// Author:      Vaclav Slavik
// Created:     2006-08-09
// RCS-ID:      $Id$
// Copyright:   (c) 2006 REA Elektronik GmbH
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DFB_PRIVATE_H_
#define _WX_DFB_PRIVATE_H_

#include "wx/intl.h"
#include "wx/log.h"

#include "wx/dfb/wrapdfb.h"
#include <directfb_version.h>

//-----------------------------------------------------------------------------
// strings conversion
//-----------------------------------------------------------------------------

// convert string from wxString to UTF-8 encoded const char*
#if wxUSE_UNICODE
    #define wxSTR_TO_DFB(s)    (s).mb_str(wxConvUTF8)
#else
    #define wxSTR_TO_DFB(s)    wxConvUTF8.cWC2MB((s).wc_str(*wxConvUI))
#endif

//-----------------------------------------------------------------------------
// surface manipulation helpers
//-----------------------------------------------------------------------------

/// Mode of wxDfbCloneSurface() call
enum wxDfbCloneSurfaceMode
{
    /// Don't copy surface pixels, just clone surface size and attributes
    wxDfbCloneSurface_NoPixels = 0,
    /// Make exact copy, including the pixels
    wxDfbCloneSurface_CopyPixels
};

/**
    Creates a new surface by cloning existing one. Depending on @a mode,
    either makes exact copy (wxDfbCloneSurface_CopyPixels) or only creates a
    new surface with the same size and attributes (wxDfbCloneSurface_NoPixels).
 */
wxIDirectFBSurfacePtr wxDfbCloneSurface(const wxIDirectFBSurfacePtr& s,
                                        wxDfbCloneSurfaceMode mode);

/// Returns bit depth used by the surface
int wxDfbGetSurfaceDepth(const wxIDirectFBSurfacePtr& s);

/// Returns interface to the primary display layer:
wxIDirectFBDisplayLayerPtr wxDfbGetDisplayLayer();

/// Returns interface to the primary surface:
wxIDirectFBSurfacePtr wxDfbGetPrimarySurface();

//-----------------------------------------------------------------------------
// misc helpers
//-----------------------------------------------------------------------------

/// Convert DirectFB timestamp to wxEvent one:
#define wxDFB_EVENT_TIMESTAMP(event) \
        ((event).timestamp.tv_sec * 1000 + (event).timestamp.tv_usec / 1000)

/**
    Check if DirectFB library version is at least @a major.@a minor.@a release.

    @see wxCHECK_VERSION
 */
#define wxCHECK_DFB_VERSION(major,minor,release) \
    (DIRECTFB_MAJOR_VERSION > (major) || \
    (DIRECTFB_MAJOR_VERSION == (major) && \
        DIRECTFB_MINOR_VERSION > (minor)) || \
    (DIRECTFB_MAJOR_VERSION == (major) && \
        DIRECTFB_MINOR_VERSION == (minor) && \
            DIRECTFB_MICRO_VERSION >= (release)))

#endif // _WX_DFB_PRIVATE_H_
