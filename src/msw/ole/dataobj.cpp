///////////////////////////////////////////////////////////////////////////////
// Name:        msw/ole/dataobj.cpp
// Purpose:     implementation of wx[I]DataObject class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     10.05.98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "dataobj.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if defined(__BORLANDC__)
    #pragma hdrstop
#endif

#if defined(__WIN32__) && !defined(__GNUWIN32_OLD__)

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
#endif

#include "wx/dataobj.h"

#include "wx/msw/private.h"         // includes <windows.h>

#ifdef wxUSE_NORLANDER_HEADERS
  #include <ole2.h>
#endif
#include <oleauto.h>

#ifndef __WIN32__
  #include <ole2.h>
  #include <olestd.h>
#endif

#include <shlobj.h>

#include "wx/msw/ole/oleutils.h"

#include "wx/msw/dib.h"

// ----------------------------------------------------------------------------
// functions
// ----------------------------------------------------------------------------

#ifdef __WXDEBUG__
    static const wxChar *GetTymedName(DWORD tymed);
#else // !Debug
    #define GetTymedName(tymed) ""
#endif // Debug/!Debug

// ----------------------------------------------------------------------------
// wxIEnumFORMATETC interface implementation
// ----------------------------------------------------------------------------

class wxIEnumFORMATETC : public IEnumFORMATETC
{
public:
    wxIEnumFORMATETC(const wxDataFormat* formats, ULONG nCount);

    // to suppress the gcc warning about "class has virtual functions but non
    // virtual dtor"
#ifdef __GNUG__
    virtual
#endif
    ~wxIEnumFORMATETC() { delete [] m_formats; }

    DECLARE_IUNKNOWN_METHODS;

    // IEnumFORMATETC
    STDMETHODIMP Next(ULONG celt, FORMATETC *rgelt, ULONG *pceltFetched);
    STDMETHODIMP Skip(ULONG celt);
    STDMETHODIMP Reset();
    STDMETHODIMP Clone(IEnumFORMATETC **ppenum);

private:
    CLIPFORMAT *m_formats;  // formats we can provide data in
    ULONG       m_nCount,   // number of formats we support
                m_nCurrent; // current enum position
};

// ----------------------------------------------------------------------------
// wxIDataObject implementation of IDataObject interface
// ----------------------------------------------------------------------------

class wxIDataObject : public IDataObject
{
public:
    wxIDataObject(wxDataObject *pDataObject);

    // to suppress the gcc warning about "class has virtual functions but non
    // virtual dtor"
#ifdef __GNUG__
    virtual
#endif
    ~wxIDataObject();

    // normally, wxDataObject controls our lifetime (i.e. we're deleted when it
    // is), but in some cases, the situation is inversed, that is we delete it
    // when this object is deleted - setting this flag enables such logic
    void SetDeleteFlag() { m_mustDelete = TRUE; }

    DECLARE_IUNKNOWN_METHODS;

    // IDataObject
    STDMETHODIMP GetData(FORMATETC *pformatetcIn, STGMEDIUM *pmedium);
    STDMETHODIMP GetDataHere(FORMATETC *pformatetc, STGMEDIUM *pmedium);
    STDMETHODIMP QueryGetData(FORMATETC *pformatetc);
    STDMETHODIMP GetCanonicalFormatEtc(FORMATETC *In, FORMATETC *pOut);
    STDMETHODIMP SetData(FORMATETC *pfetc, STGMEDIUM *pmedium, BOOL fRelease);
    STDMETHODIMP EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC **ppenumFEtc);
    STDMETHODIMP DAdvise(FORMATETC *pfetc, DWORD ad, IAdviseSink *p, DWORD *pdw);
    STDMETHODIMP DUnadvise(DWORD dwConnection);
    STDMETHODIMP EnumDAdvise(IEnumSTATDATA **ppenumAdvise);

private:
    wxDataObject *m_pDataObject;      // pointer to C++ class we belong to

    bool m_mustDelete;
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxDataFormat
// ----------------------------------------------------------------------------

void wxDataFormat::SetId(const wxChar *format)
{
    m_format = ::RegisterClipboardFormat(format);
    if ( !m_format )
    {
        wxLogError(_("Couldn't register clipboard format '%s'."), format);
    }
}

wxString wxDataFormat::GetId() const
{
    static const int max = 256;

    wxString s;

    wxCHECK_MSG( !IsStandard(), s,
                 wxT("name of predefined format cannot be retrieved") );

    int len = ::GetClipboardFormatName(m_format, s.GetWriteBuf(max), max);
    s.UngetWriteBuf();

    if ( !len )
    {
        wxLogError(_("The clipboard format '%d' doesn't exist."), m_format);
    }

    return s;
}

