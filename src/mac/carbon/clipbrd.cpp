/////////////////////////////////////////////////////////////////////////////
// Name:        clipbrd.cpp
// Purpose:     Clipboard functionality
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#pragma implementation "clipbrd.h"
#endif

#include "wx/app.h"
#include "wx/frame.h"
#include "wx/bitmap.h"
#include "wx/utils.h"
#include "wx/metafile.h"
#include "wx/clipbrd.h"
#include "wx/intl.h"
#include "wx/log.h"

#ifndef __DARWIN__
#include <Scrap.h>
#endif
#include "wx/mac/uma.h"

#define wxUSE_DATAOBJ 1

#include <string.h>

// the trace mask we use with wxLogTrace() - call
// wxLog::AddTraceMask(TRACE_CLIPBOARD) to enable the trace messages from here
// (there will be a *lot* of them!)
static const wxChar *TRACE_CLIPBOARD = _T("clipboard");

void *wxGetClipboardData(wxDataFormat dataFormat, long *len)
{
#if !TARGET_CARBON
    OSErr err = noErr ;
#else
    OSStatus err = noErr ;
#endif
  	void * data = NULL ;
	Size byteCount;
    
    switch (dataFormat.GetType())
    {
        case wxDF_OEMTEXT:
            dataFormat = wxDF_TEXT;
            // fall through

        case wxDF_TEXT:
                break;
        case wxDF_BITMAP :
        case wxDF_METAFILE :
        		break ;
        default:
            {
                wxLogError(_("Unsupported clipboard format."));
                return NULL;
            }
    }

#if TARGET_CARBON
    ScrapRef scrapRef;
    
    err = GetCurrentScrap( &scrapRef );
    if ( err != noTypeErr && err != memFullErr )    
    {
        ScrapFlavorFlags    flavorFlags;
        
        if (( err = GetScrapFlavorFlags( scrapRef, dataFormat.GetFormatId(), &flavorFlags )) == noErr)
        {
            if (( err = GetScrapFlavorSize( scrapRef, dataFormat.GetFormatId(), &byteCount )) == noErr)
            {
              if ( dataFormat.GetType() == wxDF_TEXT )
                byteCount++ ;
                
          data = new char[ byteCount ] ;
          if (( err = GetScrapFlavorData( scrapRef, dataFormat.GetFormatId(), &byteCount , data )) == noErr )
          {
            *len = byteCount ;
                if ( dataFormat.GetType() == wxDF_TEXT )  
                ((char*)data)[byteCount] = 0 ;
          }
          else
          {
            delete[] ((char *)data) ;
            data = NULL ;
          }
            }
        }
    }
    
#else
    long offset ;
    Handle datahandle = NewHandle(0) ;
    HLock( datahandle ) ;
    GetScrap( datahandle , dataFormat.GetFormatId() , &offset ) ;
    HUnlock( datahandle ) ;
    if ( GetHandleSize( datahandle ) > 0 )
    {
      byteCount = GetHandleSize( datahandle ) ;
      if ( dataFormat.GetType() == wxDF_TEXT )  
        data = new char[ byteCount + 1] ;
      else
        data = new char[ byteCount ] ;

      memcpy( (char*) data , (char*) *datahandle , byteCount ) ;
      if ( dataFormat.GetType() == wxDF_TEXT )  
          ((char*)data)[byteCount] = 0 ;
      * len = byteCount ;
    }
    DisposeHandle( datahandle ) ;
#endif
    if ( err )
    {
        wxLogSysError(_("Failed to get clipboard data."));

        return NULL ;
    }
    if ( dataFormat.GetType() == wxDF_TEXT && wxApp::s_macDefaultEncodingIsPC )
    {
      wxMacConvertToPC((char*)data,(char*)data,byteCount) ;
    }
    return data;
}


/*
 * Generalized clipboard implementation by Matthew Flatt
 */

IMPLEMENT_DYNAMIC_CLASS(wxClipboard, wxObject)

wxClipboard::wxClipboard()
{
  m_open = false ;
  m_data = NULL ;
}

wxClipboard::~wxClipboard()
{
    if (m_data)
    {
        delete m_data;
        m_data = (wxDataObject*) NULL;
    }
}

void wxClipboard::Clear()
{
    if (m_data)
    {
        delete m_data;
        m_data = (wxDataObject*) NULL;
    }
#if TARGET_CARBON
    OSStatus err ;
    err = ClearCurrentScrap( );
#else
    OSErr err ;
    err = ZeroScrap( );
#endif
    if ( err )
    {
        wxLogSysError(_("Failed to empty the clipboard."));
    }
}

bool wxClipboard::Flush()
{
    return FALSE;
}

bool wxClipboard::Open()
{
    wxCHECK_MSG( !m_open, FALSE, wxT("clipboard already open") );
    m_open = true ;
    return true ;
}

bool wxClipboard::IsOpened() const
{
    return m_open;
}

bool wxClipboard::SetData( wxDataObject *data )
{
    wxCHECK_MSG( m_open, FALSE, wxT("clipboard not open") );

    wxCHECK_MSG( data, FALSE, wxT("data is invalid") );

    Clear();

    return AddData( data );
}

