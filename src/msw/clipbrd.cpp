/////////////////////////////////////////////////////////////////////////////
// Name:        clipbrd.cpp
// Purpose:     Clipboard functionality
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

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
    #include "wx/intl.h"
#endif

#if wxUSE_METAFILE
    #include "wx/metafile.h"
#endif

#include "wx/log.h"
#include "wx/clipbrd.h"

#include <string.h>
#include <windows.h>

#include "wx/msw/private.h"

#ifndef __WXMICROWIN__
#include "wx/msw/dib.h"
#endif

// wxDataObject is tied to OLE/drag and drop implementation, therefore so are
// the functions using wxDataObject in wxClipboard
//#define wxUSE_DATAOBJ wxUSE_DRAG_AND_DROP

#if wxUSE_DATAOBJ
    #include "wx/dataobj.h"
#endif

#if wxUSE_OLE
    // use OLE clipboard
    #define wxUSE_OLE_CLIPBOARD 1
#else // !wxUSE_DATAOBJ
    // use Win clipboard API
    #define wxUSE_OLE_CLIPBOARD 0
#endif

#if wxUSE_OLE_CLIPBOARD
    #include <ole2.h>
#endif // wxUSE_OLE_CLIPBOARD

#ifdef __WIN16__
    #define memcpy hmemcpy
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
}

bool wxCloseClipboard()
{
    wxCHECK_MSG( gs_wxClipboardIsOpen, FALSE, wxT("clipboard is not opened") );

    gs_wxClipboardIsOpen = FALSE;

    if ( ::CloseClipboard() == 0 )
    {
        wxLogSysError(_("Failed to close the clipboard."));

        return FALSE;
    }

    return TRUE;
}

bool wxEmptyClipboard()
{
    if ( ::EmptyClipboard() == 0 )
    {
        wxLogSysError(_("Failed to empty the clipboard."));

        return FALSE;
    }

    return TRUE;
}

bool wxIsClipboardOpened()
{
  return gs_wxClipboardIsOpen;
}

bool wxIsClipboardFormatAvailable(wxDataFormat dataFormat)
{
    if ( ::IsClipboardFormatAvailable(dataFormat) )
    {
        // ok from the first try
        return TRUE;
    }

    // for several standard formats, we can convert from some other ones too
    switch ( dataFormat.GetFormatId() )
    {
        // for bitmaps, DIBs will also do
        case CF_BITMAP:
            return ::IsClipboardFormatAvailable(CF_DIB) != 0;

#if wxUSE_ENH_METAFILE && !defined(__WIN16__)
        case CF_METAFILEPICT:
            return ::IsClipboardFormatAvailable(CF_ENHMETAFILE) != 0;
#endif // wxUSE_ENH_METAFILE

        default:
            return FALSE;
    }
}

#ifdef __DIGITALMARS__
extern "C" HGLOBAL wxDIB::ConvertFromBitmap(HBITMAP hbmp);
#endif


bool wxSetClipboardData(wxDataFormat dataFormat,
                        const void *data,
                        int width, int height)
{
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
                wxBitmap *bitmap = (wxBitmap *)data;

                HGLOBAL hDIB = wxDIB::ConvertFromBitmap(GetHbitmapOf(*bitmap));
                if ( hDIB )
                {
                    handle = ::SetClipboardData(CF_DIB, hDIB);
                }
                break;
            }

    // VZ: I'm told that this code works, but it doesn't seem to work for me
    //     and, anyhow, I'd be highly surprized if it did. So I leave it here
    //     but IMNSHO it is completely broken.
#if wxUSE_METAFILE && !defined(wxMETAFILE_IS_ENH)
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
#endif // wxUSE_METAFILE

#if wxUSE_ENH_METAFILE && !defined(__WIN16__)
        case wxDF_ENHMETAFILE:
            {
                wxEnhMetaFile *emf = (wxEnhMetaFile *)data;
                wxEnhMetaFile emfCopy = *emf;

                handle = SetClipboardData(CF_ENHMETAFILE,
                                          (void *)emfCopy.GetHENHMETAFILE());
            }
            break;
#endif // wxUSE_ENH_METAFILE

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
            // Only tested with non-Unicode, Visual C++ 6.0 so far
