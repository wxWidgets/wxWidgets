/////////////////////////////////////////////////////////////////////////////
// Name:        clipbrd.cpp
// Purpose:     Clipboard functionality
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "clipbrd.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/setup.h"
#endif

#if wxUSE_CLIPBOARD

#ifndef WX_PRECOMP
#include "wx/object.h"
#include "wx/event.h"
#include "wx/app.h"
#include "wx/frame.h"
#include "wx/bitmap.h"
#include "wx/utils.h"
#endif

#if wxUSE_METAFILE
#include "wx/metafile.h"
#endif

#include "wx/clipbrd.h"

#include <windows.h>

HICON myIcon;

#include "wx/msw/private.h"
#include "wx/msw/dib.h"

// wxDataObject is tied to OLE/drag and drop implementation,
// therefore so is wxClipboard :-(
#if wxUSE_DRAG_AND_DROP
#include "wx/dataobj.h"
#endif

#include <string.h>

bool wxClipboardIsOpen = FALSE;

bool wxOpenClipboard(void)
{
  if (wxTheApp->GetTopWindow() && !wxClipboardIsOpen)
  {
    wxClipboardIsOpen = (::OpenClipboard((HWND) wxTheApp->GetTopWindow()->GetHWND()) != 0);
    return wxClipboardIsOpen;
  }
  else return FALSE;
}

bool wxCloseClipboard(void)
{
  if (wxClipboardIsOpen)
  {
    wxClipboardIsOpen = FALSE;
  }
  return (::CloseClipboard() != 0);
}

bool wxEmptyClipboard(void)
{
  return (::EmptyClipboard() != 0);
}

bool wxClipboardOpen(void)
{
  return wxClipboardIsOpen;
}

bool wxIsClipboardFormatAvailable(wxDataFormat dataFormat)
{
  return (::IsClipboardFormatAvailable(dataFormat) != 0);
}

bool wxSetClipboardData(wxDataFormat dataFormat, wxObject *obj, int width, int height)
{
  switch (dataFormat)
  {
    case wxDF_BITMAP:
    {
      wxBitmap *wxBM = (wxBitmap *)obj;

      HDC hdcMem = CreateCompatibleDC(NULL);
      HDC hdcSrc = CreateCompatibleDC(NULL);
      HBITMAP old = (HBITMAP) ::SelectObject(hdcSrc, (HBITMAP) wxBM->GetHBITMAP());
      HBITMAP hBitmap = CreateCompatibleBitmap(hdcSrc,
                                              wxBM->GetWidth(), wxBM->GetHeight());
      if (!hBitmap)
      {
        SelectObject(hdcSrc, old);
        DeleteDC(hdcMem);
        DeleteDC(hdcSrc);
        return FALSE;
      }
      HBITMAP old1 = (HBITMAP) SelectObject(hdcMem, hBitmap);
      BitBlt(hdcMem, 0, 0, wxBM->GetWidth(), wxBM->GetHeight(),
             hdcSrc, 0, 0, SRCCOPY);

      // Select new bitmap out of memory DC
      SelectObject(hdcMem, old1);

      // Set the data
      bool success = (bool)(::SetClipboardData(CF_BITMAP, hBitmap) != 0);

      // Clean up
      SelectObject(hdcSrc, old);
      DeleteDC(hdcSrc);
      DeleteDC(hdcMem);      
      return success;
      break;
    }
    case wxDF_DIB:
    {
#if wxUSE_IMAGE_LOADING_IN_MSW
      HBITMAP hBitmap=(HBITMAP) ((wxBitmap *)obj)->GetHBITMAP();
      HANDLE hDIB=BitmapToDIB(hBitmap,NULL); // NULL==uses system palette
      bool success = (::SetClipboardData(CF_DIB,hDIB) != 0);
#else
      bool success=FALSE;
#endif
      return success;
      break;
    }
#if wxUSE_METAFILE
    case wxDF_METAFILE:
    {
      wxMetafile *wxMF = (wxMetafile *)obj;
      HANDLE data = GlobalAlloc(GHND, sizeof(METAFILEPICT) + 1);
#ifdef __WINDOWS_386__
      METAFILEPICT *mf = (METAFILEPICT *)MK_FP32(GlobalLock(data));
#else
      METAFILEPICT *mf = (METAFILEPICT *)GlobalLock(data);
#endif

      mf->mm = wxMF->GetWindowsMappingMode();
      mf->xExt = width;
      mf->yExt = height;
      mf->hMF = (HMETAFILE) wxMF->GetHMETAFILE();
      GlobalUnlock(data);
      wxMF->SetHMETAFILE((WXHANDLE) NULL);

      return (SetClipboardData(CF_METAFILEPICT, data) != 0);
      break;
    }
#endif
    case CF_SYLK:
    case CF_DIF:
    case CF_TIFF:
    case CF_PALETTE:
    {
      return FALSE;
      break;
    }
    case wxDF_OEMTEXT:
      dataFormat = wxDF_TEXT;
    case wxDF_TEXT:
       width = strlen((char *)obj) + 1;
       height = 1;
    default:
    {
      char *s = (char *)obj;
      DWORD l;

      l = (width * height);
      HANDLE hGlobalMemory = GlobalAlloc(GHND, l);
                if (!hGlobalMemory)
        return FALSE;

#ifdef __WINDOWS_386__
      LPSTR lpGlobalMemory = (LPSTR)MK_FP32(GlobalLock(hGlobalMemory));
#else
      LPSTR lpGlobalMemory = (LPSTR)GlobalLock(hGlobalMemory);
#endif

#ifdef __WIN32__
      memcpy(lpGlobalMemory, s, l);
#elif defined(__WATCOMC__) && defined(__WINDOWS_386__)
      memcpy(lpGlobalMemory, s, l);
#else
      hmemcpy(lpGlobalMemory, s, l);
#endif

      GlobalUnlock(hGlobalMemory);
      HANDLE success = SetClipboardData(dataFormat, hGlobalMemory);
      return (success != 0);
      break;
    }
  }
  return FALSE;
}