// ----------------------------------------------------------------------------
// wxIEnumFORMATETC
// ----------------------------------------------------------------------------

BEGIN_IID_TABLE(wxIEnumFORMATETC)
    ADD_IID(Unknown)
    ADD_IID(EnumFORMATETC)
END_IID_TABLE;

IMPLEMENT_IUNKNOWN_METHODS(wxIEnumFORMATETC)

wxIEnumFORMATETC::wxIEnumFORMATETC(const wxDataFormat *formats, ULONG nCount)
{
    m_cRef = 0;
    m_nCurrent = 0;
    m_nCount = nCount;
    m_formats = new CLIPFORMAT[nCount];
    for ( ULONG n = 0; n < nCount; n++ ) {
        m_formats[n] = formats[n].GetFormatId();
    }
}

STDMETHODIMP wxIEnumFORMATETC::Next(ULONG      celt,
                                    FORMATETC *rgelt,
                                    ULONG     *pceltFetched)
{
    wxLogTrace(wxTRACE_OleCalls, wxT("wxIEnumFORMATETC::Next"));

    if ( celt > 1 ) {
        // we only return 1 element at a time - mainly because I'm too lazy to
        // implement something which you're never asked for anyhow
        return S_FALSE;
    }

    if ( m_nCurrent < m_nCount ) {
        FORMATETC format;
        format.cfFormat = m_formats[m_nCurrent++];
        format.ptd      = NULL;
        format.dwAspect = DVASPECT_CONTENT;
        format.lindex   = -1;
        format.tymed    = TYMED_HGLOBAL;
        *rgelt = format;

        return S_OK;
    }
    else {
        // bad index
        return S_FALSE;
    }
}

STDMETHODIMP wxIEnumFORMATETC::Skip(ULONG celt)
{
    wxLogTrace(wxTRACE_OleCalls, wxT("wxIEnumFORMATETC::Skip"));

    m_nCurrent += celt;
    if ( m_nCurrent < m_nCount )
        return S_OK;

    // no, can't skip this many elements
    m_nCurrent -= celt;

    return S_FALSE;
}

STDMETHODIMP wxIEnumFORMATETC::Reset()
{
    wxLogTrace(wxTRACE_OleCalls, wxT("wxIEnumFORMATETC::Reset"));

    m_nCurrent = 0;

    return S_OK;
}

STDMETHODIMP wxIEnumFORMATETC::Clone(IEnumFORMATETC **ppenum)
{
    wxLogTrace(wxTRACE_OleCalls, wxT("wxIEnumFORMATETC::Clone"));

    // unfortunately, we can't reuse the code in ctor - types are different
    wxIEnumFORMATETC *pNew = new wxIEnumFORMATETC(NULL, 0);
    pNew->m_nCount = m_nCount;
    pNew->m_formats = new CLIPFORMAT[m_nCount];
    for ( ULONG n = 0; n < m_nCount; n++ ) {
        pNew->m_formats[n] = m_formats[n];
    }
    pNew->AddRef();
    *ppenum = pNew;

    return S_OK;
}

// ----------------------------------------------------------------------------
// wxIDataObject
// ----------------------------------------------------------------------------

BEGIN_IID_TABLE(wxIDataObject)
    ADD_IID(Unknown)
    ADD_IID(DataObject)
END_IID_TABLE;

IMPLEMENT_IUNKNOWN_METHODS(wxIDataObject)

wxIDataObject::wxIDataObject(wxDataObject *pDataObject)
{
    m_cRef = 0;
    m_pDataObject = pDataObject;
    m_mustDelete = FALSE;
}

wxIDataObject::~wxIDataObject()
{
    if ( m_mustDelete )
    {
        delete m_pDataObject;
    }
}