#if defined(__VISUALC__) && !defined(UNICODE)
        case wxDF_HTML:
            {
                char* html = (char *)data;
                
                // Create temporary buffer for HTML header...
                char *buf = new char [400 + strlen(html)];
                if(!buf) return FALSE;
                
                // Get clipboard id for HTML format...
                static int cfid = 0;
                if(!cfid) cfid = RegisterClipboardFormat(wxT("HTML Format"));
                
                // Create a template string for the HTML header...
                strcpy(buf,
                    "Version:0.9\r\n"
                    "StartHTML:00000000\r\n"
                    "EndHTML:00000000\r\n"
                    "StartFragment:00000000\r\n"
                    "EndFragment:00000000\r\n"
                    "<html><body>\r\n"
                    "<!--StartFragment -->\r\n");
                
                // Append the HTML...
                strcat(buf, html);
                strcat(buf, "\r\n");
                // Finish up the HTML format...
                strcat(buf,
                    "<!--EndFragment-->\r\n"
                    "</body>\r\n"
                    "</html>");
                
                // Now go back, calculate all the lengths, and write out the
                // necessary header information. Note, wsprintf() truncates the
                // string when you overwrite it so you follow up with code to replace
                // the 0 appended at the end with a '\r'...
                char *ptr = strstr(buf, "StartHTML");
                wsprintf(ptr+10, "%08u", strstr(buf, "<html>") - buf);
                *(ptr+10+8) = '\r';
                
                ptr = strstr(buf, "EndHTML");
                wsprintf(ptr+8, "%08u", strlen(buf));
                *(ptr+8+8) = '\r';
                
                ptr = strstr(buf, "StartFragment");
                wsprintf(ptr+14, "%08u", strstr(buf, "<!--StartFrag") - buf);
                *(ptr+14+8) = '\r';
                
                ptr = strstr(buf, "EndFragment");
                wsprintf(ptr+12, "%08u", strstr(buf, "<!--EndFrag") - buf);
                *(ptr+12+8) = '\r';
                
                // Now you have everything in place ready to put on the
                // clipboard.
                
                // Allocate global memory for transfer...
                HGLOBAL hText = GlobalAlloc(GMEM_MOVEABLE |GMEM_DDESHARE, strlen(buf)+4);
                
                // Put your string in the global memory...
                ptr = (char *)GlobalLock(hText);
                strcpy(ptr, buf);
                GlobalUnlock(hText);
                
                handle = ::SetClipboardData(cfid, hText);
                
                // Free memory...
                GlobalFree(hText);
                
                // Clean up...
                delete [] buf;
                break;
            }
#endif
    }

    if ( handle == 0 )
    {
        wxLogSysError(_("Failed to set clipboard data."));

        return FALSE;
    }

    return TRUE;
}

void *wxGetClipboardData(wxDataFormat dataFormat, long *len)
{
    void *retval = NULL;

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
#if WXWIN_COMPATIBILITY_2
                wxBM->SetOk(TRUE);
#endif // WXWIN_COMPATIBILITY_2
                retval = wxBM;
                break;
            }

        case wxDF_METAFILE:
        case CF_SYLK:
        case CF_DIF:
        case CF_TIFF:
        case CF_PALETTE:
        case wxDF_DIB:
            wxLogError(_("Unsupported clipboard format."));
            return NULL;

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
}

wxDataFormat wxEnumClipboardFormats(wxDataFormat dataFormat)
{
  return (wxDataFormat::NativeFormat)::EnumClipboardFormats(dataFormat);
}

int wxRegisterClipboardFormat(wxChar *formatName)
{
  return ::RegisterClipboardFormat(formatName);
}

bool wxGetClipboardFormatName(wxDataFormat dataFormat,
                              wxChar *formatName,
                              int maxCount)
{
  return ::GetClipboardFormatName((int)dataFormat, formatName, maxCount) > 0;
}

// ---------------------------------------------------------------------------
// wxClipboard
// ---------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxClipboard, wxObject)

wxClipboard::wxClipboard()
{
    m_clearOnExit = FALSE;
    m_isOpened = FALSE;
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
#if wxUSE_OLE_CLIPBOARD
    if ( FAILED(OleSetClipboard(NULL)) )
    {
        wxLogLastError(wxT("OleSetClipboard(NULL)"));
    }
#endif
}

