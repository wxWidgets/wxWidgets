///////////////////////////////////////////////////////////////////////////////
// Name:        palmos/gdiimage.cpp
// Purpose:     wxGDIImage implementation
// Author:      William Osborne
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id: 
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "gdiimage.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/log.h"
#endif // WX_PRECOMP

#include "wx/app.h"

#include "wx/bitmap.h"
#include "wx/palmos/gdiimage.h"

#if wxUSE_WXDIB
#include "wx/palmos/dib.h"
#endif

#include "wx/file.h"

#include "wx/listimpl.cpp"
WX_DEFINE_LIST(wxGDIImageHandlerList);

// ----------------------------------------------------------------------------
// auxiliary functions
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// ============================================================================
// implementation
// ============================================================================

wxGDIImageHandlerList wxGDIImage::ms_handlers;

// ----------------------------------------------------------------------------
// wxGDIImage functions forwarded to wxGDIImageRefData
// ----------------------------------------------------------------------------

bool wxGDIImage::FreeResource(bool WXUNUSED(force))
{
    return false;
}

WXHANDLE wxGDIImage::GetResourceHandle() const
{
    return NULL;
}

// ----------------------------------------------------------------------------
// wxGDIImage handler stuff
// ----------------------------------------------------------------------------

void wxGDIImage::AddHandler(wxGDIImageHandler *handler)
{
}

void wxGDIImage::InsertHandler(wxGDIImageHandler *handler)
{
}

bool wxGDIImage::RemoveHandler(const wxString& name)
{
    return false;
}

wxGDIImageHandler *wxGDIImage::FindHandler(const wxString& name)
{
    return NULL;
}

wxGDIImageHandler *wxGDIImage::FindHandler(const wxString& extension,
                                           long type)
{
    return NULL;
}

wxGDIImageHandler *wxGDIImage::FindHandler(long type)
{
    return NULL;
}

void wxGDIImage::CleanUpHandlers()
{
}

void wxGDIImage::InitStandardHandlers()
{
}


