/////////////////////////////////////////////////////////////////////////////
// Name:        icon.cpp
// Purpose:     wxIcon class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "icon.h"
#endif

#include "wx/icon.h"

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxIcon, wxBitmap)
#endif

/*
 * Icons
 */


wxIconRefData::wxIconRefData()
{
    m_ok = FALSE;
    m_width = 0;
    m_height = 0;
    m_depth = 0;
    m_quality = 0;
    m_numColors = 0;
    m_bitmapMask = NULL;
		m_hBitmap = NULL ;
		m_hIcon = NULL ;
}

wxIconRefData::~wxIconRefData()
{
	if ( m_hIcon )
	{
		DisposeCIcon( m_hIcon ) ;
		m_hIcon = NULL ;
	}
		
  if (m_bitmapMask)
  {
    delete m_bitmapMask;
    m_bitmapMask = NULL;
  }
}

wxIcon::wxIcon()
{
}

wxIcon::wxIcon(const char WXUNUSED(bits)[], int WXUNUSED(width), int WXUNUSED(height))
{
}

wxIcon::wxIcon( const char **bits, int width, int height )
{
}

wxIcon::wxIcon( char **bits, int width, int height )
{
}

wxIcon::wxIcon(const wxString& icon_file, long flags,
    int desiredWidth, int desiredHeight)

{
    LoadFile(icon_file, flags, desiredWidth, desiredHeight);
}

wxIcon::~wxIcon()
{
}

bool wxIcon::LoadFile(const wxString& filename, long type,
    int desiredWidth, int desiredHeight)
{
  UnRef();

  m_refData = new wxIconRefData;

  wxBitmapHandler *handler = FindHandler(type);

  if ( handler )
	return handler->LoadFile(this, filename, type, desiredWidth, desiredHeight);
  else
	return FALSE;
}

IMPLEMENT_DYNAMIC_CLASS(wxICONResourceHandler, wxBitmapHandler)

bool  wxICONResourceHandler::LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
          int desiredWidth, int desiredHeight)
{
	Str255 theName ;
	short theId ;
	OSType theType ;

#if TARGET_CARBON
	c2pstrcpy( (StringPtr) theName , name ) ;
#else
	strcpy( (char *) theName , name ) ;
	c2pstr( (char *) theName ) ;
#endif
	
	Handle resHandle = GetNamedResource( 'cicn' , theName ) ;
	if ( resHandle != 0L )
	{
		GetResInfo( resHandle , &theId , &theType , theName ) ;
		ReleaseResource( resHandle ) ;
		
		CIconHandle theIcon = (CIconHandle ) GetCIcon( theId ) ;
		if ( theIcon )
		{
			M_ICONHANDLERDATA->m_hIcon = theIcon ;
			M_ICONHANDLERDATA->m_width =  32 ;
			M_ICONHANDLERDATA->m_height = 32 ;
			
			M_ICONHANDLERDATA->m_depth = 8 ;
			M_ICONHANDLERDATA->m_ok = true ;
			M_ICONHANDLERDATA->m_numColors = 256 ;
			return TRUE ;
		}
	}
	return FALSE ;
}
