/////////////////////////////////////////////////////////////////////////////
// Name:        clipbrd.cpp
// Purpose:     Clipboard functionality
// Author:      David Webster
// Modified by:
// Created:     10/13/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

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
    #include "wx/intl.h"
#endif

#if wxUSE_METAFILE
    #include "wx/metafile.h"
#endif

#include "wx/log.h"
#include "wx/clipbrd.h"

#include <string.h>

#define INCL_DOS
#define INCL_PM
#include <os2.h>

#include "wx/os2/private.h"

// wxDataObject is tied to OLE/drag and drop implementation,
// therefore so is wxClipboard :-(
#if wxUSE_DRAG_AND_DROP
    #include "wx/dataobj.h"

    static bool wxSetClipboardData(wxDataObject *data);
#endif

// ===========================================================================
// implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// old-style clipboard functions using Windows API
// ---------------------------------------------------------------------------

static bool gs_wxClipboardIsOpen = FALSE;

bool wxOpenClipboard()
{
    wxCHECK_MSG( !gs_wxClipboardIsOpen, TRUE, wxT("clipboard already opened.") );
// TODO:
/*
    wxWindow *win = wxTheApp->GetTopWindow();
    if ( win )
    {
        gs_wxClipboardIsOpen = ::OpenClipboard((HWND)win->GetHWND()) != 0;

        if ( !gs_wxClipboardIsOpen )
            wxLogSysError(_("Failed to open the clipboard."));

        return gs_wxClipboardIsOpen;
    }
    else
    {
        wxLogDebug(wxT("Can not open clipboard without a main window."));

        return FALSE;
    }
*/
    return FALSE;
}

bool wxCloseClipboard()
{
    wxCHECK_MSG( gs_wxClipboardIsOpen, FALSE, wxT("clipboard is not opened") );
// TODO:
/*
    gs_wxClipboardIsOpen = FALSE;

    if ( ::CloseClipboard() == 0 )
    {
        wxLogSysError(_("Failed to close the clipboard."));

        return FALSE;
    }
*/
    return TRUE;
}

bool wxEmptyClipboard()
{
// TODO:
/*
    if ( ::EmptyClipboard() == 0 )
    {
        wxLogSysError(_("Failed to empty the clipboard."));

        return FALSE;
    }
*/
    return TRUE;
}

bool wxIsClipboardOpened()
{
  return gs_wxClipboardIsOpen;
}

bool wxIsClipboardFormatAvailable(wxDataFormat dataFormat)
{
    // TODO: return ::IsClipboardFormatAvailable(dataFormat) != 0;
    return FALSE;
}

#if wxUSE_DRAG_AND_DROP
static bool wxSetClipboardData(wxDataObject *data)
{
    // TODO:
/*
    size_t size = data->GetDataSize();
    HANDLE hGlobal = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, size);
    if ( !hGlobal )
    {
        wxLogSysError(_("Failed to allocate %dKb of memory for clipboard "
                        "transfer."), size / 1024);

        return FALSE;
    }

    LPVOID lpGlobalMemory = ::GlobalLock(hGlobal);

    data->GetDataHere(lpGlobalMemory);

    GlobalUnlock(hGlobal);

    wxDataFormat format = data->GetPreferredFormat();
    if ( !::SetClipboardData(format, hGlobal) )
    {
        wxLogSysError(_("Failed to set clipboard data in format %s"),
                      wxDataObject::GetFormatName(format));

        return FALSE;
    }
*/
    return TRUE;
}
#endif // wxUSE_DRAG_AND_DROP