// get data functions
STDMETHODIMP wxIDataObject::GetData(FORMATETC *pformatetcIn, STGMEDIUM *pmedium)
{
    wxLogTrace(wxTRACE_OleCalls, wxT("wxIDataObject::GetData"));

    // is data is in our format?
    HRESULT hr = QueryGetData(pformatetcIn);
    if ( FAILED(hr) )
        return hr;

    // for the bitmaps and metafiles we use the handles instead of global memory
    // to pass the data
    wxDataFormat format = (wxDataFormatId)pformatetcIn->cfFormat;

    switch ( format )
    {
        case wxDF_BITMAP:
            pmedium->tymed = TYMED_GDI;
            break;

        case wxDF_ENHMETAFILE:
            pmedium->tymed = TYMED_ENHMF;
            break;

        case wxDF_METAFILE:
            pmedium->hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE,
                                           sizeof(METAFILEPICT));
            if ( !pmedium->hGlobal ) {
                wxLogLastError("GlobalAlloc");
                return E_OUTOFMEMORY;
            }
            pmedium->tymed = TYMED_MFPICT;
            break;

        default:
            // alloc memory
            size_t size = m_pDataObject->GetDataSize(format);
            if ( !size ) {
                // it probably means that the method is just not implemented
                wxLogDebug(wxT("Invalid data size - can't be 0"));

                return DV_E_FORMATETC;
            }

            if ( !format.IsStandard() ) {
                // for custom formats, put the size with the data - alloc the
                // space for it
                size += sizeof(size_t);
            }

            HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, size);
            if ( hGlobal == NULL ) {
                wxLogLastError("GlobalAlloc");
                return E_OUTOFMEMORY;
            }

            // copy data
            pmedium->tymed   = TYMED_HGLOBAL;
            pmedium->hGlobal = hGlobal;
    }

    pmedium->pUnkForRelease = NULL;

    // do copy the data
    hr = GetDataHere(pformatetcIn, pmedium);
    if ( FAILED(hr) ) {
        // free resources we allocated
        if ( pmedium->tymed & (TYMED_HGLOBAL | TYMED_MFPICT) ) {
            GlobalFree(pmedium->hGlobal);
        }

        return hr;
    }

    return S_OK;
}

STDMETHODIMP wxIDataObject::GetDataHere(FORMATETC *pformatetc,
                                        STGMEDIUM *pmedium)
{
    wxLogTrace(wxTRACE_OleCalls, wxT("wxIDataObject::GetDataHere"));

    // put data in caller provided medium
    switch ( pmedium->tymed )
    {
        case TYMED_GDI:
            if ( !m_pDataObject->GetDataHere(wxDF_BITMAP, &pmedium->hBitmap) )
                return E_UNEXPECTED;
            break;

        case TYMED_ENHMF:
            if ( !m_pDataObject->GetDataHere(wxDF_ENHMETAFILE,
                                             &pmedium->hEnhMetaFile) )
                return E_UNEXPECTED;
            break;

        case TYMED_MFPICT:
            // fall through - we pass METAFILEPICT through HGLOBAL

        case TYMED_HGLOBAL:
            {
                // copy data
                HGLOBAL hGlobal = pmedium->hGlobal;
                void *pBuf = GlobalLock(hGlobal);
                if ( pBuf == NULL ) {
                    wxLogLastError(wxT("GlobalLock"));
                    return E_OUTOFMEMORY;
                }

                if ( !wxDataFormat(pformatetc->cfFormat).IsStandard() ) {
                    // for custom formats, put the size with the data
                    size_t *p = (size_t *)pBuf;
                    *p++ = GlobalSize(hGlobal);
                    pBuf = p;
                }

                wxDataFormat format = pformatetc->cfFormat;
                if ( !m_pDataObject->GetDataHere(format, pBuf) )
                    return E_UNEXPECTED;

                GlobalUnlock(hGlobal);
            }
            break;

        default:
            return DV_E_TYMED;
    }

    return S_OK;
}