wxObject *wxGetClipboardData(wxDataFormat dataFormat, long *len)
{
  switch (dataFormat)
  {
    case wxDF_BITMAP:
    {
      BITMAP bm;
      HBITMAP hBitmap = (HBITMAP) GetClipboardData(CF_BITMAP);
      if (!hBitmap)
        return NULL;

      HDC hdcMem = CreateCompatibleDC(NULL);
      HDC hdcSrc = CreateCompatibleDC(NULL);

      HBITMAP old = (HBITMAP) ::SelectObject(hdcSrc, hBitmap);
      GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bm);

      HBITMAP hNewBitmap = CreateBitmapIndirect(&bm);

      if (!hNewBitmap)
      {
        SelectObject(hdcSrc, old);
        DeleteDC(hdcMem);
        DeleteDC(hdcSrc);
        return NULL;
      }

      HBITMAP old1 = (HBITMAP) SelectObject(hdcMem, hNewBitmap);
      BitBlt(hdcMem, 0, 0, bm.bmWidth, bm.bmHeight,
             hdcSrc, 0, 0, SRCCOPY);

      // Select new bitmap out of memory DC
      SelectObject(hdcMem, old1);

      // Clean up
      SelectObject(hdcSrc, old);
      DeleteDC(hdcSrc);
      DeleteDC(hdcMem);

      // Create and return a new wxBitmap
      wxBitmap *wxBM = new wxBitmap;
      wxBM->SetHBITMAP((WXHBITMAP) hNewBitmap);
      wxBM->SetWidth(bm.bmWidth);
      wxBM->SetHeight(bm.bmHeight);
      wxBM->SetDepth(bm.bmPlanes);
      wxBM->SetOk(TRUE);
      return (wxObject *)wxBM;
      break;
    }
    case wxDF_METAFILE:
    case CF_SYLK:
    case CF_DIF:
    case CF_TIFF:
    case CF_PALETTE:
    case wxDF_DIB:
    {
      return FALSE;
      break;
    }
    case wxDF_OEMTEXT:
      dataFormat = wxDF_TEXT;
         case wxDF_TEXT:
    default:
    {
      HANDLE hGlobalMemory = GetClipboardData(dataFormat);
      if (!hGlobalMemory)
        return NULL;

      int hsize = (int)GlobalSize(hGlobalMemory);
      if (len)
        *len = hsize;

      char *s = new char[hsize];
      if (!s)
        return NULL;

#ifdef __WINDOWS_386__
      LPSTR lpGlobalMemory = (LPSTR)MK_FP32(GlobalLock(hGlobalMemory));
#else
      LPSTR lpGlobalMemory = (LPSTR)GlobalLock(hGlobalMemory);
#endif

#ifdef __WIN32__
      memcpy(s, lpGlobalMemory, GlobalSize(hGlobalMemory));
#elif __WATCOMC__ && defined(__WINDOWS_386__)
      memcpy(s, lpGlobalMemory, GlobalSize(hGlobalMemory));
#else
      hmemcpy(s, lpGlobalMemory, GlobalSize(hGlobalMemory));
#endif

      GlobalUnlock(hGlobalMemory);

      return (wxObject *)s;
      break;
    }
  }
  return NULL;
}

wxDataFormat  wxEnumClipboardFormats(wxDataFormat dataFormat)
{
  return (wxDataFormat) ::EnumClipboardFormats(dataFormat);
}

int  wxRegisterClipboardFormat(char *formatName)
{
  return ::RegisterClipboardFormat(formatName);
}