bool wxClipboard::AddData( wxDataObject *data )
{
    wxCHECK_MSG( m_open, FALSE, wxT("clipboard not open") );

    wxCHECK_MSG( data, FALSE, wxT("data is invalid") );

    /* we can only store one wxDataObject */
    Clear();

    m_data = data;

    /* get formats from wxDataObjects */
    wxDataFormat *array = new wxDataFormat[ m_data->GetFormatCount() ];
    m_data->GetAllFormats( array );

    for (size_t i = 0; i < m_data->GetFormatCount(); i++)
    {
        wxLogTrace( TRACE_CLIPBOARD,
                    wxT("wxClipboard now supports atom %s"),
                    array[i].GetId().c_str() );

#if !TARGET_CARBON
        OSErr err = noErr ;
#else
        OSStatus err = noErr ;
#endif

       switch ( array[i].GetType() )
       {
           case wxDF_TEXT:
           case wxDF_OEMTEXT:
           {
               wxTextDataObject* textDataObject = (wxTextDataObject*) data;
               wxString str(textDataObject->GetText());
                wxString mac ;
                if ( wxApp::s_macDefaultEncodingIsPC )
                {
                    mac = wxMacMakeMacStringFromPC(textDataObject->GetText()) ;
                }
                else
                {
                    mac = textDataObject->GetText() ;
                }
                err = UMAPutScrap( mac.Length() , 'TEXT' , (void*) mac.c_str()  ) ;
           }
           break ;

#if wxUSE_DRAG_AND_DROP
        case wxDF_METAFILE:
           {
				wxMetafileDataObject* metaFileDataObject =
                (wxMetafileDataObject*) data;
              	wxMetafile metaFile = metaFileDataObject->GetMetafile();
            	PicHandle pict = (PicHandle) metaFile.GetHMETAFILE() ;
              	HLock( (Handle) pict ) ;
              	err = UMAPutScrap( GetHandleSize(  (Handle) pict ) , 'PICT' , *pict ) ;
              	HUnlock(  (Handle) pict ) ;
           }
           break ;
#endif
           case wxDF_BITMAP:
           case wxDF_DIB:
           {
           		PicHandle pict = NULL ;
           		
           		wxBitmapDataObject* bitmapDataObject = (wxBitmapDataObject*) data ;
           		bool created = bitmapDataObject->GetBitmap().GetBitmapType() != kMacBitmapTypePict ;
           		pict = (PicHandle) bitmapDataObject->GetBitmap().GetPict() ;

              	HLock( (Handle) pict ) ;
              	err = UMAPutScrap( GetHandleSize(  (Handle) pict ) , 'PICT' , *pict ) ;
              	HUnlock(  (Handle) pict ) ;
              	if ( created && pict)
              		KillPicture( pict ) ;
           }
           default:
                break ;
       }

    }

    delete[] array;

    return true ;
}

void wxClipboard::Close()
{
    m_open = false ;
}

bool wxClipboard::IsSupported( const wxDataFormat &dataFormat )
{
  if ( m_data )
  {
    return m_data->IsSupported( dataFormat ) ;
  }
#if TARGET_CARBON
    OSStatus err = noErr;
    ScrapRef scrapRef;
    
    err = GetCurrentScrap( &scrapRef );
    if ( err != noTypeErr && err != memFullErr )    
    {
        ScrapFlavorFlags    flavorFlags;
        Size                byteCount;
        
        if (( err = GetScrapFlavorFlags( scrapRef, dataFormat.GetFormatId(), &flavorFlags )) == noErr)
        {
            if (( err = GetScrapFlavorSize( scrapRef, dataFormat.GetFormatId(), &byteCount )) == noErr)
            {
                return TRUE ;
            }
        }
    }
    return FALSE;
    
#else
    long offset ;
    Handle datahandle = NewHandle(0) ;
    HLock( datahandle ) ;
    GetScrap( datahandle , dataFormat.GetFormatId() , &offset ) ;
    HUnlock( datahandle ) ;
    bool hasData = GetHandleSize( datahandle ) > 0 ;
    DisposeHandle( datahandle ) ;
    return hasData ;
#endif
}

bool wxClipboard::GetData( wxDataObject& data )
{
    wxCHECK_MSG( m_open, FALSE, wxT("clipboard not open") );

    size_t formatcount = data.GetFormatCount() + 1 ;
    wxDataFormat *array = new wxDataFormat[ formatcount  ];
    array[0] = data.GetPreferredFormat();
    data.GetAllFormats( &array[1] );

    bool transferred = false ;

    if ( m_data )
    {
      for (size_t i = 0; !transferred && i < formatcount ; i++)
      {
          wxDataFormat format = array[i] ;
          if ( m_data->IsSupported( format ) ) 
          {
            int size = m_data->GetDataSize( format );
            transferred = true ;

            if (size == 0) 
            {
              data.SetData(format , 0 , 0 ) ;
            }
            else
            {
              char *d = new char[size];
              m_data->GetDataHere( format , (void*) d );
              data.SetData( format , size , d ) ;
              delete[] d ;
            }
          }
       }
    }
    /* get formats from wxDataObjects */
    if ( !transferred ) 
    {
      for (size_t i = 0; !transferred && i < formatcount ; i++)
      {
          wxDataFormat format = array[i] ;

          switch ( format.GetType() )
          {
              case wxDF_TEXT :
              case wxDF_OEMTEXT :
              case wxDF_BITMAP :
              case wxDF_METAFILE :
              {
                  long len ;
                  char* s = (char*)wxGetClipboardData(format, &len );
                  if ( s )
                  {
                    data.SetData( format , len , s ) ;
                    delete [] s;

                    transferred = true ;
                  }
              }
							break ;

              default :
                break ;
          }
       }
    }

    delete[] array ;
    return transferred ;
}