// set data functions
STDMETHODIMP wxIDataObject::SetData(FORMATETC *pformatetc,
                                    STGMEDIUM *pmedium,
                                    BOOL       fRelease)
{
    wxLogTrace(wxTRACE_OleCalls, wxT("wxIDataObject::SetData"));

    switch ( pmedium->tymed )
    {
        case TYMED_GDI:
            m_pDataObject->SetData(wxDF_BITMAP, 0, &pmedium->hBitmap);
            break;

        case TYMED_ENHMF:
            m_pDataObject->SetData(wxDF_ENHMETAFILE, 0, &pmedium->hEnhMetaFile);
            break;

        case TYMED_MFPICT:
            // fall through - we pass METAFILEPICT through HGLOBAL
        case TYMED_HGLOBAL:
            {
                wxDataFormat format = pformatetc->cfFormat;

                // this is quite weird, but for file drag and drop, explorer
                // calls our SetData() with the formats we do *not* support!
                //
                // as we can't fix this bug in explorer (it's a bug because it
                // should only use formats returned by EnumFormatEtc), do the
                // check here
                if ( !m_pDataObject->IsSupportedFormat(format) ) {
                    // go away!
                    return DV_E_FORMATETC;
                }

                // copy data
                void *pBuf = GlobalLock(pmedium->hGlobal);
                if ( pBuf == NULL ) {
                    wxLogLastError("GlobalLock");

                    return E_OUTOFMEMORY;
                }

                // we've got a problem with SetData() here because the base
                // class version requires the size parameter which we don't
                // have anywhere in OLE data transfer - so we need to
                // synthetise it for known formats and we suppose that all data
                // in custom formats starts with a DWORD containing the size
                size_t size;
                switch ( format )
                {
                    case CF_TEXT:
                    case CF_OEMTEXT:
                        size = strlen((const char *)pBuf);
                        break;
#if !defined(__WATCOMC__) && ! (defined(__BORLANDC__) && (__BORLANDC__ < 0x500))
                    case CF_UNICODETEXT:
#if (defined(__BORLANDC__) && (__BORLANDC__ > 0x530))
                        size = std::wcslen((const wchar_t *)pBuf);
#else
                        size = ::wcslen((const wchar_t *)pBuf);
#endif
                        break;
#endif
                    case CF_BITMAP:
                    case CF_HDROP:
                        // these formats don't use size at all, anyhow (but
                        // pass data by handle, which is always a single DWORD)
                        size = 0;
                        break;

                    case CF_DIB:
                        // the handler will calculate size itself (it's too
                        // complicated to do it here)
                        size = 0;
                        break;

                    case CF_METAFILEPICT:
                        size = sizeof(METAFILEPICT);
                        break;

                    default:
                        {
                            // we suppose that the size precedes the data
                            size_t *p = (size_t *)pBuf;
                            size = *p++;
                            pBuf = p;
                        }
                }

                bool ok = m_pDataObject->SetData(format, size, pBuf);

                GlobalUnlock(pmedium->hGlobal);

                if ( !ok ) {
                    return E_UNEXPECTED;
                }
            }
            break;

        default:
            return DV_E_TYMED;
    }

    if ( fRelease ) {
        // we own the medium, so we must release it - but do *not* free any
        // data we pass by handle because we have copied it elsewhere
        switch ( pmedium->tymed )
        {
            case TYMED_GDI:
                pmedium->hBitmap = 0;
                break;

            case TYMED_MFPICT:
                pmedium->hMetaFilePict = 0;
                break;

            case TYMED_ENHMF:
                pmedium->hEnhMetaFile = 0;
                break;
        }

        ReleaseStgMedium(pmedium);
    }

    return S_OK;
}

// information functions
STDMETHODIMP wxIDataObject::QueryGetData(FORMATETC *pformatetc)
{
    // do we accept data in this format?
    if ( pformatetc == NULL ) {
        wxLogTrace(wxTRACE_OleCalls,
                   wxT("wxIDataObject::QueryGetData: invalid ptr."));

        return E_INVALIDARG;
    }

    // the only one allowed by current COM implementation
    if ( pformatetc->lindex != -1 ) {
        wxLogTrace(wxTRACE_OleCalls,
                   wxT("wxIDataObject::QueryGetData: bad lindex %d"),
                   pformatetc->lindex);

        return DV_E_LINDEX;
    }

    // we don't support anything other (THUMBNAIL, ICON, DOCPRINT...)
    if ( pformatetc->dwAspect != DVASPECT_CONTENT ) {
        wxLogTrace(wxTRACE_OleCalls,
                   wxT("wxIDataObject::QueryGetData: bad dwAspect %d"),
                   pformatetc->dwAspect);

        return DV_E_DVASPECT;
    }

    // and now check the type of data requested
    wxDataFormat format = pformatetc->cfFormat;
    if ( m_pDataObject->IsSupportedFormat(format) ) {
        wxLogTrace(wxTRACE_OleCalls, wxT("wxIDataObject::QueryGetData: %s ok"),
                   wxGetFormatName(format));
    }
    else {
        wxLogTrace(wxTRACE_OleCalls,
                   wxT("wxIDataObject::QueryGetData: %s unsupported"),
                   wxGetFormatName(format));

        return DV_E_FORMATETC;
    }

    // we only transfer data by global memory, except for some particular cases
    DWORD tymed = pformatetc->tymed;
    if ( (format == wxDF_BITMAP && !(tymed & TYMED_GDI)) &&
         !(tymed & TYMED_HGLOBAL) ) {
        // it's not what we're waiting for
        wxLogTrace(wxTRACE_OleCalls,
                   wxT("wxIDataObject::QueryGetData: %s != %s"),
                   GetTymedName(tymed),
                   GetTymedName(format == wxDF_BITMAP ? TYMED_GDI
                                                      : TYMED_HGLOBAL));

        return DV_E_TYMED;
    }

    return S_OK;
}

STDMETHODIMP wxIDataObject::GetCanonicalFormatEtc(FORMATETC *pFormatetcIn,
                                                  FORMATETC *pFormatetcOut)
{
    wxLogTrace(wxTRACE_OleCalls, wxT("wxIDataObject::GetCanonicalFormatEtc"));

    // TODO we might want something better than this trivial implementation here
    if ( pFormatetcOut != NULL )
        pFormatetcOut->ptd = NULL;

    return DATA_S_SAMEFORMATETC;
}

