/////////////////////////////////////////////////////////////////////////////
// Name:        icon.cpp
// Purpose:     wxIcon class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "icon.h"
#endif

#include "wx/wxprec.h"

#include "wx/icon.h"

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxIcon, wxBitmap)
#endif

#include "wx/image.h"
#include "wx/mac/private.h"


/*
 * Icons
 */

wxIcon::wxIcon()
{
}

wxIcon::wxIcon(const char bits[], int width, int height) :
    wxBitmap(bits, width, height)
{
    
}

wxIcon::wxIcon( const char **bits ) :
    wxBitmap(bits)
{
}

wxIcon::wxIcon( char **bits ) :
    wxBitmap(bits)
{
}

wxIcon::wxIcon(const wxString& icon_file, int flags,
    int desiredWidth, int desiredHeight)
{
    LoadFile(icon_file, (wxBitmapType) flags, desiredWidth, desiredHeight);
}

wxIcon::~wxIcon()
{
}

bool wxIcon::LoadFile(const wxString& filename, wxBitmapType type,
    int desiredWidth, int desiredHeight)
{
    UnRef();
    
    wxBitmapHandler *handler = FindHandler(type);

    if ( handler )
    {
        m_refData = new wxBitmapRefData;
        return handler->LoadFile(this, filename, type, desiredWidth, desiredHeight );
    }
    else
    {
        wxImage loadimage(filename, type);
        if (loadimage.Ok()) 
        {
            if ( desiredWidth == -1 )
                desiredWidth = loadimage.GetWidth() ;
            if ( desiredHeight == -1 )
                desiredHeight = loadimage.GetHeight() ;
            if ( desiredWidth != loadimage.GetWidth() || desiredHeight != loadimage.GetHeight() )
                loadimage.Rescale( desiredWidth , desiredHeight ) ;
            wxBitmap bmp( loadimage );
            wxIcon *icon = (wxIcon*)(&bmp);
            *this = *icon;
            return true;
        }
    }
    return false ;
}

void wxIcon::CopyFromBitmap(const wxBitmap& bmp)
{
    wxIcon *icon = (wxIcon*)(&bmp);
    *this = *icon;
}

IMPLEMENT_DYNAMIC_CLASS(wxICONResourceHandler, wxBitmapHandler)

bool  wxICONResourceHandler::LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
                                      int desiredWidth, int desiredHeight)
{
    short theId = -1 ;
    if ( name == wxT("wxICON_INFORMATION") )
    {
        theId = kNoteIcon ;
    }
    else if ( name == wxT("wxICON_QUESTION") )
    {
        theId = kCautionIcon ;
    }
    else if ( name == wxT("wxICON_WARNING") )
    {
        theId = kCautionIcon ;
    }
    else if ( name == wxT("wxICON_ERROR") )
    {
        theId = kStopIcon ;
    }
    else
    {
        Str255 theName ;
        OSType theType ;
        wxMacStringToPascal( name , theName ) ;
        
        Handle resHandle = GetNamedResource( 'cicn' , theName ) ;
        if ( resHandle != 0L )
        {
            GetResInfo( resHandle , &theId , &theType , theName ) ;
            ReleaseResource( resHandle ) ;
        }
    }
    if ( theId != -1 )
    {
        CIconHandle theIcon = (CIconHandle ) GetCIcon( theId ) ;
        if ( theIcon )
        {
            M_BITMAPHANDLERDATA->m_hIcon = theIcon ;
            M_BITMAPHANDLERDATA->m_width =  32 ;
            M_BITMAPHANDLERDATA->m_height = 32 ;
            
            M_BITMAPHANDLERDATA->m_depth = 8 ;
            M_BITMAPHANDLERDATA->m_ok = true ;
            M_BITMAPHANDLERDATA->m_numColors = 256 ;
            M_BITMAPHANDLERDATA->m_bitmapType = kMacBitmapTypeIcon ;
            return TRUE ;
        }
    }
    return FALSE ;
}