bool wxSetClipboardData(wxDataFormat dataFormat,
                        const void *data,
                        int width, int height)
{
// TODO:
/*
    HANDLE handle = 0; // return value of SetClipboardData
    switch (dataFormat)
    {
        case wxDF_BITMAP:
            {
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
                break;
            }

        case wxDF_DIB:
            {
#if wxUSE_IMAGE_LOADING_IN_MSW
                wxBitmap *bitmap = (wxBitmap *)data;
                HBITMAP hBitmap = (HBITMAP)bitmap->GetHBITMAP();
                // NULL palette means to use the system one
                HANDLE hDIB = wxBitmapToDIB(hBitmap, (HPALETTE)NULL);
                handle = SetClipboardData(CF_DIB, hDIB);
#endif
                break;
            }

#if wxUSE_METAFILE
        case wxDF_METAFILE:
            {
                wxMetafile *wxMF = (wxMetafile *)data;
                HANDLE data = GlobalAlloc(GHND, sizeof(METAFILEPICT) + 1);
                METAFILEPICT *mf = (METAFILEPICT *)GlobalLock(data);

                mf->mm = wxMF->GetWindowsMappingMode();
                mf->xExt = width;
                mf->yExt = height;
                mf->hMF = (HMETAFILE) wxMF->GetHMETAFILE();
                GlobalUnlock(data);
                wxMF->SetHMETAFILE((WXHANDLE) NULL);

                handle = SetClipboardData(CF_METAFILEPICT, data);
                break;
            }
#endif
        case CF_SYLK:
        case CF_DIF:
        case CF_TIFF:
        case CF_PALETTE:
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
                char *s = (char *)data;

                width = strlen(s) + 1;
                height = 1;
                DWORD l = (width * height);
                HANDLE hGlobalMemory = GlobalAlloc(GHND, l);
                if ( hGlobalMemory )
                {
                    LPSTR lpGlobalMemory = (LPSTR)GlobalLock(hGlobalMemory);

                    memcpy(lpGlobalMemory, s, l);

                    GlobalUnlock(hGlobalMemory);
                }

                handle = SetClipboardData(dataFormat, hGlobalMemory);
                break;
            }
    }

    if ( handle == 0 )
    {
        wxLogSysError(_("Failed to set clipboard data."));

        return FALSE;
    }
*/
    return TRUE;
}

void *wxGetClipboardData(wxDataFormat dataFormat, long *len)
{
    void *retval = NULL;
// TODO:
/*
    switch ( dataFormat )
    {
        case wxDF_BITMAP:
            {
                BITMAP bm;
                HBITMAP hBitmap = (HBITMAP) GetClipboardData(CF_BITMAP);
                if (!hBitmap)
                    break;

                HDC hdcMem = CreateCompatibleDC((HDC) NULL);
                HDC hdcSrc = CreateCompatibleDC((HDC) NULL);

                HBITMAP old = (HBITMAP) ::SelectObject(hdcSrc, hBitmap);
                GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bm);

                HBITMAP hNewBitmap = CreateBitmapIndirect(&bm);

                if (!hNewBitmap)
                {
                    SelectObject(hdcSrc, old);
                    DeleteDC(hdcMem);
                    DeleteDC(hdcSrc);
                    break;
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
                retval = wxBM;
                break;
            }

        case wxDF_METAFILE:
        case CF_SYLK:
        case CF_DIF:
        case CF_TIFF:
        case CF_PALETTE:
        case wxDF_DIB:
            {
                wxLogError(_("Unsupported clipboard format."));
                return FALSE;
            }

        case wxDF_OEMTEXT:
            dataFormat = wxDF_TEXT;
            // fall through

        case wxDF_TEXT:
            {
                HANDLE hGlobalMemory = ::GetClipboardData(dataFormat);
                if (!hGlobalMemory)
                    break;

                DWORD hsize = ::GlobalSize(hGlobalMemory);
                if (len)
                    *len = hsize;

                char *s = new char[hsize];
                if (!s)
                    break;

                LPSTR lpGlobalMemory = (LPSTR)::GlobalLock(hGlobalMemory);

                memcpy(s, lpGlobalMemory, hsize);

                ::GlobalUnlock(hGlobalMemory);

                retval = s;
                break;
            }

        default:
            {
                HANDLE hGlobalMemory = ::GetClipboardData(dataFormat);
                if ( !hGlobalMemory )
                    break;

                DWORD size = ::GlobalSize(hGlobalMemory);
                if ( len )
                    *len = size;

                void *buf = malloc(size);
                if ( !buf )
                    break;

                LPSTR lpGlobalMemory = (LPSTR)::GlobalLock(hGlobalMemory);

                memcpy(buf, lpGlobalMemory, size);

                ::GlobalUnlock(hGlobalMemory);

                retval = buf;
                break;
            }
    }

    if ( !retval )
    {
        wxLogSysError(_("Failed to retrieve data from the clipboard."));
    }

    return retval;
*/
    return NULL;
}

