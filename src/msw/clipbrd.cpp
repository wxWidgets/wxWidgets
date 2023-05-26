/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/clipbrd.cpp
// Purpose:     Clipboard functionality
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_CLIPBOARD

#include "wx/clipbrd.h"

#ifndef WX_PRECOMP
    #include "wx/object.h"
    #include "wx/event.h"
    #include "wx/app.h"
    #include "wx/frame.h"
    #include "wx/bitmap.h"
    #include "wx/utils.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/dataobj.h"
    #include "wx/dcmemory.h"
#endif

#if wxUSE_METAFILE
    #include "wx/metafile.h"
#endif


#include <string.h>

#include "wx/msw/private.h"
#include "wx/msw/ole/oleutils.h"

#if wxUSE_WXDIB
    #include "wx/msw/dib.h"
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

// ===========================================================================
// implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// old-style clipboard functions using Windows API
// ---------------------------------------------------------------------------

static bool gs_wxClipboardIsOpen = false;
static int gs_htmlcfid = 0;
static int gs_pngcfid = 0;

bool wxOpenClipboard()
{
    wxCHECK_MSG( !gs_wxClipboardIsOpen, true, wxT("clipboard already opened.") );

    wxWindow *win = wxTheApp->GetTopWindow();
    if ( win )
    {
        gs_wxClipboardIsOpen = ::OpenClipboard((HWND)win->GetHWND()) != 0;

        if ( !gs_wxClipboardIsOpen )
        {
            wxLogSysError(_("Failed to open the clipboard."));
        }

        return gs_wxClipboardIsOpen;
    }
    else
    {
        wxLogDebug(wxT("Cannot open clipboard without a main window."));

        return false;
    }
}

bool wxCloseClipboard()
{
    wxCHECK_MSG( gs_wxClipboardIsOpen, false, wxT("clipboard is not opened") );

    gs_wxClipboardIsOpen = false;

    if ( ::CloseClipboard() == 0 )
    {
        wxLogSysError(_("Failed to close the clipboard."));

        return false;
    }

    return true;
}

bool wxEmptyClipboard()
{
    if ( ::EmptyClipboard() == 0 )
    {
        wxLogSysError(_("Failed to empty the clipboard."));

        return false;
    }

    return true;
}

bool wxIsClipboardOpened()
{
  return gs_wxClipboardIsOpen;
}

bool wxIsClipboardFormatAvailable(wxDataFormat dataFormat)
{
    wxDataFormat::NativeFormat cf = dataFormat.GetFormatId();
    if (cf == wxDF_HTML)
        cf = gs_htmlcfid;
    else if ( cf == wxDF_PNG )
        cf = gs_pngcfid;

    if ( ::IsClipboardFormatAvailable(cf) )
    {
        // ok from the first try
        return true;
    }

    // for several standard formats, we can convert from some other ones too
    switch ( cf )
    {
        // for bitmaps, DIBs will also do
        case CF_BITMAP:
            return ::IsClipboardFormatAvailable(CF_DIB) != 0;

#if wxUSE_ENH_METAFILE
        case CF_METAFILEPICT:
            return ::IsClipboardFormatAvailable(CF_ENHMETAFILE) != 0;
#endif // wxUSE_ENH_METAFILE

        default:
            return false;
    }
}


#if !wxUSE_OLE_CLIPBOARD
namespace
{
struct wxRawImageData
{
    size_t m_size;
    void* m_data;
};
}

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

                HDC hdcMem = CreateCompatibleDC(nullptr);
                HDC hdcSrc = CreateCompatibleDC(nullptr);
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
                    return false;
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