STDMETHODIMP wxIDataObject::EnumFormatEtc(DWORD dwDir,
                                          IEnumFORMATETC **ppenumFormatEtc)
{
    wxLogTrace(wxTRACE_OleCalls, wxT("wxIDataObject::EnumFormatEtc"));

    wxDataObject::Direction dir = dwDir == DATADIR_GET ? wxDataObject::Get
                                                       : wxDataObject::Set;

    size_t nFormatCount = m_pDataObject->GetFormatCount(dir);
    wxDataFormat format, *formats;
    formats = nFormatCount == 1 ? &format : new wxDataFormat[nFormatCount];
    m_pDataObject->GetAllFormats(formats, dir);

    wxIEnumFORMATETC *pEnum = new wxIEnumFORMATETC(formats, nFormatCount);
    pEnum->AddRef();
    *ppenumFormatEtc = pEnum;

    if ( formats != &format ) {
        delete [] formats;
    }

    return S_OK;
}

// ----------------------------------------------------------------------------
// advise sink functions (not implemented)
// ----------------------------------------------------------------------------

STDMETHODIMP wxIDataObject::DAdvise(FORMATETC   *pformatetc,
                                    DWORD        advf,
                                    IAdviseSink *pAdvSink,
                                    DWORD       *pdwConnection)
{
  return OLE_E_ADVISENOTSUPPORTED;
}

STDMETHODIMP wxIDataObject::DUnadvise(DWORD dwConnection)
{
  return OLE_E_ADVISENOTSUPPORTED;
}

STDMETHODIMP wxIDataObject::EnumDAdvise(IEnumSTATDATA **ppenumAdvise)
{
  return OLE_E_ADVISENOTSUPPORTED;
}

// ----------------------------------------------------------------------------
// wxDataObject
// ----------------------------------------------------------------------------

wxDataObject::wxDataObject()
{
    m_pIDataObject = new wxIDataObject(this);
    m_pIDataObject->AddRef();
}

wxDataObject::~wxDataObject()
{
    ReleaseInterface(m_pIDataObject);
}

void wxDataObject::SetAutoDelete()
{
    ((wxIDataObject *)m_pIDataObject)->SetDeleteFlag();
    m_pIDataObject->Release();

    // so that the dtor doesnt' crash
    m_pIDataObject = NULL;
}

#ifdef __WXDEBUG__

const char *wxDataObject::GetFormatName(wxDataFormat format)
{
    // case 'xxx' is not a valid value for switch of enum 'wxDataFormat'
    #ifdef __VISUALC__
        #pragma warning(disable:4063)
    #endif // VC++

    static char s_szBuf[256];
    switch ( format ) {
        case CF_TEXT:         return "CF_TEXT";
        case CF_BITMAP:       return "CF_BITMAP";
        case CF_METAFILEPICT: return "CF_METAFILEPICT";
        case CF_SYLK:         return "CF_SYLK";
        case CF_DIF:          return "CF_DIF";
        case CF_TIFF:         return "CF_TIFF";
        case CF_OEMTEXT:      return "CF_OEMTEXT";
        case CF_DIB:          return "CF_DIB";
        case CF_PALETTE:      return "CF_PALETTE";
        case CF_PENDATA:      return "CF_PENDATA";
        case CF_RIFF:         return "CF_RIFF";
        case CF_WAVE:         return "CF_WAVE";
        case CF_UNICODETEXT:  return "CF_UNICODETEXT";
        case CF_ENHMETAFILE:  return "CF_ENHMETAFILE";
        case CF_HDROP:        return "CF_HDROP";
        case CF_LOCALE:       return "CF_LOCALE";

        default:
            if ( !::GetClipboardFormatName(format, s_szBuf, WXSIZEOF(s_szBuf)) )
            {
                // it must be a new predefined format we don't know the name of
                sprintf(s_szBuf, "unknown CF (0x%04x)", format.GetFormatId());
            }

            return s_szBuf;
    }

    #ifdef __VISUALC__
        #pragma warning(default:4063)
    #endif // VC++
}

#endif // Debug

// ----------------------------------------------------------------------------
// wxBitmapDataObject supports CF_DIB format
// ----------------------------------------------------------------------------

size_t wxBitmapDataObject::GetDataSize() const
{
    return wxConvertBitmapToDIB(NULL, GetBitmap());
}

bool wxBitmapDataObject::GetDataHere(void *buf) const
{
    return wxConvertBitmapToDIB((LPBITMAPINFO)buf, GetBitmap()) != 0;
}

bool wxBitmapDataObject::SetData(size_t len, const void *buf)
{
    wxBitmap bitmap(wxConvertDIBToBitmap((const LPBITMAPINFO)buf));

    if ( !bitmap.Ok() ) {
        wxFAIL_MSG(wxT("pasting/dropping invalid bitmap"));

        return FALSE;
    }

    SetBitmap(bitmap);

    return TRUE;
}

