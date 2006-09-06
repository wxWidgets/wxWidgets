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

wxIDirectFBSurfacePtr wxDfbCloneSurface(const wxIDirectFBSurfacePtr& s,
                                        wxDfbCloneSurfaceMode mode)
{
    if ( !s )
        return NULL;

    DFBSurfaceDescription desc;
    desc.flags = (DFBSurfaceDescriptionFlags)(
            DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT);
    s->GetCapabilities(&desc.caps);
    s->GetSize(&desc.width, &desc.height);
    s->GetPixelFormat(&desc.pixelformat);

    wxIDirectFBSurfacePtr snew(wxIDirectFB::Get()->CreateSurface(&desc));
    if ( !snew )
        return NULL;

    if ( desc.pixelformat == DSPF_LUT8 )
    {
        wxIDirectFBPalettePtr pal(s->GetPalette());
        if ( s )
        {
            if ( !snew->SetPalette(pal) )
                return NULL;
        }
    }

    if ( mode == wxDfbCloneSurface_CopyPixels )
    {
        if ( !snew->SetBlittingFlags(DSBLIT_NOFX) )
            return NULL;
        if ( !snew->Blit(s, NULL, 0, 0) )
            return NULL;
    }

    return snew;
}

int wxDfbGetSurfaceDepth(const wxIDirectFBSurfacePtr& s)
{
    wxCHECK_MSG( s, -1, _T("invalid surface") );

    DFBSurfacePixelFormat format = DSPF_UNKNOWN;

    if ( !s->GetPixelFormat(&format) )
        return -1;

    return DFB_BITS_PER_PIXEL(format);
}

wxIDirectFBDisplayLayerPtr wxDfbGetDisplayLayer()
{
    return wxIDirectFB::Get()->GetDisplayLayer(DLID_PRIMARY);
}

wxIDirectFBSurfacePtr wxDfbGetPrimarySurface()
{
    wxIDirectFBDisplayLayerPtr layer(wxDfbGetDisplayLayer());
    return layer ? layer->GetSurface() : NULL;
}


//-----------------------------------------------------------------------------
// mouse
//-----------------------------------------------------------------------------

void wxGetMousePosition(int *x, int *y)
{
    wxIDirectFBDisplayLayerPtr layer(wxDfbGetDisplayLayer());
    if ( layer )
        layer->GetCursorPosition(x, y);
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