#if wxUSE_WXDIB
        case wxDF_DIB:
            {
                wxBitmap *bitmap = (wxBitmap *)data;

                if ( bitmap && bitmap->IsOk() )
                {
                    wxDIB dib(*bitmap);

                    if ( dib.IsOk() )
                    {
                        DIBSECTION ds;
                        int n = ::GetObject(dib.GetHandle(), sizeof(DIBSECTION), &ds);
                        wxASSERT( n == sizeof(DIBSECTION) && ds.dsBm.bmBits );
                        // Number of colours in the palette.
                        int numColors;
                        switch ( ds.dsBmih.biCompression )
                        {
                        case BI_BITFIELDS:
                            numColors = 3;
                            break;
                        case BI_RGB:
                        case BI_RLE8:
                        case BI_RLE4:
                            numColors = ds.dsBmih.biClrUsed;
                            if ( !numColors )
                            {
                                numColors = ds.dsBmih.biBitCount <= 8 ? 1 << ds.dsBmih.biBitCount : 0;
                            }
                            break;
                        default:
                            numColors = 0;
                        }

                        unsigned long bmpSize = wxDIB::GetLineSize(ds.dsBmih.biWidth, ds.dsBmih.biBitCount) *
                                                                   abs(ds.dsBmih.biHeight);
                        HANDLE hMem;
                        hMem = ::GlobalAlloc(GHND, ds.dsBmih.biSize + numColors*sizeof(RGBQUAD) + bmpSize);
                        if ( !hMem )
                            break;

                        {
                            GlobalPtrLock ptr(hMem);
                            char* pDst = (char*)ptr.Get();
                            memcpy(pDst, &ds.dsBmih, ds.dsBmih.biSize);
                            pDst += ds.dsBmih.biSize;
                            if ( numColors > 0 )
                            {
                                // Get colour table.
                                MemoryHDC hDC;
                                SelectInHDC sDC(hDC, dib.GetHandle());
                                ::GetDIBColorTable(hDC, 0, numColors, (RGBQUAD*)pDst);
                                pDst += numColors*sizeof(RGBQUAD);
                            }
                            memcpy(pDst, dib.GetData(), bmpSize);
                        } // unlock hMem

                        handle = ::SetClipboardData(CF_DIB, hMem);
                    }
                }
                break;
            }
#endif

    // VZ: I'm told that this code works, but it doesn't seem to work for me
    //     and, anyhow, I'd be highly surprised if it did. So I leave it here
    //     but IMNSHO it is completely broken.
#if wxUSE_METAFILE && !defined(wxMETAFILE_IS_ENH)
        case wxDF_METAFILE:
            {
                wxMetafile *wxMF = (wxMetafile *)data;
                HANDLE data = GlobalAlloc(GHND, sizeof(METAFILEPICT) + 1);
                {
                    GlobalPtrLock ptr(data);
                    METAFILEPICT *mf = (METAFILEPICT *)data.Get();

                    mf->mm = wxMF->GetWindowsMappingMode();
                    mf->xExt = width;
                    mf->yExt = height;
                    mf->hMF = (HMETAFILE) wxMF->GetHMETAFILE();
                    wxMF->SetHMETAFILE(nullptr);
                } // unlock data

                handle = SetClipboardData(CF_METAFILEPICT, data);
                break;
            }
#endif // wxUSE_METAFILE

#if wxUSE_ENH_METAFILE
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
                return false;
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
                    memcpy(GlobalPtrLock(hGlobalMemory), s, l);
                }

                handle = SetClipboardData(dataFormat, hGlobalMemory);
                break;
            }

        case wxDF_UNICODETEXT:
            {
                LPWSTR s = (LPWSTR)data;
                DWORD size = sizeof(WCHAR) * (lstrlenW(s) + 1);
                HANDLE hGlobalMemory = ::GlobalAlloc(GHND, size);
                if ( hGlobalMemory )
                {
                    memcpy(GlobalPtrLock(hGlobalMemory), s, size);
                }

                handle = ::SetClipboardData(CF_UNICODETEXT, hGlobalMemory);
            }
            break;

        case wxDF_HTML:
            {
                char* html = (char *)data;

                // Create temporary buffer for HTML header...
                char *buf = new char [400 + strlen(html)];
                if(!buf) return false;

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
                sprintf(ptr+10, "%08u", (unsigned)(strstr(buf, "<html>") - buf));
                *(ptr+10+8) = '\r';

                ptr = strstr(buf, "EndHTML");
                sprintf(ptr+8, "%08u", (unsigned)strlen(buf));
                *(ptr+8+8) = '\r';

                ptr = strstr(buf, "StartFragment");
                sprintf(ptr+14, "%08u", (unsigned)(strstr(buf, "<!--StartFrag") - buf));
                *(ptr+14+8) = '\r';

                ptr = strstr(buf, "EndFragment");
                sprintf(ptr+12, "%08u", (unsigned)(strstr(buf, "<!--EndFrag") - buf));
                *(ptr+12+8) = '\r';

                // Now you have everything in place ready to put on the
                // clipboard.

                // Allocate global memory for transfer...
                HGLOBAL hText = GlobalAlloc(GMEM_MOVEABLE |GMEM_DDESHARE, strlen(buf)+4);

                // Put your string in the global memory...
                strcpy((char*)GlobalPtrLock(hText).Get(), buf);

                handle = ::SetClipboardData(gs_htmlcfid, hText);

                // Free memory...
                GlobalFree(hText);

                // Clean up...
                delete [] buf;
                break;
            }

            case wxDF_PNG:
            {
                const wxRawImageData* imgData = reinterpret_cast<const wxRawImageData*>(data);

                GlobalPtr hImage(imgData->m_size, GMEM_MOVEABLE | GMEM_DDESHARE);
                memcpy(GlobalPtrLock(hImage).Get(), imgData->m_data, imgData->m_size);
                handle = ::SetClipboardData(gs_pngcfid, hImage);
                break;
            }
    }

    if ( handle == 0 )
    {
        wxLogSysError(_("Failed to set clipboard data."));

        return false;
    }

    return true;
}
#endif // !wxUSE_OLE_CLIPBOARD

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