// ----------------------------------------------------------------------------
// wxBitmapDataObject2 supports CF_BITMAP format
// ----------------------------------------------------------------------------

// the bitmaps aren't passed by value as other types of data (i.e. by copying
// the data into a global memory chunk and passing it to the clipboard or
// another application or whatever), but by handle, so these generic functions
// don't make much sense to them.

size_t wxBitmapDataObject2::GetDataSize() const
{
    return 0;
}

bool wxBitmapDataObject2::GetDataHere(void *pBuf) const
{
    // we put a bitmap handle into pBuf
    *(WXHBITMAP *)pBuf = GetBitmap().GetHBITMAP();

    return TRUE;
}

bool wxBitmapDataObject2::SetData(size_t WXUNUSED(len), const void *pBuf)
{
    HBITMAP hbmp = *(HBITMAP *)pBuf;

    BITMAP bmp;
    if ( !GetObject(hbmp, sizeof(BITMAP), &bmp) )
    {
        wxLogLastError("GetObject(HBITMAP)");
    }

    wxBitmap bitmap(bmp.bmWidth, bmp.bmHeight, bmp.bmPlanes);
    bitmap.SetHBITMAP((WXHBITMAP)hbmp);

    if ( !bitmap.Ok() ) {
        wxFAIL_MSG(wxT("pasting/dropping invalid bitmap"));

        return FALSE;
    }

    SetBitmap(bitmap);

    return TRUE;
}

#if 0

size_t wxBitmapDataObject::GetDataSize(const wxDataFormat& format) const
{
    if ( format.GetFormatId() == CF_DIB )
    {
        // create the DIB
        ScreenHDC hdc;

        // shouldn't be selected into a DC or GetDIBits() would fail
        wxASSERT_MSG( !m_bitmap.GetSelectedInto(),
                      wxT("can't copy bitmap selected into wxMemoryDC") );

        // first get the info
        BITMAPINFO bi;
        if ( !GetDIBits(hdc, (HBITMAP)m_bitmap.GetHBITMAP(), 0, 0,
                        NULL, &bi, DIB_RGB_COLORS) )
        {
            wxLogLastError("GetDIBits(NULL)");

            return 0;
        }

        return sizeof(BITMAPINFO) + bi.bmiHeader.biSizeImage;
    }
    else // CF_BITMAP
    {
        // no data to copy - we don't pass HBITMAP via global memory
        return 0;
    }
}

bool wxBitmapDataObject::GetDataHere(const wxDataFormat& format,
                                     void *pBuf) const
{
    wxASSERT_MSG( m_bitmap.Ok(), wxT("copying invalid bitmap") );

    HBITMAP hbmp = (HBITMAP)m_bitmap.GetHBITMAP();
    if ( format.GetFormatId() == CF_DIB )
    {
        // create the DIB
        ScreenHDC hdc;

        // shouldn't be selected into a DC or GetDIBits() would fail
        wxASSERT_MSG( !m_bitmap.GetSelectedInto(),
                      wxT("can't copy bitmap selected into wxMemoryDC") );

        // first get the info
        BITMAPINFO *pbi = (BITMAPINFO *)pBuf;
        if ( !GetDIBits(hdc, hbmp, 0, 0, NULL, pbi, DIB_RGB_COLORS) )
        {
            wxLogLastError("GetDIBits(NULL)");

            return 0;
        }

        // and now copy the bits
        if ( !GetDIBits(hdc, hbmp, 0, pbi->bmiHeader.biHeight, pbi + 1,
                        pbi, DIB_RGB_COLORS) )
        {
            wxLogLastError("GetDIBits");

            return FALSE;
        }
    }
    else // CF_BITMAP
    {
        // we put a bitmap handle into pBuf
        *(HBITMAP *)pBuf = hbmp;
    }

    return TRUE;
}