bool wxClipboard::Flush()
{
#if wxUSE_OLE_CLIPBOARD
    if ( FAILED(OleFlushClipboard()) )
    {
        wxLogLastError(wxT("OleFlushClipboard"));

        return FALSE;
    }
    else
    {
        m_clearOnExit = FALSE;

        return TRUE;
    }
#else // !wxUSE_OLE_CLIPBOARD
    return FALSE;
#endif // wxUSE_OLE_CLIPBOARD/!wxUSE_OLE_CLIPBOARD
}

bool wxClipboard::Open()
{
    // OLE opens clipboard for us
    m_isOpened = TRUE;
#if wxUSE_OLE_CLIPBOARD
    return TRUE;
#else
    return wxOpenClipboard();
#endif
}

bool wxClipboard::IsOpened() const
{
#if wxUSE_OLE_CLIPBOARD
    return m_isOpened;
#else
    return wxIsClipboardOpened();
#endif
}

bool wxClipboard::SetData( wxDataObject *data )
{
#if !wxUSE_OLE_CLIPBOARD
    (void)wxEmptyClipboard();
#endif // wxUSE_OLE_CLIPBOARD

    if ( data )
        return AddData(data);
    else
        return TRUE;
}

bool wxClipboard::AddData( wxDataObject *data )
{
    wxCHECK_MSG( data, FALSE, wxT("data is invalid") );

#if wxUSE_OLE_CLIPBOARD
    HRESULT hr = OleSetClipboard(data->GetInterface());
    if ( FAILED(hr) )
    {
        wxLogSysError(hr, _("Failed to put data on the clipboard"));

        // don't free anything in this case

        return FALSE;
    }

    // we have a problem here because we should delete wxDataObject, but we
    // can't do it because IDataObject which we just gave to the clipboard
    // would try to use it when it will need the data. IDataObject is ref
    // counted and so doesn't suffer from such problem, so we release it now
    // and tell it to delete wxDataObject when it is deleted itself.
    data->SetAutoDelete();

    // we have to call either OleSetClipboard(NULL) or OleFlushClipboard() when
    // using OLE clipboard when the app terminates - by default, we call
    // OleSetClipboard(NULL) which won't waste RAM, but the app can call
    // wxClipboard::Flush() to chaneg this
    m_clearOnExit = TRUE;

    return TRUE;
#elif wxUSE_DATAOBJ
    wxCHECK_MSG( wxIsClipboardOpened(), FALSE, wxT("clipboard not open") );

    wxDataFormat format = data->GetPreferredFormat();

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
            return wxSetClipboardData(data->GetPreferredFormat(), &bitmap);
        }

#if wxUSE_METAFILE
        case wxDF_METAFILE:
        {
#if 1
            // TODO
            wxLogError("Not implemented because wxMetafileDataObject does not contain width and height values.");
            return FALSE;
#else
            wxMetafileDataObject* metaFileDataObject =
                (wxMetafileDataObject*) data;
            wxMetafile metaFile = metaFileDataObject->GetMetafile();
            return wxSetClipboardData(wxDF_METAFILE, &metaFile,
                                      metaFileDataObject->GetWidth(),
                                      metaFileDataObject->GetHeight());
#endif
        }
#endif // wxUSE_METAFILE

        default:
        {
// This didn't compile, of course
//            return wxSetClipboardData(data);
            // TODO
            wxLogError("Not implemented.");
            return FALSE;
        }
    }
#else // !wxUSE_DATAOBJ
    return FALSE;
#endif // wxUSE_DATAOBJ/!wxUSE_DATAOBJ
}

void wxClipboard::Close()
{
    m_isOpened = FALSE;
    // OLE closes clipboard for us
#if !wxUSE_OLE_CLIPBOARD
    wxCloseClipboard();
#endif
}

bool wxClipboard::IsSupported( wxDataFormat format )
{
    return wxIsClipboardFormatAvailable(format);
}