wxIMPLEMENT_DYNAMIC_CLASS(wxClipboard, wxObject);

wxClipboard::wxClipboard()
{
    m_lastDataObject = nullptr;
    m_isOpened = false;
}

wxClipboard::~wxClipboard()
{
    if ( m_lastDataObject )
    {
        Clear();
    }
}

void wxClipboard::Clear()
{
    if ( IsUsingPrimarySelection() )
        return;

#if wxUSE_OLE_CLIPBOARD
    if (m_lastDataObject)
    {
        // don't touch data set by other applications
        HRESULT hr = OleIsCurrentClipboard(m_lastDataObject);
        if (S_OK == hr)
        {
            hr = OleSetClipboard(nullptr);
            if ( FAILED(hr) )
            {
                wxLogApiError(wxT("OleSetClipboard(nullptr)"), hr);
            }
        }
        m_lastDataObject = nullptr;
    }
#endif // wxUSE_OLE_CLIPBOARD
}

bool wxClipboard::Flush()
{
#if wxUSE_OLE_CLIPBOARD
    if (m_lastDataObject)
    {
        // don't touch data set by other applications
        HRESULT hr = OleIsCurrentClipboard(m_lastDataObject);
        m_lastDataObject = nullptr;
        if (S_OK == hr)
        {
            hr = OleFlushClipboard();
            if ( FAILED(hr) )
            {
                wxLogApiError(wxT("OleFlushClipboard"), hr);

                return false;
            }
            return true;
        }
    }
    return false;
#else // !wxUSE_OLE_CLIPBOARD
    return false;
#endif // wxUSE_OLE_CLIPBOARD/!wxUSE_OLE_CLIPBOARD
}

bool wxClipboard::Open()
{
    // Get clipboard id for HTML and PNG formats...
    if(!gs_htmlcfid)
        gs_htmlcfid = RegisterClipboardFormat(wxT("HTML Format"));
    if ( !gs_pngcfid )
        gs_pngcfid = ::RegisterClipboardFormat(wxT("PNG"));

    // OLE opens clipboard for us
    m_isOpened = true;
#if wxUSE_OLE_CLIPBOARD
    return true;
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
    if ( IsUsingPrimarySelection() )
        return false;

#if !wxUSE_OLE_CLIPBOARD
    (void)wxEmptyClipboard();
#endif // wxUSE_OLE_CLIPBOARD

    if ( data )
        return AddData(data);
    else
        return true;
}

