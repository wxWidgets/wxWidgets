/////////////////////////////////////////////////////////////////////////////
// Name:        clipbrd.cpp
// Purpose:     Clipboard functionality
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
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

#define wxUSE_DATAOBJ 1

#include <string.h>

// open/close

bool clipboard_opened = false ;

bool wxOpenClipboard()
{
	clipboard_opened = true ;
    return TRUE;
}

bool wxCloseClipboard()
{
	clipboard_opened = false ;
    return TRUE;
}

bool wxIsClipboardOpened()
{
     return clipboard_opened;
}

bool wxEmptyClipboard()
{
	
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
		return FALSE ;
	}
    return TRUE;
}

// get/set data

// clipboard formats

bool wxIsClipboardFormatAvailable(wxDataFormat dataFormat)
{
#if TARGET_CARBON
	OSStatus err = noErr;
	ScrapRef scrapRef;
	
	err = GetCurrentScrap( &scrapRef );
	if ( err != noTypeErr && err != memFullErr )	
	{
		ScrapFlavorFlags	flavorFlags;
		Size				byteCount;
		
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
	if ( GetScrap( NULL , dataFormat.GetFormatId() , &offset ) > 0 )
	{
		return TRUE ;
	}
     return FALSE;
#endif
}

bool wxSetClipboardData(wxDataFormat dataFormat,const void *data,int width , int height)
{
#if !TARGET_CARBON
	OSErr err = noErr ;
#else
	OSStatus err = noErr ;
#endif
	
    switch (dataFormat.GetType())
    {
        case wxDF_BITMAP:
            {
            	/*
                wxBitmap *bitmap = (wxBitmap *)data;

                HDC hdcMem = CreateCompatibleDC((HDC) NULL);
                HDC hdcSrc = CreateCompatibleDC((HDC) NULL);
                HBITMAP old = (HBITMAP)
                    ::SelectObject(hdcSrc, (HBITMAP)bitmap->GetHBITMAP());
                HBITMAP hBitmap = CreateCompatibleBitmap(hdcSrc,
                                                         bitmap->GetWidth(),
                                                         bitmap->GetHeight());
                if (!hBitmap)
                {
                    SelectObject(hdcSrc, old);
                    DeleteDC(hdcMem);
                    DeleteDC(hdcSrc);
                    return FALSE;
                }

                HBITMAP old1 = (HBITMAP) SelectObject(hdcMem, hBitmap);
                BitBlt(hdcMem, 0, 0, bitmap->GetWidth(), bitmap->GetHeight(),
                       hdcSrc, 0, 0, SRCCOPY);

                // Select new bitmap out of memory DC
                SelectObject(hdcMem, old1);

                // Set the data
                handle = ::SetClipboardData(CF_BITMAP, hBitmap);

                // Clean up
                SelectObject(hdcSrc, old);
                DeleteDC(hdcSrc);
                DeleteDC(hdcMem);
                */
                break;
            }

        case wxDF_DIB:
            {
            	/*
#if wxUSE_IMAGE_LOADING_IN_MSW
                wxBitmap *bitmap = (wxBitmap *)data;
                HBITMAP hBitmap = (HBITMAP)bitmap->GetHBITMAP();
                // NULL palette means to use the system one
                HANDLE hDIB = wxBitmapToDIB(hBitmap, (HPALETTE)NULL); 
                handle = SetClipboardData(CF_DIB, hDIB);
#endif // wxUSE_IMAGE_LOADING_IN_MSW
*/
                break;
            }

#if wxUSE_METAFILE
        case wxDF_METAFILE:
            {
                wxMetafile *wxMF = (wxMetafile *)data;
				PicHandle pict = wxMF->GetHMETAFILE() ;
				HLock( (Handle) pict ) ;
#if !TARGET_CARBON
				err = PutScrap( GetHandleSize(  (Handle) pict ) , 'PICT' , *pict ) ;
#else
				ScrapRef	scrap;
				err = GetCurrentScrap (&scrap); 
				if ( !err )
				{
					err = PutScrapFlavor (scrap, 'PICT', 0, GetHandleSize((Handle) pict), *pict);
				}
#endif
				HUnlock(  (Handle) pict ) ;
                break;
            }
#endif
        case wxDF_SYLK:
        case wxDF_DIF:
        case wxDF_TIFF:
        case wxDF_PALETTE:
        default:
            {
                wxLogError(_("Unsupported clipboard format."));
                return FALSE;
            }

        case wxDF_OEMTEXT:
            dataFormat = wxDF_TEXT;
            // fall through

        case wxDF_TEXT:
            {
            	wxString mac ;
            	if ( wxApp::s_macDefaultEncodingIsPC )
            	{
            		mac = wxMacMakeMacStringFromPC((char *)data) ;
            	}
            	else
            	{
            		mac = (char *)data ;
            	}
#if !TARGET_CARBON
				err = PutScrap( mac.Length() , 'TEXT' , mac.c_str() ) ;
#else
				ScrapRef	scrap;
				err = GetCurrentScrap (&scrap); 
				if ( !err )
				{
					err = PutScrapFlavor (scrap, 'TEXT', 0, mac.Length(), mac.c_str());
				}
#endif
                break;
            }
    }

    if ( err )
    {
        wxLogSysError(_("Failed to set clipboard data."));

        return FALSE;
    }

    return TRUE;
}

wxDataFormat wxEnumClipboardFormats(wxDataFormat dataFormat)
{
    return wxDataFormat();
}

int  wxRegisterClipboardFormat(wxChar *formatName)
{
    return 0;
}

bool wxGetClipboardFormatName(wxDataFormat dataFormat, wxChar *formatName, int maxCount)
{
    return FALSE;
}

void *wxGetClipboardData(wxDataFormat dataFormat, long *len)
{
    return NULL;
}


/*
 * Generalized clipboard implementation by Matthew Flatt
 */

IMPLEMENT_DYNAMIC_CLASS(wxClipboard, wxClipboardBase)

wxClipboard::wxClipboard()
{
    m_clearOnExit = FALSE;
}

wxClipboard::~wxClipboard()
{
    if ( m_clearOnExit )
    {
        Clear();
    }
}

void wxClipboard::Clear()
{
}

bool wxClipboard::Flush()
{
    return FALSE;
}

bool wxClipboard::Open()
{
    return wxOpenClipboard();
}

bool wxClipboard::IsOpened() const
{
    return wxIsClipboardOpened();
}

static int FormatStringToID(char *str)
{
  if (!strcmp(str, "TEXT"))
    return wxDF_TEXT;

  return wxRegisterClipboardFormat(str);
}

bool wxClipboard::SetData( wxDataObject *data )
{
    (void)wxEmptyClipboard();

    if ( data )
        return AddData(data);
    else
        return TRUE;
}

bool wxClipboard::AddData( wxDataObject *data )
{
    wxCHECK_MSG( data, FALSE, wxT("data is invalid") );

#if wxUSE_DATAOBJ
    wxCHECK_MSG( wxIsClipboardOpened(), FALSE, wxT("clipboard not open") );

    wxDataFormat format = data->GetPreferredFormat();

    switch ( format.GetType() )
    {
        case wxDF_TEXT:
        case wxDF_OEMTEXT:
        {
            wxTextDataObject* textDataObject = (wxTextDataObject*) data;
            wxString str(textDataObject->GetText());
            return wxSetClipboardData(format, str.c_str());
        }

        case wxDF_BITMAP:
        case wxDF_DIB:
        {
            wxBitmapDataObject* bitmapDataObject = (wxBitmapDataObject*) data;
            wxBitmap bitmap(bitmapDataObject->GetBitmap());
            return wxSetClipboardData(format, &bitmap);
        }

#if 0 // wxUSE_METAFILE
        case wxDF_METAFILE:
        {
            wxMetafileDataObject* metaFileDataObject = 
                (wxMetafileDataObject*) data;
            wxMetafile metaFile = metaFileDataObject->GetMetafile();
            return wxSetClipboardData(wxDF_METAFILE, &metaFile,
                                      metaFileDataObject->GetWidth(),
                                      metaFileDataObject->GetHeight());
        }
#endif // wxUSE_METAFILE

        default:
 //           return wxSetClipboardData(data);
 				break ;
    }
#else // !wxUSE_DATAOBJ
#endif 
    return FALSE;
}

void wxClipboard::Close()
{
    wxCloseClipboard();
}

bool wxClipboard::IsSupported( const wxDataFormat &format )
{
    return wxIsClipboardFormatAvailable(format);
}

bool wxClipboard::GetData( wxDataObject& data )
{
#if wxUSE_DATAOBJ
    wxCHECK_MSG( wxIsClipboardOpened(), FALSE, wxT("clipboard not open") );

    wxDataFormat format = data.GetPreferredFormat();
    switch ( format )
    {
        case wxDF_TEXT:
        case wxDF_OEMTEXT:
        {
            wxTextDataObject& textDataObject = (wxTextDataObject &)data;
            char* s = (char*)wxGetClipboardData(format);
            if ( !s )
                return FALSE;

            textDataObject.SetText(s);
            delete [] s;

            return TRUE;
        }

        case wxDF_BITMAP:
        case wxDF_DIB:
        {
            wxBitmapDataObject& bitmapDataObject = (wxBitmapDataObject &)data;
            wxBitmap* bitmap = (wxBitmap *)wxGetClipboardData(format );
            if ( !bitmap )
                return FALSE;

            bitmapDataObject.SetBitmap(*bitmap);
            delete bitmap;

            return TRUE;
        }
#if 0 // wxUSE_METAFILE
        case wxDF_METAFILE:
        {
            wxMetafileDataObject& metaFileDataObject = (wxMetafileDataObject &)data;
            wxMetafile* metaFile = (wxMetafile *)wxGetClipboardData(wxDF_METAFILE);
            if ( !metaFile )
                return FALSE;

            metaFileDataObject.SetMetafile(*metaFile);
            delete metaFile;

            return TRUE;
        }
#endif // wxUSE_METAFILE
    }
#else // !wxUSE_DATAOBJ
    wxFAIL_MSG( wxT("no clipboard implementation") );
#endif
    return FALSE;
}
/*
void wxClipboard::SetClipboardClient(wxClipboardClient *client, long time)
{
  bool got_selection;

  if (clipOwner)
    clipOwner->BeingReplaced();
  clipOwner = client;
  if (cbString) {
    delete[] cbString;
    cbString = NULL;
  }

  if (wxOpenClipboard()) {
    char **formats, *data;
         int i;
    int ftype;
    long size;

    formats = clipOwner->formats.ListToArray(FALSE);
    for (i = clipOwner->formats.Number(); i--; ) {
      ftype = FormatStringToID(formats[i]);
      data = clipOwner->GetData(formats[i], &size);
      if (!wxSetClipboardData(ftype, (wxObject *)data, size, 1)) {
        got_selection = FALSE;
        break;
      }
    }

    if (i < 0)
      got_selection = wxCloseClipboard();
  } else
    got_selection = FALSE;
  
  got_selection = FALSE; // Assume another process takes over

  if (!got_selection) {
    clipOwner->BeingReplaced();
         clipOwner = NULL;
  }
}

wxClipboardClient *wxClipboard::GetClipboardClient()
{
  return clipOwner;
}

void wxClipboard::SetClipboardString(char *str, long time)
{
  bool got_selection;

  if (clipOwner) {
    clipOwner->BeingReplaced();
    clipOwner = NULL;
  }
  if (cbString)
    delete[] cbString;

  cbString = str;

  if (wxOpenClipboard()) {
    if (!wxSetClipboardData(wxDF_TEXT, (wxObject *)str))
      got_selection = FALSE;
    else
                got_selection = wxCloseClipboard();
  } else
    got_selection = FALSE;

  got_selection = FALSE; // Assume another process takes over

  if (!got_selection) {
    delete[] cbString;
    cbString = NULL;
  }
}
char *wxClipboard::GetClipboardString(long time)
{
  char *str;
  long length;

  str = GetClipboardData("TEXT", &length, time);
  if (!str) {
    str = new char[1];
    *str = 0;
  }

  return str;
}


char *wxClipboard::GetClipboardData(char *format, long *length, long time)
{
  if (clipOwner)  {
         if (clipOwner->formats.Member(format))
      return clipOwner->GetData(format, length);
    else
      return NULL;
  } else if (cbString) {
    if (!strcmp(format, "TEXT"))
      return copystring(cbString);
    else
      return NULL;
  } else {
    if (wxOpenClipboard()) {
      receivedString = (char *)wxGetClipboardData(FormatStringToID(format), 
                                                  length);
      wxCloseClipboard();
    } else
      receivedString = NULL;

    return receivedString;
  }
}
*/

