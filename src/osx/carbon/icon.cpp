/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/carbon/icon.cpp
// Purpose:     wxIcon class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/icon.h"

#ifndef WX_PRECOMP
    #include "wx/image.h"
#endif

#include "wx/osx/private.h"

IMPLEMENT_DYNAMIC_CLASS(wxIcon, wxGDIObject)

#define M_ICONDATA ((wxIconRefData *)m_refData)

class WXDLLEXPORT wxIconRefData : public wxGDIRefData
{
public:
    wxIconRefData() { Init(); }
    wxIconRefData( WXHICON iconref, int desiredWidth, int desiredHeight );
    virtual ~wxIconRefData() { Free(); }

    virtual bool IsOk() const { return m_iconRef != NULL; }

    virtual void Free();

    void SetWidth( int width ) { m_width = width; }
    void SetHeight( int height ) { m_height = height; }

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

    WXHICON GetHICON() const { return (WXHICON) m_iconRef; }

private:
    void Init();

    IconRef m_iconRef;
    int m_width;
    int m_height;
};


wxIconRefData::wxIconRefData( WXHICON icon, int desiredWidth, int desiredHeight )
{
    m_iconRef = (IconRef)( icon ) ;

    // Standard sizes
    SetWidth( desiredWidth == -1 ? 32 : desiredWidth ) ;
    SetHeight( desiredHeight == -1 ? 32 : desiredHeight ) ;
}

void wxIconRefData::Init()
{
    m_iconRef = NULL ;
    m_width =
    m_height = 0;
}

void wxIconRefData::Free()
{
    if ( m_iconRef )
    {
        ReleaseIconRef( m_iconRef ) ;
        m_iconRef = NULL ;
    }
}

//
//
//

wxIcon::wxIcon()
{
}

wxIcon::wxIcon( const char bits[], int width, int height )
{
    wxBitmap bmp( bits, width, height ) ;
    CopyFromBitmap( bmp ) ;
}

wxIcon::wxIcon(const char* const* bits)
{
    wxBitmap bmp( bits ) ;
    CopyFromBitmap( bmp ) ;
}

wxIcon::wxIcon(
    const wxString& icon_file, wxBitmapType flags,
    int desiredWidth, int desiredHeight )
{
    LoadFile( icon_file, flags, desiredWidth, desiredHeight );
}

wxIcon::wxIcon(WXHICON icon, const wxSize& size)
      : wxGDIObject()
{
    // as the icon owns that ref, we have to acquire it as well
    if (icon)
        AcquireIconRef( (IconRef) icon ) ;

    m_refData = new wxIconRefData( icon, size.x, size.y ) ;
}

wxIcon::~wxIcon()
{
}

wxGDIRefData *wxIcon::CreateGDIRefData() const
{
    return new wxIconRefData;
}

wxGDIRefData *wxIcon::CloneGDIRefData(const wxGDIRefData *data) const
{
    return new wxIconRefData(*static_cast<const wxIconRefData *>(data));
}

WXHICON wxIcon::GetHICON() const
{
    wxASSERT( Ok() ) ;

    return (WXHICON) ((wxIconRefData*)m_refData)->GetHICON() ;
}

int wxIcon::GetWidth() const
{
   wxCHECK_MSG( Ok(), -1, wxT("invalid icon") );

   return M_ICONDATA->GetWidth();
}

int wxIcon::GetHeight() const
{
   wxCHECK_MSG( Ok(), -1, wxT("invalid icon") );

   return M_ICONDATA->GetHeight();
}

int wxIcon::GetDepth() const
{
    return 32;
}

void wxIcon::SetDepth( int WXUNUSED(depth) )
{
}

void wxIcon::SetWidth( int WXUNUSED(width) )
{
}

void wxIcon::SetHeight( int WXUNUSED(height) )
{
}

bool wxIcon::LoadFile(
    const wxString& filename, wxBitmapType type,
    int desiredWidth, int desiredHeight )
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
        else if ( filename == wxT("wxICON_FOLDER") )
        {
            theId = kGenericFolderIcon ;
        }
        else if ( filename == wxT("wxICON_FOLDER_OPEN") )
        {
            theId = kOpenFolderIcon ;
        }
        else if ( filename == wxT("wxICON_NORMAL_FILE") )
        {
            theId = kGenericDocumentIcon ;
        }
        else
        {
            IconRef iconRef = NULL ;

            // first look in the resource fork
            if ( iconRef == NULL )
            {
                Str255 theName ;

                wxMacStringToPascal( filename , theName ) ;
                Handle resHandle = GetNamedResource( 'icns' , theName ) ;
                if ( resHandle != 0L )
                {
                    IconFamilyHandle iconFamily = (IconFamilyHandle) resHandle ;
                    HLock((Handle) iconFamily);
                    OSStatus err = GetIconRefFromIconFamilyPtr( *iconFamily, GetHandleSize((Handle) iconFamily), &iconRef );
                    HUnlock((Handle) iconFamily);
                    if ( err != noErr )
                    {
                        wxFAIL_MSG("Error when constructing icon ref");
                    }

                    ReleaseResource( resHandle ) ;
                }
              }
            if ( iconRef == NULL )
            {
                // TODO add other attempts to load it from files etc here
            }
               if ( iconRef )
               {
                   m_refData = new wxIconRefData( (WXHICON) iconRef, desiredWidth, desiredHeight ) ;
                return true ;
               }
        }

        if ( theId != 0 )
        {
            IconRef iconRef = NULL ;
            verify_noerr( GetIconRef( kOnSystemDisk, kSystemIconsCreator, theId, &iconRef ) ) ;
            if ( iconRef )
            {
                m_refData = new wxIconRefData( (WXHICON) iconRef, desiredWidth, desiredHeight ) ;

                return true ;
            }
        }

        return false ;
    }
    else
    {
        wxBitmapHandler *handler = wxBitmap::FindHandler( type );

        if ( handler )
        {
            wxBitmap bmp ;
            if ( handler->LoadFile( &bmp , filename, type, desiredWidth, desiredHeight ))
            {
                CopyFromBitmap( bmp ) ;

                return true ;
            }

            return false ;
        }
        else
        {
#if wxUSE_IMAGE
            wxImage loadimage( filename, type );
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
#endif
        }
    }
    return true ;
}

void wxIcon::CopyFromBitmap( const wxBitmap& bmp )
{
    UnRef() ;

    // as the bitmap owns that ref, we have to acquire it as well
    IconRef iconRef = bmp.CreateIconRef() ;
    m_refData = new wxIconRefData( (WXHICON) iconRef, bmp.GetWidth(), bmp.GetHeight()  ) ;
}

IMPLEMENT_DYNAMIC_CLASS(wxICONResourceHandler, wxBitmapHandler)

bool  wxICONResourceHandler::LoadFile(
    wxBitmap *bitmap, const wxString& name, wxBitmapType WXUNUSED(flags),
    int desiredWidth, int desiredHeight )
{
    wxIcon icon ;
    icon.LoadFile( name , wxBITMAP_TYPE_ICON_RESOURCE , desiredWidth , desiredHeight ) ;
    bitmap->CopyFromIcon( icon ) ;

    return bitmap->Ok() ;
}