bool wxClipboard::AddData( wxDataObject *data )
{
    if ( IsUsingPrimarySelection() )
        return false;

    wxCHECK_MSG( data, false, wxT("data is invalid") );

    const wxDataFormat format = data->GetPreferredFormat();
    if ( format == wxDF_BITMAP || format == wxDF_DIB )
    {
        wxBitmapDataObject* bmpData = (wxBitmapDataObject*)data;
        wxBitmap bmp = bmpData->GetBitmap();
        wxASSERT_MSG( bmp.IsOk(), wxS("Invalid bitmap") );
        // Replace 0RGB bitmap with its RGB copy
        // to ensure compatibility with applications
        // not recognizing bitmaps in 0RGB format.
        if ( bmp.GetDepth() == 32 && !bmp.HasAlpha() )
        {
            wxBitmap bmpRGB(bmp.GetSize(), 24);
            wxMemoryDC dc(bmpRGB);
            dc.DrawBitmap(bmp, 0, 0);
            dc.SelectObject(wxNullBitmap);

            bmpData->SetBitmap(bmpRGB);
        }
    }

#if wxUSE_OLE_CLIPBOARD
    HRESULT hr = OleSetClipboard(data->GetInterface());
    if ( FAILED(hr) )
    {
        wxLogSysError(hr, _("Failed to put data on the clipboard"));

        // don't free anything in this case

        return false;
    }

    // we have to call either OleSetClipboard(nullptr) or OleFlushClipboard() when
    // using OLE clipboard when the app terminates - by default, we call
    // OleSetClipboard(nullptr) which won't waste RAM, but the app can call
    // wxClipboard::Flush() to change this
    m_lastDataObject = data->GetInterface();

    // we have a problem here because we should delete wxDataObject, but we
    // can't do it because IDataObject which we just gave to the clipboard
    // would try to use it when it will need the data. IDataObject is ref
    // counted and so doesn't suffer from such problem, so we release it now
    // and tell it to delete wxDataObject when it is deleted itself.
    data->SetAutoDelete();

    return true;
#elif wxUSE_DATAOBJ
    wxCHECK_MSG( wxIsClipboardOpened(), false, wxT("clipboard not open") );

    bool bRet = false;
    switch ( format )
    {
        case wxDF_TEXT:
        case wxDF_OEMTEXT:
        {
            wxTextDataObject* textDataObject = (wxTextDataObject*) data;
            wxString str(textDataObject->GetText());
            bRet = wxSetClipboardData(format, str.c_str());
        }
        break;

        case wxDF_UNICODETEXT:
        {
            wxTextDataObject* textDataObject = (wxTextDataObject*)data;
            wxString str(textDataObject->GetText());
            bRet = wxSetClipboardData(format, str.wc_str());
        }
        break;

        case wxDF_BITMAP:
        case wxDF_DIB:
        {
            wxBitmapDataObject* bitmapDataObject = (wxBitmapDataObject*) data;
            wxBitmap bitmap(bitmapDataObject->GetBitmap());
            bRet = wxSetClipboardData(format, &bitmap);
        }
        break;

        case wxDF_PNG:
        {
            wxCustomDataObject* imageDataObject = reinterpret_cast<wxCustomDataObject*>(data);
            wxRawImageData imgData;
            imgData.m_size = imageDataObject->GetDataSize();
            imgData.m_data = imageDataObject->GetData();
            bRet = wxSetClipboardData(format, &imgData);
        }
        break;

#if wxUSE_METAFILE
        case wxDF_METAFILE:
        {
#if 1
            // TODO
            wxLogError(wxT("Not implemented because wxMetafileDataObject does not contain width and height values."));
#else
            wxMetafileDataObject* metaFileDataObject =
                (wxMetafileDataObject*) data;
            wxMetafile metaFile = metaFileDataObject->GetMetafile();
            bRet = wxSetClipboardData(wxDF_METAFILE, &metaFile,
                                      metaFileDataObject->GetWidth(),
                                      metaFileDataObject->GetHeight());
#endif
        }
        break;
#endif // wxUSE_METAFILE

        default:
        {
// This didn't compile, of course
//            return wxSetClipboardData(data);
            // TODO
            wxLogError(wxT("Not implemented."));
        }
    }
    // Delete owned, no longer necessary data.
    delete data;
    return bRet;
#else // !wxUSE_DATAOBJ
    return false;
#endif // wxUSE_DATAOBJ/!wxUSE_DATAOBJ
}

void wxClipboard::Close()
{
    m_isOpened = false;
    // OLE closes clipboard for us
#if !wxUSE_OLE_CLIPBOARD
    wxCloseClipboard();
#endif
}

bool wxClipboard::IsSupported( const wxDataFormat& format )
{
    return !IsUsingPrimarySelection() && wxIsClipboardFormatAvailable(format);
}

