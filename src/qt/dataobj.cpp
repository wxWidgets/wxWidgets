/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/dataobj.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include <QtCore/QMimeData>
#include <QtGui/QPixmap>

#include "wx/dataobj.h"
#include "wx/scopedarray.h"
#include "wx/qt/private/converter.h"

typedef wxScopedArray<wxDataFormat> wxDataFormatArray;

namespace
{

wxString DataFormatIdToMimeType(wxDataFormatId formatId)
{
    switch ( formatId )
    {
        case wxDF_TEXT: return "text/plain";
        case wxDF_BITMAP: return "image/bmp";
        case wxDF_TIFF: return "image/tiff";
        case wxDF_WAVE: return "audio/x-wav";
        case wxDF_UNICODETEXT: return "text/plain";
        case wxDF_HTML: return "text/html";
        case wxDF_METAFILE:
        case wxDF_SYLK:
        case wxDF_DIF:
        case wxDF_OEMTEXT:
        case wxDF_DIB:
        case wxDF_PALETTE:
        case wxDF_PENDATA:
        case wxDF_RIFF:
        case wxDF_ENHMETAFILE:
        case wxDF_FILENAME:
        case wxDF_LOCALE:
        case wxDF_PRIVATE:
        case wxDF_INVALID:
        case wxDF_MAX:
        default:
            return "";
    }
}

} // anonymous namespace

wxDataFormat::wxDataFormat(wxDataFormatId formatId)
{
    SetType(formatId);
}

wxDataFormat::wxDataFormat(const wxString &id)
{
    SetId(id);
}

const wxString& wxDataFormat::GetMimeType() const
{
    return m_mimeType;
}

void wxDataFormat::SetMimeType(const wxString& mimeType)
{
    m_mimeType = mimeType;
    m_formatId = wxDF_INVALID;
}

void wxDataFormat::SetId(const wxString& id)
{
    SetMimeType(id);
}

const wxString& wxDataFormat::GetId() const
{
    return m_mimeType;
}

wxDataFormatId wxDataFormat::GetType() const
{
    return m_formatId;
}

void wxDataFormat::SetType(wxDataFormatId formatId)
{
    m_mimeType = DataFormatIdToMimeType(formatId);
    m_formatId = formatId;
}

bool wxDataFormat::operator==(wxDataFormatId format) const
{
    return m_mimeType == DataFormatIdToMimeType(format)
        && m_formatId == format;
}

bool wxDataFormat::operator!=(wxDataFormatId format) const
{
    return !operator==(format);
}

bool wxDataFormat::operator==(const wxDataFormat& format) const
{
    // If mime types match, then that's good enough.
    // (Could be comparing a standard constructed format to a
    // custom constructed one, where both are actually the same.)
    if (!m_mimeType.empty() && m_mimeType == format.m_mimeType)
        return true;

    return m_mimeType == format.m_mimeType
        && m_formatId == format.m_formatId;
}

bool wxDataFormat::operator!=(const wxDataFormat& format) const
{
    return !operator==(format);
}

//############################################################################

wxDataObject::wxDataObject()
{
}

wxDataObject::~wxDataObject()
{
}

bool wxDataObject::IsSupportedFormat(const wxDataFormat& format,
                                     Direction dir) const
{
    const size_t formatCount = GetFormatCount(dir);
    if ( formatCount == 1 )
    {
        return format == GetPreferredFormat();
    }

    wxScopedArray<wxDataFormat> formats(formatCount);
    GetAllFormats(formats.get(), dir);

    for ( size_t n = 0; n < formatCount; ++n )
    {
        if ( formats[n] == format )
            return true;
    }

    return false;
}

void wxDataObject::QtAddDataTo(QMimeData &mimeData) const
{
    const size_t count = GetFormatCount();
    wxDataFormatArray formats(count);
    GetAllFormats(formats.get());

    // how to add timestamp?

    // Unfortunately I cannot find a way to use the qt clipboard with
    // a callback to select the data type, so I must copy it all here

    for (size_t i = 0; i < count; i++)
    {
        const wxDataFormat format(formats[i]);

        int size = GetDataSize(format);
        if (!size)
            continue;

        QByteArray bytearray(size, 0);
        GetDataHere(format, bytearray.data());
        mimeData.setData(wxQtConvertString(format.GetMimeType()), bytearray);
    }
}

bool wxDataObject::QtSetDataFrom(const QMimeData &mimeData)
{
    const size_t count = GetFormatCount(Set);
    wxDataFormatArray formats(count);
    GetAllFormats(formats.get(), Set);

    for (size_t i = 0; i < count; i++)
    {
        const wxDataFormat format(formats[i]);

        // is this format supported by clipboard ?
        if (!mimeData.hasFormat(wxQtConvertString(format.GetMimeType())))
            continue;

        QtSetDataSingleFormat(mimeData, format);
        return true;
    }

    return false;
}

void wxDataObject::QtSetDataSingleFormat(const QMimeData &mimeData, const wxDataFormat &format)
{
    QByteArray bytearray = mimeData.data(wxQtConvertString(format.GetMimeType()));
    SetData(format, bytearray.size(), bytearray.constData());
}

//############################################################################

wxBitmapDataObject::wxBitmapDataObject()
{
}

wxBitmapDataObject::wxBitmapDataObject( const wxBitmap &bitmap )
    : wxBitmapDataObjectBase( bitmap )
{
}

void wxBitmapDataObject::QtAddDataTo(QMimeData &mimeData) const
{
    mimeData.setImageData(GetBitmap().GetHandle()->toImage());
}

bool wxBitmapDataObject::QtSetDataFrom(const QMimeData &mimeData)
{
    if (!mimeData.hasImage())
        return false;

    SetBitmap(wxBitmap(QPixmap::fromImage(qvariant_cast<QImage>(mimeData.imageData()))));
    return true;
}

//#############################################################################
// ----------------------------------------------------------------------------
// wxTextDataObject
// ---------------------------------------------------------------------------

void wxTextDataObject::GetAllFormats(wxDataFormat *formats,
    wxDataObjectBase::Direction WXUNUSED(dir)) const
{
    formats[0] = wxDataFormat(wxDF_UNICODETEXT);
    formats[1] = wxDataFormat(wxDF_TEXT);
}

void wxTextDataObject::QtSetDataSingleFormat(const QMimeData &mimeData, const wxDataFormat &WXUNUSED(format))
{
    SetText(wxQtConvertString(mimeData.text()));
}

//#############################################################################

wxFileDataObject::wxFileDataObject()
{
}

void wxFileDataObject::AddFile( const wxString &WXUNUSED(filename) )
{

}
