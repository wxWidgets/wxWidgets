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

#include <ole2.h>

// ===========================================================================
// implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// old-style clipboard functions using Windows API
// ---------------------------------------------------------------------------

static int gs_htmlcfid = 0;
static int gs_pngcfid = 0;

#ifdef WXWIN_COMPATIBILITY_3_2

static bool gs_wxClipboardIsOpen = false;

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

bool wxClipboardOpen()
{
  return gs_wxClipboardIsOpen;
}

#endif // WXWIN_COMPATIBILITY_3_2

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
}

bool wxClipboard::Flush()
{
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

    return true;
}

bool wxClipboard::IsOpened() const
{
    return m_isOpened;
}

bool wxClipboard::SetData( wxDataObject *data )
{
    if ( IsUsingPrimarySelection() )
        return false;

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
}

void wxClipboard::Close()
{
    m_isOpened = false;
}

bool wxClipboard::IsSupported( const wxDataFormat& format )
{
    return !IsUsingPrimarySelection() && wxIsClipboardFormatAvailable(format);
}

bool wxClipboard::GetData( wxDataObject& data )
{
    if ( IsUsingPrimarySelection() )
        return false;

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
}

#endif // wxUSE_CLIPBOARD