bool wxBitmapDataObject::SetData(const wxDataFormat& format,
                                 size_t size, const void *pBuf)
{
    HBITMAP hbmp;
    if ( format.GetFormatId() == CF_DIB )
    {
        // here we get BITMAPINFO struct followed by the actual bitmap bits and
        // BITMAPINFO starts with BITMAPINFOHEADER followed by colour info
        ScreenHDC hdc;

        BITMAPINFO *pbmi = (BITMAPINFO *)pBuf;
        BITMAPINFOHEADER *pbmih = &pbmi->bmiHeader;
        hbmp = CreateDIBitmap(hdc, pbmih, CBM_INIT,
                              pbmi + 1, pbmi, DIB_RGB_COLORS);
        if ( !hbmp )
        {
            wxLogLastError("CreateDIBitmap");
        }

        m_bitmap.SetWidth(pbmih->biWidth);
        m_bitmap.SetHeight(pbmih->biHeight);
    }
    else // CF_BITMAP
    {
        // it's easy with bitmaps: we pass them by handle
        hbmp = *(HBITMAP *)pBuf;

        BITMAP bmp;
        if ( !GetObject(hbmp, sizeof(BITMAP), &bmp) )
        {
            wxLogLastError("GetObject(HBITMAP)");
        }

        m_bitmap.SetWidth(bmp.bmWidth);
        m_bitmap.SetHeight(bmp.bmHeight);
        m_bitmap.SetDepth(bmp.bmPlanes);
    }

    m_bitmap.SetHBITMAP((WXHBITMAP)hbmp);

    wxASSERT_MSG( m_bitmap.Ok(), wxT("pasting invalid bitmap") );

    return TRUE;
}

#endif // 0

// ----------------------------------------------------------------------------
// wxFileDataObject
// ----------------------------------------------------------------------------

bool wxFileDataObject::SetData(size_t WXUNUSED(size), const void *pData)
{
    m_filenames.Empty();

    // the documentation states that the first member of DROPFILES structure is
    // a "DWORD offset of double NUL terminated file list". What they mean by
    // this (I wonder if you see it immediately) is that the list starts at
    // ((char *)&(pDropFiles.pFiles)) + pDropFiles.pFiles. We're also advised
    // to use DragQueryFile to work with this structure, but not told where and
    // how to get HDROP.
    HDROP hdrop = (HDROP)pData;   // NB: it works, but I'm not sure about it

    // get number of files (magic value -1)
    UINT nFiles = ::DragQueryFile(hdrop, (unsigned)-1, NULL, 0u);

    wxCHECK_MSG ( nFiles != (UINT)-1, FALSE, wxT("wrong HDROP handle") );

    // for each file get the length, allocate memory and then get the name
    wxString str;
    UINT len, n;
    for ( n = 0; n < nFiles; n++ ) {
        // +1 for terminating NUL
        len = ::DragQueryFile(hdrop, n, NULL, 0) + 1;

        UINT len2 = ::DragQueryFile(hdrop, n, str.GetWriteBuf(len), len);
        str.UngetWriteBuf();
        m_filenames.Add(str);

        if ( len2 != len - 1 ) {
            wxLogDebug(wxT("In wxFileDropTarget::OnDrop DragQueryFile returned"
                           " %d characters, %d expected."), len2, len - 1);
        }
    }

    return TRUE;
}

void wxFileDataObject::AddFile(const wxString& file)
{
    // just add file to filenames array
    // all useful data (such as DROPFILES struct) will be
    // created later as necessary
    m_filenames.Add(file);
}

size_t wxFileDataObject::GetDataSize() const
{
    // size returned will be the size of the DROPFILES structure,
    // plus the list of filesnames (null byte separated), plus
    // a double null at the end

    // if no filenames in list, size is 0
    if ( m_filenames.GetCount() == 0 )
        return 0;

    // inital size of DROPFILES struct + null byte
    size_t sz = sizeof(DROPFILES) + 1;

    size_t count = m_filenames.GetCount();
    for ( size_t i = 0; i < count; i++ )
    {
        // add filename length plus null byte
        sz += m_filenames[i].Len() + 1;
    }

    return sz;
}

bool wxFileDataObject::GetDataHere(void *pData) const
{
    // pData points to an externally allocated memory block
    // created using the size returned by GetDataSize()

    // if pData is NULL, or there are no files, return
    if ( !pData || m_filenames.GetCount() == 0 )
        return FALSE;

    // convert data pointer to a DROPFILES struct pointer
    LPDROPFILES pDrop = (LPDROPFILES) pData;

    // initialize DROPFILES struct
    pDrop->pFiles = sizeof(DROPFILES);
    pDrop->fNC = FALSE;                 // not non-client coords
#if wxUSE_UNICODE
    pDrop->fWide = TRUE;
#else // ANSI
    pDrop->fWide = FALSE;
#endif // Unicode/Ansi

    // set start of filenames list (null separated)
    wxChar *pbuf = (wxChar*) ((BYTE *)pDrop + sizeof(DROPFILES));

    size_t count = m_filenames.GetCount();
    for (size_t i = 0; i < count; i++ )
    {
        // copy filename to pbuf and add null terminator
        size_t len = m_filenames[i].Len();
        memcpy(pbuf, m_filenames[i], len);
        pbuf += len;
        *pbuf++ = wxT('\0');
    }

    *pbuf = wxT('\0');	// add final null terminator

    return TRUE;
}

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