bool wxClipboard::GetData( wxDataObject& data )
{
#if wxUSE_OLE_CLIPBOARD
    IDataObject *pDataObject = NULL;
    HRESULT hr = OleGetClipboard(&pDataObject);
    if ( FAILED(hr) || !pDataObject )
    {
        wxLogSysError(hr, _("Failed to get data from the clipboard"));

        return FALSE;
    }

    // build the list of supported formats
    size_t nFormats = data.GetFormatCount(wxDataObject::Set);
    wxDataFormat format;
    wxDataFormat *formats;
    if ( nFormats == 1 )
    {
        // the most common case
        formats = &format;
    }
    else
    {
        // bad luck, need to alloc mem
        formats = new wxDataFormat[nFormats];
    }

    data.GetAllFormats(formats, wxDataObject::Set);

    // get the format enumerator
    bool result = FALSE;
    wxArrayInt supportedFormats;
    IEnumFORMATETC *pEnumFormatEtc = NULL;
    hr = pDataObject->EnumFormatEtc(DATADIR_GET, &pEnumFormatEtc);
    if ( FAILED(hr) || !pEnumFormatEtc )
    {
        wxLogSysError(hr,
                      _("Failed to retrieve the supported clipboard formats"));
    }
    else
    {
        // ask for the supported formats and see if there are any we support
        FORMATETC formatEtc;
        for ( ;; )
        {
            ULONG nCount;
            hr = pEnumFormatEtc->Next(1, &formatEtc, &nCount);

            // don't use FAILED() because S_FALSE would pass it
            if ( hr != S_OK )
            {
                // no more formats
                break;
            }

            CLIPFORMAT cf = formatEtc.cfFormat;

#ifdef __WXDEBUG__
            wxLogTrace(wxTRACE_OleCalls,
                       wxT("Object on the clipboard supports format %s."),
                       wxDataObject::GetFormatName(cf));
#endif // Debug

            // is supported?
            for ( size_t n = 0; n < nFormats; n++ )
            {
                if ( formats[n].GetFormatId() == cf )
                {
                    if ( supportedFormats.Index(cf) == wxNOT_FOUND )
                    {
                        supportedFormats.Add(cf);
                    }
                }
            }
        }

        pEnumFormatEtc->Release();
    }

    if ( formats != &format )
    {
        delete [] formats;
    }
    //else: we didn't allocate any memory

    if ( !supportedFormats.IsEmpty() )
    {
        FORMATETC formatEtc;
        formatEtc.ptd      = NULL;
        formatEtc.dwAspect = DVASPECT_CONTENT;
        formatEtc.lindex   = -1;

        size_t nSupportedFormats = supportedFormats.GetCount();
        for ( size_t n = 0; !result && (n < nSupportedFormats); n++ )
        {
            STGMEDIUM medium;
            formatEtc.cfFormat = supportedFormats[n];

            // use the appropriate tymed
            switch ( formatEtc.cfFormat )
            {
                case CF_BITMAP:
                    formatEtc.tymed = TYMED_GDI;
                    break;

                case CF_METAFILEPICT:
                    formatEtc.tymed = TYMED_MFPICT;
                    break;

                case CF_ENHMETAFILE:
                    formatEtc.tymed = TYMED_ENHMF;
                    break;

                default:
                    formatEtc.tymed = TYMED_HGLOBAL;
            }

            // try to get data
            hr = pDataObject->GetData(&formatEtc, &medium);
            if ( FAILED(hr) )
            {
                // try other tymed for GDI objects
                if ( formatEtc.cfFormat == CF_BITMAP )
                {
                    formatEtc.tymed = TYMED_HGLOBAL;
                    hr = pDataObject->GetData(&formatEtc, &medium);
                }
            }

            if ( SUCCEEDED(hr) )
            {
                // pass the data to the data object
                hr = data.GetInterface()->SetData(&formatEtc, &medium, TRUE);
                if ( FAILED(hr) )
                {
                    wxLogDebug(wxT("Failed to set data in wxIDataObject"));

                    // IDataObject only takes the ownership of data if it
                    // successfully got it - which is not the case here
                    ReleaseStgMedium(&medium);
                }
                else
                {
                    result = TRUE;
                }
            }
            //else: unsupported tymed?
        }
    }
    //else: unsupported format

    // clean up and return
    pDataObject->Release();

    return result;
#elif wxUSE_DATAOBJ
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
            wxBitmap* bitmap = (wxBitmap *)wxGetClipboardData(data.GetPreferredFormat());
            if ( !bitmap )
                return FALSE;

            bitmapDataObject.SetBitmap(*bitmap);
            delete bitmap;

            return TRUE;
        }
#if wxUSE_METAFILE
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
    return FALSE;
#else // !wxUSE_DATAOBJ
    wxFAIL_MSG( wxT("no clipboard implementation") );
    return FALSE;
#endif // wxUSE_OLE_CLIPBOARD/wxUSE_DATAOBJ
}

#endif // wxUSE_CLIPBOARD

