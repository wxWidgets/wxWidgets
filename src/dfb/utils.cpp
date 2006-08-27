/////////////////////////////////////////////////////////////////////////////
// Name:        src/dfb/utils.cpp
// Purpose:     Miscellaneous utility functions and classes
// Author:      Vaclav Slavik
// Created:     2006-08-08
// RCS-ID:      $Id$
// Copyright:   (c) 2006 REA Elektronik GmbH
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/utils.h"
#include "wx/apptrait.h"
#include "wx/unix/execute.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif

#include "wx/dfb/private.h"
#include <directfb_version.h>

// ----------------------------------------------------------------------------
// toolkit info
// ----------------------------------------------------------------------------

wxPortId wxGUIAppTraits::GetToolkitVersion(int *verMaj, int *verMin) const
{
    if ( verMaj ) *verMaj = DIRECTFB_MAJOR_VERSION;
    if ( verMin ) *verMaj = DIRECTFB_MINOR_VERSION;

    return wxPORT_DFB;
}

// ----------------------------------------------------------------------------
// display characteristics
// ----------------------------------------------------------------------------

bool wxColourDisplay()
{
    #warning "FIXME: wxColourDisplay"
    return true;
}

int wxDisplayDepth()
{
    return wxTheApp->GetDisplayMode().bpp;
}

void wxDisplaySize(int *width, int *height)
{
    wxVideoMode mode(wxTheApp->GetDisplayMode());
    if ( width ) *width = mode.w;
    if ( height ) *height = mode.h;
}

void wxDisplaySizeMM(int *width, int *height)
{
    // FIXME: there's no way to get physical resolution using the DirectDB
    //        API, we hardcode a commonly used value of 72dpi
    #define DPI          72.0
    #define PX_TO_MM(x)  (int(((x) / DPI) * inches2mm))

    wxDisplaySize(width, height);
    if ( width ) *width = PX_TO_MM(*width);
    if ( height ) *height = PX_TO_MM(*height);

    #undef DPI
    #undef PX_TO_MM
}

void wxClientDisplayRect(int *x, int *y, int *width, int *height)
{
    // return desktop dimensions minus any panels, menus, trays:
    if (x) *x = 0;
    if (y) *y = 0;
    wxDisplaySize(width, height);
}

//-----------------------------------------------------------------------------
// surface manipulation helpers
//-----------------------------------------------------------------------------

IDirectFBSurfacePtr wxDfbCloneSurface(const IDirectFBSurfacePtr& s,
                                      wxDfbCloneSurfaceMode mode)
{
    if ( !s )
        return NULL;

    DFBSurfaceDescription desc;
    desc.flags = (DFBSurfaceDescriptionFlags)(
            DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT);
    s->GetCapabilities(s, &desc.caps);
    s->GetSize(s, &desc.width, &desc.height);
    s->GetPixelFormat(s, &desc.pixelformat);

    IDirectFBPtr dfb(wxTheApp->GetDirectFBInterface());

    IDirectFBSurfacePtr snew;
    if ( !DFB_CALL( dfb->CreateSurface(dfb, &desc, &snew) ) )
        return NULL;

    IDirectFBPalettePtr pal;
    if ( s->GetPalette(s, &pal) == DFB_OK )
    {
        if ( !DFB_CALL( snew->SetPalette(snew, pal) ) )
            return NULL;
    }

    if ( mode == wxDfbCloneSurface_CopyPixels )
    {
        if ( !DFB_CALL( snew->SetBlittingFlags(snew, DSBLIT_NOFX) ) )
            return NULL;
        if ( !DFB_CALL( snew->Blit(snew, s, NULL, 0, 0) ) )
            return NULL;
    }

    return snew;
}

int wxDfbGetSurfaceDepth(const IDirectFBSurfacePtr& s)
{
    wxCHECK_MSG( s, -1, _T("invalid surface") );

    DFBSurfacePixelFormat format = DSPF_UNKNOWN;

    if ( !DFB_CALL( s->GetPixelFormat(s, &format) ) )
        return -1;

    return DFB_BITS_PER_PIXEL(format);
}

IDirectFBDisplayLayerPtr wxDfbGetDisplayLayer()
{
    IDirectFBPtr dfb(wxTheApp->GetDirectFBInterface());

    IDirectFBDisplayLayerPtr layer;
    if ( !DFB_CALL( dfb->GetDisplayLayer(dfb, DLID_PRIMARY, &layer) ) )
        return NULL;

    return layer;
}

IDirectFBSurfacePtr wxDfbGetPrimarySurface()
{
    IDirectFBDisplayLayerPtr layer(wxDfbGetDisplayLayer());
    IDirectFBSurfacePtr surface;
    DFB_CALL( layer->GetSurface(layer, &surface) );
    return surface;
}


//-----------------------------------------------------------------------------
// mouse
//-----------------------------------------------------------------------------

void wxGetMousePosition(int *x, int *y)
{
    IDirectFBDisplayLayerPtr layer(wxDfbGetDisplayLayer());
    DFB_CALL( layer->GetCursorPosition(layer, x, y) );
}

wxPoint wxGetMousePosition()
{
    wxPoint pt;
    wxGetMousePosition(&pt.x, &pt.y);
    return pt;
}

//-----------------------------------------------------------------------------
// keyboard
//-----------------------------------------------------------------------------

bool wxGetKeyState(wxKeyCode key)
{
    wxASSERT_MSG(key != WXK_LBUTTON && key != WXK_RBUTTON && key != WXK_MBUTTON,
                 _T("can't use wxGetKeyState() for mouse buttons"));

    return false; // FIXME
}

//----------------------------------------------------------------------------
// misc.
//----------------------------------------------------------------------------

void wxBell()
{
}

int wxAddProcessCallback(wxEndProcessData *proc_data, int fd)
{
    wxFAIL_MSG( _T("wxAddProcessCallback not implemented") );
    return 0;
}