bool wxGetClipboardFormatName(wxDataFormat dataFormat, char *formatName, int maxCount)
{
  return (::GetClipboardFormatName((int) dataFormat, formatName, maxCount) > 0);
}

/*
 * wxClipboard
 */

//-----------------------------------------------------------------------------
// wxClipboard
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxClipboard,wxObject)

wxClipboard* wxTheClipboard = (wxClipboard*) NULL;

wxClipboard::wxClipboard()
{
    m_open = FALSE;
}

wxClipboard::~wxClipboard()
{
    Clear();  
}

void wxClipboard::Clear()
{
    wxNode* node = m_data.First();
    while (node)
    {
        wxDataObject* data = (wxDataObject*) node->Data();
        delete data;
        node = node->Next();
    }
    m_data.Clear();
}

bool wxClipboard::Open()
{
    wxCHECK_MSG( !m_open, FALSE, "clipboard already open" );
  
    m_open = TRUE;

    return wxOpenClipboard();
}

bool wxClipboard::SetData( wxDataObject *data )
{
#if wxUSE_DRAG_AND_DROP
    wxCHECK_MSG( data, FALSE, "data is invalid" );
    wxCHECK_MSG( m_open, FALSE, "clipboard not open" );

    switch (data->GetFormat())
    {
        case wxDF_TEXT:
        case wxDF_OEMTEXT:
        {
            wxTextDataObject* textDataObject = (wxTextDataObject*) data;
            wxString str(textDataObject->GetText());
            return wxSetClipboardData(data->GetFormat(), (wxObject*) (const char*) str);
            break;
        }
        case wxDF_BITMAP:
        case wxDF_DIB:
        {
            wxBitmapDataObject* bitmapDataObject = (wxBitmapDataObject*) data;
            wxBitmap bitmap(bitmapDataObject->GetBitmap());
            return wxSetClipboardData(data->GetFormat(), & bitmap);
            break;
        }
#if wxUSE_METAFILE
        case wxDF_METAFILE:
        {
            wxMetafileDataObject* metaFileDataObject = (wxMetafileDataObject*) data;
            wxMetafile metaFile = metaFileDataObject->GetMetafile();
            return wxSetClipboardData(wxDF_METAFILE, & metaFile, metaFileDataObject->GetWidth(), metaFileDataObject->GetHeight());
            break;
        }
#endif
        default:
        {
            return FALSE;
        }
    }
  
    return FALSE;
#else
    return FALSE;
#endif
}

void wxClipboard::Close()
{
    wxCHECK_RET( m_open, "clipboard not open" );
    
    m_open = FALSE;
    wxCloseClipboard();
}

bool wxClipboard::IsSupportedFormat( wxDataFormat format, const wxString& WXUNUSED(id) )
{
    return wxIsClipboardFormatAvailable(format);
}

bool wxClipboard::GetData( wxDataObject *data )
{
    wxCHECK_MSG( m_open, FALSE, "clipboard not open" );
    
#if wxUSE_DRAG_AND_DROP
    switch (data->GetFormat())
    {
        case wxDF_TEXT:
        case wxDF_OEMTEXT:
        {
            wxTextDataObject* textDataObject = (wxTextDataObject*) data;
            char* s = (char*) wxGetClipboardData(data->GetFormat());
            if (s)
            {
                textDataObject->SetText(s);
                delete[] s;
                return TRUE;
            }
            else
                return FALSE;
            break;
        }
        case wxDF_BITMAP:
        case wxDF_DIB:
        {
            wxBitmapDataObject* bitmapDataObject = (wxBitmapDataObject*) data;
            wxBitmap* bitmap = (wxBitmap*) wxGetClipboardData(data->GetFormat());
            if (bitmap)
            {
                bitmapDataObject->SetBitmap(* bitmap);
                delete bitmap;
                return TRUE;
            }
            else
                return FALSE;
            break;
        }
#if wxUSE_METAFILE
        case wxDF_METAFILE:
        {
            wxMetafileDataObject* metaFileDataObject = (wxMetafileDataObject*) data;
            wxMetafile* metaFile = (wxMetafile*) wxGetClipboardData(wxDF_METAFILE);
            if (metaFile)
            {
                metaFileDataObject->SetMetafile(* metaFile);
                delete metaFile;
                return TRUE;
            }
            else
                return FALSE;

            break;
        }
#endif
        default:
        {
            return FALSE;
        }
    }
    return FALSE;
#else
    return FALSE;
#endif
}

//-----------------------------------------------------------------------------
// wxClipboardModule
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxClipboardModule,wxModule)

bool wxClipboardModule::OnInit()
{
    wxTheClipboard = new wxClipboard();
  
    return TRUE;
}

void wxClipboardModule::OnExit()
{
    if (wxTheClipboard) delete wxTheClipboard;
    wxTheClipboard = (wxClipboard*) NULL;
}

#endif // wxUSE_CLIPBOARD

