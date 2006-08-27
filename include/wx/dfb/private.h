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

#include <directfb.h>
#include <directfb_version.h>

#include "wx/dfb/ifacehelpers.h"

wxDFB_DECLARE_INTERFACE(IDirectFB);
wxDFB_DECLARE_INTERFACE(IDirectFBDisplayLayer);
wxDFB_DECLARE_INTERFACE(IDirectFBSurface);
wxDFB_DECLARE_INTERFACE(IDirectFBPalette);

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
// error checking
//-----------------------------------------------------------------------------

inline bool wxDfbCheckReturn(DFBResult code)
{
    switch ( code )
    {
        case DFB_OK:
            return true;

        // these are programming errors, assert:
        #define DFB_ASSERT(code)                                        \
            case code:                                                  \
                wxFAIL_MSG( _T("DirectFB error: ") _T(#code) );         \
                return false                                            \

        DFB_ASSERT(DFB_DEAD);
        DFB_ASSERT(DFB_UNSUPPORTED);
        DFB_ASSERT(DFB_UNIMPLEMENTED);
        DFB_ASSERT(DFB_INVARG);
        DFB_ASSERT(DFB_NOIMPL);
        DFB_ASSERT(DFB_MISSINGFONT);
        DFB_ASSERT(DFB_THIZNULL);
        DFB_ASSERT(DFB_INVAREA);
        DFB_ASSERT(DFB_DESTROYED);
        DFB_ASSERT(DFB_NOSUCHMETHOD);
        DFB_ASSERT(DFB_NOSUCHINSTANCE);
        DFB_ASSERT(DFB_VERSIONMISMATCH);

        #undef  DFB_ASSERT

        // these are not errors, but valid return codes:
        case DFB_INTERRUPTED:
        case DFB_BUFFEREMPTY:
            return true;

        default:
            // FIXME: should handle the errors individually
            wxLogError(_("DirectFB error %d occured."), (int)code);
            return false;
    }
}

/**
    Wrap all calls to DirectFB in this macro so that the return value is
    checked and errors reported as appropriate.

    Returns true if the call succeeded, false otherwise.
 */
#define DFB_CALL(call)      (wxDfbCheckReturn(call))


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
IDirectFBSurfacePtr wxDfbCloneSurface(const IDirectFBSurfacePtr& s,
                                      wxDfbCloneSurfaceMode mode);

/// Returns bit depth used by the surface
int wxDfbGetSurfaceDepth(const IDirectFBSurfacePtr& s);

/// Returns interface to the primary display layer:
IDirectFBDisplayLayerPtr wxDfbGetDisplayLayer();

/// Returns interface to the primary surface:
IDirectFBSurfacePtr wxDfbGetPrimarySurface();


//-----------------------------------------------------------------------------
// wxDfbEvent
//-----------------------------------------------------------------------------

/**
    The struct defined by this macro is a thin wrapper around DFB*Event type.
    It is needed because DFB*Event are typedefs and so we can't forward declare
    them, but we need to pass them to methods declared in public headers where
    <directfb.h> cannot be included. So this struct just holds the event value,
    it's sole purpose is that it can be forward declared.
 */
#define WXDFB_DEFINE_EVENT_WRAPPER(T)                                       \
    struct wx##T                                                            \
    {                                                                       \
        wx##T() {}                                                          \
        wx##T(const T& event) : m_event(event) {}                           \
                                                                            \
        operator T&() { return m_event; }                                   \
        operator const T&() const { return m_event; }                       \
        T* operator&() { return &m_event; }                                 \
                                                                            \
        DFBEventClass GetClass() const { return m_event.clazz; }            \
                                                                            \
    private:                                                                \
        T m_event;                                                          \
    };

WXDFB_DEFINE_EVENT_WRAPPER(DFBEvent)
WXDFB_DEFINE_EVENT_WRAPPER(DFBWindowEvent)

/// Convert DirectFB timestamp to wxEvent one:
#define wxDFB_EVENT_TIMESTAMP(event) \
        ((event).timestamp.tv_sec * 1000 + (event).timestamp.tv_usec / 1000)

/**
    Check if DirectFB library version is at least @a major.@a minor.@a release.

    @sa wxCHECK_VERSION
 */
#define wxCHECK_DFB_VERSION(major,minor,release) \
    (DIRECTFB_MAJOR_VERSION > (major) || \
    (DIRECTFB_MAJOR_VERSION == (major) && \
        DIRECTFB_MINOR_VERSION > (minor)) || \
    (DIRECTFB_MAJOR_VERSION == (major) && \
        DIRECTFB_MINOR_VERSION == (minor) && \
            DIRECTFB_MICRO_VERSION >= (release)))

#endif // _WX_DFB_PRIVATE_H_
