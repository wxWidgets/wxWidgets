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

wxIcon::wxIcon(const char bits[], int width, int height) 
{
    wxBitmap bmp(bits,width,height) ;
    CopyFromBitmap( bmp ) ;
}

wxIcon::wxIcon( const char **bits ) 
{
    wxBitmap bmp(bits) ;
    CopyFromBitmap( bmp ) ;
}

wxIcon::wxIcon( char **bits ) 
{
    wxBitmap bmp(bits) ;
    CopyFromBitmap( bmp ) ;
}

wxIcon::wxIcon(const wxString& icon_file, int flags,
    int desiredWidth, int desiredHeight)
{
    LoadFile(icon_file, (wxBitmapType) flags, desiredWidth, desiredHeight);
}

wxIcon::~wxIcon()
{
}

WXHICON wxIcon::GetHICON() const 
{
    wxASSERT( Ok() ) ;
    return (WXHICON) ((wxIconRefData*)m_refData)->GetHICON() ;
}

int wxIcon::GetWidth() const
{
    return 32 ;
}

int wxIcon::GetHeight() const
{
    return 32 ;
}

bool wxIcon::Ok() const
{
    return m_refData != NULL ;
}

bool wxIcon::LoadFile(const wxString& filename, wxBitmapType type,
    int desiredWidth, int desiredHeight)
{
    UnRef();
    
    if ( type == wxBITMAP_TYPE_ICON_RESOURCE )
    {
        OSType theId = 0 ;
        if ( filename == wxT("wxICON_INFORMATION") )
        {
            theId = kAlertNoteIcon ;
        }
        else if ( filename == wxT("wxICON_QUESTION") )
        {
            theId = kAlertCautionIcon ;
        }
        else if ( filename == wxT("wxICON_WARNING") )
        {
            theId = kAlertCautionIcon ;
        }
        else if ( filename == wxT("wxICON_ERROR") )
        {
            theId = kAlertStopIcon ;
        }
        else
        {/*
            Str255 theName ;
            OSType theType ;
            wxMacStringToPascal( name , theName ) ;
            
            Handle resHandle = GetNamedResource( 'cicn' , theName ) ;
            if ( resHandle != 0L )
            {
                GetResInfo( resHandle , &theId , &theType , theName ) ;
                ReleaseResource( resHandle ) ;
            }
        */
        }
        if ( theId != 0 )
        {
            IconRef iconRef = NULL ;
            verify_noerr(GetIconRef(kOnSystemDisk,kSystemIconsCreator,theId, &iconRef)) ;    
            if ( iconRef )
            {
                m_refData = new wxIconRefData( (WXHICON) iconRef ) ;
                return TRUE ;
            }
        }
        return FALSE ;
    }
    else
    {
        wxBitmapHandler *handler = wxBitmap::FindHandler(type);

        if ( handler )
        {
            wxBitmap bmp ;
            if ( handler->LoadFile(&bmp , filename, type, desiredWidth, desiredHeight ))
            {
                CopyFromBitmap( bmp ) ;
                return true ;
            }
            return false ;
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
                CopyFromBitmap( bmp ) ;
                return true;
            }
        }
    }
    return true ;
}

void wxIcon::CopyFromBitmap(const wxBitmap& bmp)
{
    UnRef() ;
    
    m_refData = new wxIconRefData( (WXHICON) wxMacCreateIconRef( bmp ) ) ;
}

wxIconRefData::wxIconRefData( WXHICON icon )
{
    m_iconRef = MAC_WXHICON( icon ) ;
}

void wxIconRefData::Init() 
{
    m_iconRef = NULL ;
}

void wxIconRefData::Free()
{
    if ( m_iconRef )
    {
        ReleaseIconRef( m_iconRef ) ;
        m_iconRef = NULL ;
    }
}

IMPLEMENT_DYNAMIC_CLASS(wxICONResourceHandler, wxBitmapHandler)

bool  wxICONResourceHandler::LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
                                      int desiredWidth, int desiredHeight)
{
    wxIcon icon ;
    icon.LoadFile( name , wxBITMAP_TYPE_ICON_RESOURCE , desiredWidth , desiredHeight ) ;
    bitmap->CopyFromIcon( icon ) ;
    return bitmap->Ok() ;
}