wxDataFormat wxEnumClipboardFormats(wxDataFormat dataFormat)
{
  // TODO: return ::EnumClipboardFormats(dataFormat);
  return dataFormat;
}

int wxRegisterClipboardFormat(wxChar *formatName)
{
  // TODO: return ::RegisterClipboardFormat(formatName);
  return 0;
}

bool wxGetClipboardFormatName(wxDataFormat dataFormat,
                              wxChar *formatName,
                              int maxCount)
{
  // TODO: return ::GetClipboardFormatName((int)dataFormat, formatName, maxCount) > 0;
  return 0;
}

// ---------------------------------------------------------------------------
// wxClipboard
// ---------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxClipboard, wxObject)

wxClipboard* wxTheClipboard = (wxClipboard *)NULL;

wxClipboard::wxClipboard()
{
}

wxClipboard::~wxClipboard()
{
    Clear();
}

void wxClipboard::Clear()
{
}

bool wxClipboard::Flush()
{
    // TODO:
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

bool wxClipboard::SetData( wxDataObject *data )
{
    (void)wxEmptyClipboard();
    // TODO:
    /*
    if ( data )
        return AddData(data);
    else
        return TRUE;
    */
    return TRUE;
}

bool wxClipboard::AddData( wxDataObject *data )
{
    wxCHECK_MSG( data, FALSE, wxT("data is invalid") );

#if wxUSE_DRAG_AND_DROP
    wxCHECK_MSG( wxIsClipboardOpened(), FALSE, wxT("clipboard not open") );

//    wxDataFormat format = data->GetFormat();
// TODO:
/*
    switch ( format )
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
            return wxSetClipboardData(data->GetFormat(), &bitmap);
        }

#if wxUSE_METAFILE
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
            return wxSetClipboardData(data);
    }
#else // !wxUSE_DRAG_AND_DROP
*/
    return FALSE;
#endif // wxUSE_DRAG_AND_DROP/!wxUSE_DRAG_AND_DROP
}

void wxClipboard::Close()
{
    wxCloseClipboard();
}

bool wxClipboard::IsSupported( wxDataFormat format )
{
    return wxIsClipboardFormatAvailable(format);
}

bool wxClipboard::GetData( wxDataObject *data )
{
    wxCHECK_MSG( wxIsClipboardOpened(), FALSE, wxT("clipboard not open") );

#if wxUSE_DRAG_AND_DROP
//    wxDataFormat format = data->GetFormat();
    // TODO:
/*
    switch ( format )
    {
        case wxDF_TEXT:
        case wxDF_OEMTEXT:
        {
            wxTextDataObject* textDataObject = (wxTextDataObject*) data;
            char* s = (char*) wxGetClipboardData(format);
            if ( s )
            {
                textDataObject->SetText(s);
                delete[] s;
                return TRUE;
            }
            else
                return FALSE;
        }

        case wxDF_BITMAP:
        case wxDF_DIB:
        {
            wxBitmapDataObject* bitmapDataObject = (wxBitmapDataObject *)data;
            wxBitmap* bitmap = (wxBitmap *)wxGetClipboardData(data->GetFormat());
            if (bitmap)
            {
                bitmapDataObject->SetBitmap(* bitmap);
                delete bitmap;
                return TRUE;
            }
            else
                return FALSE;
        }
#if wxUSE_METAFILE
        case wxDF_METAFILE:
        {
            wxMetafileDataObject* metaFileDataObject = (wxMetafileDataObject *)data;
            wxMetafile* metaFile = (wxMetafile *)wxGetClipboardData(wxDF_METAFILE);
            if (metaFile)
            {
                metaFileDataObject->SetMetafile(*metaFile);
                delete metaFile;
                return TRUE;
            }
            else
                return FALSE;
        }
#endif
        default:
            {
                long len;
                void *buf = wxGetClipboardData(format, &len);
                if ( buf )
                {
                    // FIXME this is for testing only!!
                    ((wxPrivateDataObject *)data)->SetData(buf, len);
                    free(buf);

                    return TRUE;
                }
            }

            return FALSE;
    }
#else
*/
    return FALSE;
#endif
}

#else
    #error "Please turn wxUSE_CLIPBOARD on to compile this file."
#endif // wxUSE_CLIPBOARD

