/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/dataform.h
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/qt/private/converter.h"
#include "wx/dataobj.h"


wxDataFormat::wxDataFormat()
{
}

static QString DataFormatIdToMimeType( wxDataFormatId formatId )
{
    switch(formatId) {
    case wxDF_TEXT: return "text/plain";
    case wxDF_BITMAP: return "image/bmp";
//    case wxDF_METAFILE: return "";
//    case wxDF_SYLK: return "";
//    case wxDF_DIF: return "";
    case wxDF_TIFF: return "image/tiff";
//    case wxDF_OEMTEXT: return "";
//    case wxDF_DIB: return "";
//    case wxDF_PALETTE: return "";
//    case wxDF_PENDATA: return "";
//    case wxDF_RIFF: return "";
    case wxDF_WAVE: return "audio/x-wav";
    case wxDF_UNICODETEXT: return "text/plain";
//    case wxDF_ENHMETAFILE: return "";
//    case wxDF_FILENAME: return "";
//    case wxDF_LOCALE: return "";
//    case wxDF_PRIVATE: return "";
    case wxDF_HTML: return "text/html";
    default: break;
    }
    return "";
}

wxDataFormat::wxDataFormat( wxDataFormatId formatId )
{
    m_MimeType = DataFormatIdToMimeType(formatId);
}

wxDataFormat::wxDataFormat(const wxString &id)
{
    m_MimeType = wxQtConvertString(id);
}

wxDataFormat::wxDataFormat(const wxChar *id)
{
    m_MimeType = wxQtConvertString(id);
}

wxDataFormat::wxDataFormat(const QString &id)
{
    m_MimeType = id;
}

void wxDataFormat::SetId( const wxChar *id )
{
    m_MimeType = wxQtConvertString(id);
}

bool wxDataFormat::operator==(wxDataFormatId format) const
{
    return m_MimeType == DataFormatIdToMimeType(format);
}

bool wxDataFormat::operator!=(wxDataFormatId format) const
{
    return m_MimeType != DataFormatIdToMimeType(format);
}

bool wxDataFormat::operator==(const wxDataFormat& format) const
{
    return m_MimeType == format.m_MimeType;
}

bool wxDataFormat::operator!=(const wxDataFormat& format) const
{
    return m_MimeType != format.m_MimeType;
}

//#############################################################################

bool wxDataObject::IsSupportedFormat(const wxDataFormat& format, Direction) const
{
    return wxDataFormat(format) != wxDF_INVALID;
}
wxDataFormat wxDataObject::GetPreferredFormat(Direction) const
{
    /* formats are in order of preference */
    if (m_qtMimeData.formats().count())
        return m_qtMimeData.formats().first();

    return wxDataFormat();
}

size_t wxDataObject::GetFormatCount(Direction) const
{
    return m_qtMimeData.formats().count();
}

void wxDataObject::GetAllFormats(wxDataFormat *formats, Direction) const
{
    int i = 0;
    foreach (QString format, m_qtMimeData.formats())
    {
        formats[i] = format;
        i++;
    }
}

size_t wxDataObject::GetDataSize(const wxDataFormat& format) const
{
    return  m_qtMimeData.data( format.m_MimeType ).count();
}

bool wxDataObject::GetDataHere(const wxDataFormat& format, void *buf) const
{
    if (!m_qtMimeData.hasFormat(format.m_MimeType))
        return false;

    QByteArray data = m_qtMimeData.data( format.m_MimeType ).data();
    memcpy(buf, data.constData(), data.size());
    return true;
}

bool wxDataObject::SetData(const wxDataFormat& format, size_t len, const void * buf)
{
    QByteArray bytearray((const char*)buf, len);
    m_qtMimeData.setData(format.m_MimeType, bytearray);

    return true;
}

wxBitmapDataObject::wxBitmapDataObject()
{
}

wxBitmapDataObject::wxBitmapDataObject( const wxBitmap &WXUNUSED(bitmap) )
{
}

wxFileDataObject::wxFileDataObject()
{
}
 
void wxFileDataObject::AddFile( const wxString &WXUNUSED(filename) )
{

}
