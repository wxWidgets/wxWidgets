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

#include "wx/metafile.h"
#include "wx/clipbrd.h"
#include "wx/msw/private.h"
#include "wx/msw/dib.h"

#include <string.h>

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxClipboard, wxObject)
IMPLEMENT_ABSTRACT_CLASS(wxClipboardClient, wxObject)
#endif

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

bool wxIsClipboardFormatAvailable(int dataFormat)
{
  return (::IsClipboardFormatAvailable(dataFormat) != 0);
}

bool wxSetClipboardData(int dataFormat, wxObject *obj, int width, int height)
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
      wxMetaFile *wxMF = (wxMetaFile *)obj;
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

wxObject *wxGetClipboardData(int dataFormat, long *len)
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

int  wxEnumClipboardFormats(int dataFormat)
{
  return ::EnumClipboardFormats(dataFormat);
}

int  wxRegisterClipboardFormat(char *formatName)
{
  return ::RegisterClipboardFormat(formatName);
}

bool wxGetClipboardFormatName(int dataFormat, char *formatName, int maxCount)
{
  return (::GetClipboardFormatName(dataFormat, formatName, maxCount) > 0);
}

/*
 * Generalized clipboard implementation by Matthew Flatt
 */

wxClipboard *wxTheClipboard = NULL;

void wxInitClipboard(void)
{
  if (!wxTheClipboard)
    wxTheClipboard = new wxClipboard;
}

wxClipboard::wxClipboard()
{
  clipOwner = NULL;
  cbString = NULL;
}

wxClipboard::~wxClipboard()
{
  if (clipOwner)
    clipOwner->BeingReplaced();
  if (cbString)
    delete[] cbString;
}

static int FormatStringToID(char *str)
{
  if (!strcmp(str, "TEXT"))
    return wxDF_TEXT;

  return wxRegisterClipboardFormat(str);
}

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


#endif // wxUSE_CLIPBOARD