static size_t wxGetNumOfBitmapColors(size_t bitsPerPixel)
{
    switch ( bitsPerPixel )
    {
        case 1:
            // monochrome bitmap, 2 entries
            return 2;

        case 4:
            return 16;

        case 8:
            return 256;

        case 24:
            // may be used with 24bit bitmaps, but we don't use it here - fall
            // through

        case 16:
        case 32:
            // bmiColors not used at all with these bitmaps
            return 0;

        default:
            wxFAIL_MSG( wxT("unknown bitmap format") );
            return 0;
    }
}

size_t wxConvertBitmapToDIB(LPBITMAPINFO pbi, const wxBitmap& bitmap)
{
    wxASSERT_MSG( bitmap.Ok(), wxT("invalid bmp can't be converted to DIB") );

    // shouldn't be selected into a DC or GetDIBits() would fail
    wxASSERT_MSG( !bitmap.GetSelectedInto(),
                  wxT("can't copy bitmap selected into wxMemoryDC") );

    // prepare all the info we need
    BITMAP bm;
    HBITMAP hbmp = (HBITMAP)bitmap.GetHBITMAP();
    if ( !GetObject(hbmp, sizeof(bm), &bm) )
    {
        wxLogLastError("GetObject(bitmap)");

        return 0;
    }

    // calculate the number of bits per pixel and the number of items in
    // bmiColors array (whose meaning depends on the bitmap format)
    WORD biBits = bm.bmPlanes * bm.bmBitsPixel;
    WORD biColors = wxGetNumOfBitmapColors(biBits);

    BITMAPINFO bi2;

    bool wantSizeOnly = pbi == NULL;
    if ( wantSizeOnly )
        pbi = &bi2;

    // just for convenience
    BITMAPINFOHEADER& bi = pbi->bmiHeader;

    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bm.bmWidth;
    bi.biHeight = bm.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = biBits;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    // memory we need for BITMAPINFO only
    DWORD dwLen = bi.biSize + biColors * sizeof(RGBQUAD);

    // first get the image size
    ScreenHDC hdc;
    if ( !GetDIBits(hdc, hbmp, 0, bi.biHeight, NULL, pbi, DIB_RGB_COLORS) )
    {
        wxLogLastError("GetDIBits(NULL)");

        return 0;
    }

    if ( wantSizeOnly )
    {
        // size of the header + size of the image
        return dwLen + bi.biSizeImage;
    }

    // and now copy the bits
    void *image = (char *)pbi + dwLen;
    if ( !GetDIBits(hdc, hbmp, 0, bi.biHeight, image, pbi, DIB_RGB_COLORS) )
    {
        wxLogLastError("GetDIBits");

        return 0;
    }

    return dwLen + bi.biSizeImage;
}

wxBitmap wxConvertDIBToBitmap(const LPBITMAPINFO pbmi)
{
    // here we get BITMAPINFO struct followed by the actual bitmap bits and
    // BITMAPINFO starts with BITMAPINFOHEADER followed by colour info
    const BITMAPINFOHEADER *pbmih = &pbmi->bmiHeader;

    // offset of image from the beginning of the header
    DWORD ofs = wxGetNumOfBitmapColors(pbmih->biBitCount) * sizeof(RGBQUAD);
    void *image = (char *)pbmih + sizeof(BITMAPINFOHEADER) + ofs;

    ScreenHDC hdc;
    HBITMAP hbmp = CreateDIBitmap(hdc, pbmih, CBM_INIT,
                                  image, pbmi, DIB_RGB_COLORS);
    if ( !hbmp )
    {
        wxLogLastError("CreateDIBitmap");
    }

    wxBitmap bitmap(pbmih->biWidth, pbmih->biHeight, pbmih->biBitCount);
    bitmap.SetHBITMAP((WXHBITMAP)hbmp);

    return bitmap;
}

#ifdef __WXDEBUG__

static const wxChar *GetTymedName(DWORD tymed)
{
    static wxChar s_szBuf[128];
    switch ( tymed ) {
        case TYMED_HGLOBAL:   return wxT("TYMED_HGLOBAL");
        case TYMED_FILE:      return wxT("TYMED_FILE");
        case TYMED_ISTREAM:   return wxT("TYMED_ISTREAM");
        case TYMED_ISTORAGE:  return wxT("TYMED_ISTORAGE");
        case TYMED_GDI:       return wxT("TYMED_GDI");
        case TYMED_MFPICT:    return wxT("TYMED_MFPICT");
        case TYMED_ENHMF:     return wxT("TYMED_ENHMF");
        default:
            wxSprintf(s_szBuf, wxT("type of media format %ld (unknown)"), tymed);
            return s_szBuf;
    }
}

#endif // Debug

#endif // not using OLE at all