bool wxClipboard::GetData( wxDataObject& data )
{
    if ( IsUsingPrimarySelection() )
        return false;

#if wxUSE_OLE_CLIPBOARD
    IDataObject *pDataObject = nullptr;
    HRESULT hr = OleGetClipboard(&pDataObject);
    if ( FAILED(hr) || !pDataObject )
    {
        wxLogSysError(hr, _("Failed to get data from the clipboard"));

        return false;
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

    // get the data for the given formats
    FORMATETC formatEtc;
    CLIPFORMAT cf;
    bool result = false;

    // enumerate all explicit formats on the clipboard.
    // note that this does not include implicit / synthetic (automatically
    // converted) formats.
#if wxDEBUG_LEVEL >= 2
    // get the format enumerator
    IEnumFORMATETC *pEnumFormatEtc = nullptr;
    hr = pDataObject->EnumFormatEtc(DATADIR_GET, &pEnumFormatEtc);
    if ( FAILED(hr) || !pEnumFormatEtc )
    {
        wxLogSysError(hr,
                      _("Failed to retrieve the supported clipboard formats"));
    }
    else
    {
        // ask for the supported formats and see if there are any we support
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

            cf = formatEtc.cfFormat;

            wxLogTrace(wxTRACE_OleCalls,
                       wxT("Object on the clipboard supports format %s."),
                       wxDataObject::GetFormatName(cf));
        }

        pEnumFormatEtc->Release();
    }
#endif // wxDEBUG_LEVEL >= 2

    STGMEDIUM medium;
    // stop at the first valid format found on the clipboard
    for ( size_t n = 0; !result && (n < nFormats); n++ )
    {
        // convert to NativeFormat Id
        cf = formats[n].GetFormatId();

        if (cf == wxDF_HTML)
            cf = gs_htmlcfid;
        else if ( cf == wxDF_PNG )
            cf = gs_pngcfid;
        // if the format is not available, try the next one
        // this test includes implicit / sythetic formats
        if ( !::IsClipboardFormatAvailable(cf) )
            continue;

        formatEtc.cfFormat = cf;
        formatEtc.ptd      = nullptr;
        formatEtc.dwAspect = DVASPECT_CONTENT;
        formatEtc.lindex   = -1;

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
            hr = data.GetInterface()->SetData(&formatEtc, &medium, true);
            if ( FAILED(hr) )
            {
                wxLogDebug(wxT("Failed to set data in wxIDataObject"));

                // IDataObject only takes the ownership of data if it
                // successfully got it - which is not the case here
                ReleaseStgMedium(&medium);
            }
            else
            {
                result = true;
            }
        }
        //else: unsupported tymed?
    }

    if ( formats != &format )
    {
        delete [] formats;
    }
    //else: we didn't allocate any memory

    // clean up and return
    pDataObject->Release();

    return result;
#elif wxUSE_DATAOBJ
    wxCHECK_MSG( wxIsClipboardOpened(), false, wxT("clipboard not open") );

    wxDataFormat format = data.GetPreferredFormat();
    switch ( format )
    {
        case wxDF_TEXT:
        case wxDF_OEMTEXT:
        case wxDF_UNICODETEXT:
            {
                // System provides an automatic type conversion
                // from CF_TEXT and CF_OEMTEXT to CF_UNICODETEXT
                HANDLE hMem = ::GetClipboardData(CF_UNICODETEXT);
                if ( hMem )
                {
                    wxTextDataObject& textDataObject = (wxTextDataObject &)data;

                    GlobalPtrLock ptr(hMem);
                    return textDataObject.SetData(ptr.GetSize(), ptr);
                }
            }
            break;
        case wxDF_BITMAP:
            {
                HANDLE hBmp = ::GetClipboardData(CF_BITMAP);
                if ( hBmp )
                {
                    wxBitmapDataObject2& bitmapDataObject = (wxBitmapDataObject2 &)data;
                    return bitmapDataObject.SetData(0, &hBmp);
                }
            }
            break;
        case wxDF_DIB:
            {
                HANDLE hMem = ::GetClipboardData(CF_DIB);
                if ( hMem )
                {
                    wxBitmapDataObject& bitmapDataObject = (wxBitmapDataObject &)data;

                    GlobalPtrLock ptr(hMem);
                    return bitmapDataObject.SetData(ptr.GetSize(), ptr);
                }
            }
            break;

#if wxUSE_METAFILE && !defined(wxMETAFILE_IS_ENH)
        case wxDF_METAFILE:
            {
                HANDLE hMem = ::GetClipboardData(CF_METAFILEPICT);
                if ( hMem )
                {
                    wxMetafileDataObject& metaFileDataObject = (wxMetafileDataObject &)data;

                    GlobalPtrLock ptr(hMem);
                    return metaFileDataObject.SetData(wxDF_METAFILE, ptr.GetSize(), ptr);
                }
            }
            break;
#endif // wxUSE_METAFILE && !defined(wxMETAFILE_IS_ENH)

#if wxUSE_ENH_METAFILE
        case wxDF_ENHMETAFILE:
            {
                HANDLE hFile = ::GetClipboardData(CF_ENHMETAFILE);
                if ( hFile )
                {
                    wxMetafileDataObject& metaFileDataObject = (wxMetafileDataObject &)data;
                    return metaFileDataObject.SetData(wxDF_ENHMETAFILE, 0, &hFile);
                }
            }
            break;
#endif // wxUSE_ENH_METAFILE
    }
    return false;
#else // !wxUSE_DATAOBJ
    wxFAIL_MSG( wxT("no clipboard implementation") );
    return false;
#endif // wxUSE_OLE_CLIPBOARD/wxUSE_DATAOBJ
}

#endif // wxUSE_CLIPBOARD
