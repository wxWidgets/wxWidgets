///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/dobjcmn.cpp
// Purpose:     implementation of data object methods common to all platforms
// Author:      Vadim Zeitlin, Robert Roebling
// Modified by:
// Created:     19.10.99
// Copyright:   (c) wxWidgets Team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_DATAOBJ

#include "wx/dataobj.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif

#include "wx/mstream.h"
#include "wx/textbuf.h"

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

static wxDataFormat dataFormatInvalid;
WXDLLEXPORT const wxDataFormat& wxFormatInvalid = dataFormatInvalid;

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxDataObjectBase
// ----------------------------------------------------------------------------

wxDataObjectBase::~wxDataObjectBase()
{
}

bool wxDataObjectBase::IsSupported(const wxDataFormat& format,
                                   Direction dir) const
{
    size_t nFormatCount = GetFormatCount( dir );
    if ( nFormatCount == 1 )
    {
        return format == GetPreferredFormat( dir );
    }
    else
    {
        wxDataFormat *formats = new wxDataFormat[nFormatCount];
        GetAllFormats( formats, dir );

        size_t n;
        for ( n = 0; n < nFormatCount; n++ )
        {
            if ( formats[n] == format )
                break;
        }

        delete [] formats;

        // found?
        return n < nFormatCount;
    }
}

// ----------------------------------------------------------------------------
// wxDataObjectComposite
// ----------------------------------------------------------------------------

wxDataObjectComposite::wxDataObjectComposite()
{
    m_preferred = 0;
    m_receivedFormat = wxFormatInvalid;
}

wxDataObjectComposite::~wxDataObjectComposite() = default;

wxDataObjectSimple *
wxDataObjectComposite::GetObject(const wxDataFormat& format, wxDataObjectBase::Direction dir) const
{
    for ( const auto& dataObj : m_dataObjects )
    {
        if (dataObj->IsSupported(format,dir))
          return dataObj.get();
    }
    return nullptr;
}

void wxDataObjectComposite::Add(wxDataObjectSimple *dataObject, bool preferred)
{
    if ( preferred )
        m_preferred = m_dataObjects.size();

    m_dataObjects.push_back(std::unique_ptr<wxDataObjectSimple>(dataObject));
}

wxDataFormat wxDataObjectComposite::GetReceivedFormat() const
{
    return m_receivedFormat;
}

wxDataFormat
wxDataObjectComposite::GetPreferredFormat(Direction WXUNUSED(dir)) const
{
    wxCHECK_MSG( m_preferred < m_dataObjects.size(), wxFormatInvalid,
                 wxT("no preferred format") );

    return m_dataObjects[m_preferred]->GetFormat();
}

#if defined(__WXMSW__)

size_t wxDataObjectComposite::GetBufferOffset( const wxDataFormat& format )
{
    wxDataObjectSimple *dataObj = GetObject(format);

    wxCHECK_MSG( dataObj, 0,
                 wxT("unsupported format in wxDataObjectComposite"));

    return dataObj->GetBufferOffset( format );
}


const void* wxDataObjectComposite::GetSizeFromBuffer( const void* buffer,
                                                      size_t* size,
                                                      const wxDataFormat& format )
{
    wxDataObjectSimple *dataObj = GetObject(format);

    wxCHECK_MSG( dataObj, nullptr,
                 wxT("unsupported format in wxDataObjectComposite"));

    return dataObj->GetSizeFromBuffer( buffer, size, format );
}


void* wxDataObjectComposite::SetSizeInBuffer( void* buffer, size_t size,
                                              const wxDataFormat& format )
{
    wxDataObjectSimple *dataObj = GetObject( format );

    wxCHECK_MSG( dataObj, nullptr,
                 wxT("unsupported format in wxDataObjectComposite"));

    return dataObj->SetSizeInBuffer( buffer, size, format );
}

#endif

size_t wxDataObjectComposite::GetFormatCount(Direction dir) const
{
    size_t n = 0;

    // NOTE: some wxDataObjectSimple objects may return a number greater than 1
    //       from GetFormatCount(): this is the case of e.g. wxTextDataObject
    //       under wxMac and wxGTK
    for ( const auto& dataObj : m_dataObjects )
        n += dataObj->GetFormatCount(dir);

    return n;
}

void wxDataObjectComposite::GetAllFormats(wxDataFormat *formats,
                                          Direction dir) const
{
    size_t index(0);

    for ( const auto& dataObj : m_dataObjects )
    {
        // NOTE: some wxDataObjectSimple objects may return more than 1 format
        //       from GetAllFormats(): this is the case of e.g. wxTextDataObject
        //       under wxMac and wxGTK
        dataObj->GetAllFormats(formats+index, dir);
        index += dataObj->GetFormatCount(dir);
    }
}

size_t wxDataObjectComposite::GetDataSize(const wxDataFormat& format) const
{
    wxDataObjectSimple *dataObj = GetObject(format);

    wxCHECK_MSG( dataObj, 0,
                 wxT("unsupported format in wxDataObjectComposite"));

    return dataObj->GetDataSize();
}

bool wxDataObjectComposite::GetDataHere(const wxDataFormat& format,
                                        void *buf) const
{
    wxDataObjectSimple *dataObj = GetObject( format );

    wxCHECK_MSG( dataObj, false,
                 wxT("unsupported format in wxDataObjectComposite"));

    return dataObj->GetDataHere( buf );
}

bool wxDataObjectComposite::SetData(const wxDataFormat& format,
                                    size_t len,
                                    const void *buf)
{
    wxDataObjectSimple *dataObj = GetObject( format );

    wxCHECK_MSG( dataObj, false,
                 wxT("unsupported format in wxDataObjectComposite"));

    m_receivedFormat = format;

    // Notice that we must pass "format" here as wxTextDataObject, that we can
    // have as one of our "simple" sub-objects actually is not that simple and
    // can support multiple formats (ASCII/UTF-8/UTF-16/...) and so needs to
    // know which one it is given.
    return dataObj->SetData( format, len, buf );
}

// ----------------------------------------------------------------------------
// wxTextDataObject
// ----------------------------------------------------------------------------

#ifdef wxNEEDS_UTF8_FOR_TEXT_DATAOBJ

// FIXME-UTF8: we should be able to merge wchar_t and UTF-8 versions once we
//             have a way to get UTF-8 string (and its length) in both builds
//             without loss of efficiency (i.e. extra buffer copy/strlen call)

#if wxUSE_UNICODE_WCHAR

static inline wxMBConv& GetConv(const wxDataFormat& format)
{
    // use UTF8 for wxDF_UNICODETEXT and UCS4 for wxDF_TEXT
    return format == wxDF_UNICODETEXT ? wxConvUTF8 : wxConvLibc;
}

size_t wxTextDataObject::GetDataSize(const wxDataFormat& format) const
{
    wxCharBuffer buffer = GetConv(format).cWX2MB( GetText().c_str() );

    return buffer ? strlen( buffer ) : 0;
}

bool wxTextDataObject::GetDataHere(const wxDataFormat& format, void *buf) const
{
    if ( !buf )
        return false;

    wxCharBuffer buffer = GetConv(format).cWX2MB( GetText().c_str() );
    if ( !buffer )
        return false;

    memcpy( (char*) buf, buffer, GetDataSize(format) );
    // strcpy( (char*) buf, buffer );

    return true;
}

bool wxTextDataObject::SetData(const wxDataFormat& format,
                               size_t len, const void *buf)
{
    if ( buf == nullptr )
        return false;

    wxWCharBuffer buffer = GetConv(format).cMB2WC((const char*)buf, len, nullptr);

    SetText( buffer );

    return true;
}

#else // wxUSE_UNICODE_UTF8

size_t wxTextDataObject::GetDataSize(const wxDataFormat& format) const
{
    const wxString& text = GetText();
    if ( format == wxDF_UNICODETEXT || wxLocaleIsUtf8 )
    {
        return text.utf8_length();
    }
    else // wxDF_TEXT
    {
        const wxCharBuffer buf(wxConvLocal.cWC2MB(text.wc_str()));
        return buf ? strlen(buf) : 0;
    }
}

bool wxTextDataObject::GetDataHere(const wxDataFormat& format, void *buf) const
{
    if ( !buf )
        return false;

    const wxString& text = GetText();
    if ( format == wxDF_UNICODETEXT || wxLocaleIsUtf8 )
    {
        memcpy(buf, text.utf8_str(), text.utf8_length());
    }
    else // wxDF_TEXT
    {
        const wxCharBuffer bufLocal(wxConvLocal.cWC2MB(text.wc_str()));
        if ( !bufLocal )
            return false;

        memcpy(buf, bufLocal, strlen(bufLocal));
    }

    return true;
}

bool wxTextDataObject::SetData(const wxDataFormat& format,
                               size_t len, const void *buf_)
{
    const char * const buf = static_cast<const char *>(buf_);

    if ( buf == nullptr )
        return false;

    if ( format == wxDF_UNICODETEXT || wxLocaleIsUtf8 )
    {
        // normally the data is in UTF-8 so we could use FromUTF8Unchecked()
        // but it's not absolutely clear what GTK+ does if the clipboard data
        // is not in UTF-8 so do an extra check for tranquility, it shouldn't
        // matter much if we lose a bit of performance when pasting from
        // clipboard
        SetText(wxString::FromUTF8(buf, len));
    }
    else // wxDF_TEXT, convert from current (non-UTF8) locale
    {
        SetText(wxConvLocal.cMB2WC(buf, len, nullptr));
    }

    return true;
}

#endif // wxUSE_UNICODE_WCHAR/wxUSE_UNICODE_UTF8

#elif defined(wxNEEDS_UTF16_FOR_TEXT_DATAOBJ)

namespace
{

inline wxMBConv& GetConv(const wxDataFormat& format)
{
    static wxMBConvUTF16 s_UTF16Converter;

    return format == wxDF_UNICODETEXT ? static_cast<wxMBConv&>(s_UTF16Converter)
                                      : static_cast<wxMBConv&>(wxConvLocal);
}

} // anonymous namespace

size_t wxTextDataObject::GetDataSize(const wxDataFormat& format) const
{
    return GetConv(format).WC2MB(nullptr, GetText().wc_str(), 0);
}

bool wxTextDataObject::GetDataHere(const wxDataFormat& format, void *buf) const
{
    if ( buf == nullptr )
        return false;

    wxCharBuffer buffer(GetConv(format).cWX2MB(GetText().c_str()));

    memcpy(buf, buffer.data(), buffer.length());

    return true;
}

bool wxTextDataObject::SetData(const wxDataFormat& format,
                               size_t len,
                               const void *buf)
{
    if ( buf == nullptr )
        return false;

    SetText(GetConv(format).cMB2WC(static_cast<const char*>(buf), len, nullptr));

    return true;
}

#else // !wxNEEDS_UTF{8,16}_FOR_TEXT_DATAOBJ

// NB: This branch, using native wxChar for the clipboard, is only used under
//     Windows currently. It's just a coincidence, but Windows is also the only
//     platform where we need to convert the text to the native EOL format, so
//     wxTextBuffer::Translate() is only used here and not in the code above.

size_t wxTextDataObject::GetDataSize() const
{
    return (wxTextBuffer::Translate(GetText()).length() + 1)*sizeof(wxChar);
}

bool wxTextDataObject::GetDataHere(void *buf) const
{
    const wxString textNative = wxTextBuffer::Translate(GetText());

    // NOTE: use wxTmemcpy() instead of wxStrncpy() to allow
    //       retrieval of strings with embedded NULs
    wxTmemcpy(static_cast<wxChar*>(buf),
              textNative.t_str(),
              textNative.length() + 1);

    return true;
}

bool wxTextDataObject::SetData(size_t len, const void *buf)
{
    // Some sanity checks to avoid problems below.
    wxCHECK_MSG( len, false, "data can't be empty" );
    wxCHECK_MSG( !(len % sizeof(wxChar)), false, "wrong data size" );

    // Input data is always NUL-terminated, but we don't want to make this NUL
    // part of the string, so take everything up to but excluding it.
    const size_t size = len/sizeof(wxChar) - 1;

    const wxString text(static_cast<const wxChar*>(buf), size);
    SetText(wxTextBuffer::Translate(text, wxTextFileType_Unix));

    return true;
}

#endif // different wxTextDataObject implementations

// ----------------------------------------------------------------------------
// wxHTMLDataObject
// ----------------------------------------------------------------------------

#ifdef __WXMSW__

// Helper functions for MSW CF_HTML format, see MSDN for more information:
//
// https://learn.microsoft.com/en-us/windows/win32/dataxchg/html-clipboard-format
namespace wxMSWClip
{

const char* const VERSION_HEADER = "Version:";
const size_t VERSION_HEADER_LEN = strlen(VERSION_HEADER);

const char* const START_HTML_HEADER = "StartHTML:";
const size_t START_HTML_HEADER_LEN = strlen(START_HTML_HEADER);

const char* const END_HTML_HEADER = "EndHTML:";
const size_t END_HTML_HEADER_LEN = strlen(END_HTML_HEADER);

const char* const START_FRAGMENT_HEADER = "StartFragment:";
const size_t START_FRAGMENT_HEADER_LEN = strlen(START_FRAGMENT_HEADER);

const char* const END_FRAGMENT_HEADER = "EndFragment:";
const size_t END_FRAGMENT_HEADER_LEN = strlen(END_FRAGMENT_HEADER);

const char* const CF_HTML_PREAMBLE =
        "Version:0.9\r\n"
        "StartHTML:00000000\r\n"
        "EndHTML:00000000\r\n"
        "StartFragment:00000000\r\n"
        "EndFragment:00000000\r\n"
        ;
const size_t CF_HTML_PREAMBLE_LEN = strlen(CF_HTML_PREAMBLE);

const char* const CF_HTML_WRAP_START =
        "<html><body>\r\n"
        "<!--StartFragment -->"
        ;
const size_t CF_HTML_WRAP_START_LEN = strlen(CF_HTML_WRAP_START);

const char* const CF_HTML_WRAP_END =
        "<!--EndFragment-->\r\n"
        "</body>\r\n"
        "</html>"
        ;
const size_t CF_HTML_WRAP_END_LEN = strlen(CF_HTML_WRAP_END);


// Return the extra size needed by HTML data in addition to the length of the
// HTML fragment itself.
int GetExtraDataSize()
{
    // +1 is for the trailing NUL.
    return CF_HTML_PREAMBLE_LEN + CF_HTML_WRAP_START_LEN + CF_HTML_WRAP_END_LEN + 1;
}

// Wrap HTML data with the extra information needed by CF_HTML and copy
// everything into the provided buffer assumed to be of sufficient size.
void FillFromHTML(char* buffer, const char* html, size_t lenHTML)
{
    // add the extra info that the MSW clipboard format requires.

        // Create a template string for the HTML header...
    strcpy(buffer, CF_HTML_PREAMBLE);
    const size_t startHTML = CF_HTML_PREAMBLE_LEN;

    strcat(buffer, CF_HTML_WRAP_START);
    const size_t startFragment = startHTML + CF_HTML_WRAP_START_LEN;

    // Append the HTML...
    strncat(buffer, html, lenHTML);
    const size_t endFragment = startFragment + lenHTML;

    // Finish up the HTML format...
    strcat(buffer, CF_HTML_WRAP_END);
    const size_t endHTML = endFragment + CF_HTML_WRAP_END_LEN;

    // Now go back and write out the necessary header information.
    //
    // Note, wsprintf() truncates the string when you overwrite it so you
    // follow up with code to replace the 0 appended at the end with a '\r'.
    const size_t OFFSET_LEN = 8; // All offsets are formatted using 8 digits.

    char *ptr = strstr(buffer, START_HTML_HEADER);
    sprintf(ptr+START_HTML_HEADER_LEN, "%08zu", startHTML);
    *(ptr+START_HTML_HEADER_LEN+OFFSET_LEN) = '\r';

    ptr = strstr(buffer, END_HTML_HEADER);
    sprintf(ptr+END_HTML_HEADER_LEN, "%08zu", endHTML);
    *(ptr+END_HTML_HEADER_LEN+OFFSET_LEN) = '\r';

    ptr = strstr(buffer, START_FRAGMENT_HEADER);
    sprintf(ptr+START_FRAGMENT_HEADER_LEN, "%08zu", startFragment);
    *(ptr+START_FRAGMENT_HEADER_LEN+OFFSET_LEN) = '\r';

    ptr = strstr(buffer, END_FRAGMENT_HEADER);
    sprintf(ptr+END_FRAGMENT_HEADER_LEN, "%08zu", endFragment);
    *(ptr+END_FRAGMENT_HEADER_LEN+OFFSET_LEN) = '\r';
}

// Extract just the HTML fragment part from CF_HTML data.
wxString ExtractHTML(const char* buffer, size_t len)
{
    // Sanity check.
    if ( len < VERSION_HEADER_LEN ||
            wxCRT_StrnicmpA(buffer, VERSION_HEADER, VERSION_HEADER_LEN) != 0 )
    {
        // This doesn't look like CF_HTML at all, don't do anything.
        return wxString();
    }

    const char* ptr = strstr(buffer, START_FRAGMENT_HEADER);
    if ( !ptr )
        return wxString();

    ptr += START_FRAGMENT_HEADER_LEN;

    const int start = atoi(ptr);
    if ( start < 0 || (unsigned)start >= len )
        return wxString();

    ptr = strstr(ptr, END_FRAGMENT_HEADER);
    if ( !ptr )
        return wxString();

    ptr += END_FRAGMENT_HEADER_LEN;
    const int end = atoi(ptr);
    if ( end < 0 || end < start || (unsigned)end >= len )
        return wxString();

    return wxString::FromUTF8(buffer + start, end - start);
}

} // anonymous namespace

#endif // __WXMSW__

size_t wxHTMLDataObject::GetDataSize() const
{
    // Ensure that the temporary string returned by GetHTML() is kept alive for
    // as long as we need it here.
    const wxString& htmlStr = GetHTML();
    const wxScopedCharBuffer buffer(htmlStr.utf8_str());

    size_t size = buffer.length();

#ifdef __WXMSW__
    size += wxMSWClip::GetExtraDataSize();
#endif

    return size;
}

bool wxHTMLDataObject::GetDataHere(void *buf) const
{
    if ( !buf )
        return false;

    // Windows and Mac always use UTF-8, and docs suggest GTK does as well.
    const wxString& htmlStr = GetHTML();
    const wxScopedCharBuffer html(htmlStr.utf8_str());
    if ( !html )
        return false;

    char* const buffer = static_cast<char*>(buf);

#ifdef __WXMSW__
    wxMSWClip::FillFromHTML(buffer, html, html.length());
#else
    memcpy(buffer, html, html.length());
#endif // __WXMSW__

    return true;
}

bool wxHTMLDataObject::SetData(size_t len, const void *buf)
{
    if ( buf == nullptr )
        return false;

    const char* const buffer = static_cast<const char*>(buf);

#ifdef __WXMSW__
    // To be consistent with other platforms, we only add the Fragment part
    // of the Windows HTML clipboard format to the data object.
    wxString html = wxMSWClip::ExtractHTML(buffer, len);
#else
    wxString html = wxString::FromUTF8(buffer, len);
#endif // __WXMSW__

    SetHTML( html );

    return true;
}


// ----------------------------------------------------------------------------
// wxCustomDataObject
// ----------------------------------------------------------------------------

wxCustomDataObject::wxCustomDataObject(const wxDataFormat& format)
    : wxDataObjectSimple(format)
{
    m_data = nullptr;
    m_size = 0;
}

wxCustomDataObject::~wxCustomDataObject()
{
    Free();
}

void wxCustomDataObject::TakeData(size_t size, void *data)
{
    Free();

    m_size = size;
    m_data = data;
}

void *wxCustomDataObject::Alloc(size_t size)
{
    return (void *)new char[size];
}

void wxCustomDataObject::Free()
{
    delete [] (char*)m_data;
    m_size = 0;
    m_data = nullptr;
}

size_t wxCustomDataObject::GetDataSize() const
{
    return GetSize();
}

bool wxCustomDataObject::GetDataHere(void *buf) const
{
    if ( buf == nullptr )
        return false;

    void *data = GetData();
    if ( data == nullptr )
        return false;

    memcpy( buf, data, GetSize() );

    return true;
}

bool wxCustomDataObject::SetData(size_t size, const void *buf)
{
    Free();

    m_data = Alloc(size);
    if ( m_data == nullptr )
        return false;

    m_size = size;
    memcpy( m_data, buf, m_size );

    return true;
}

// ----------------------------------------------------------------------------
// wxImageDataObject
// ----------------------------------------------------------------------------

#if defined(__WXMSW__)
#define wxIMAGE_FORMAT_DATA wxDF_PNG
#define wxIMAGE_FORMAT_BITMAP_TYPE wxBITMAP_TYPE_PNG
#define wxIMAGE_FORMAT_NAME "PNG"
#elif defined(__WXGTK__)
#define wxIMAGE_FORMAT_DATA wxDF_BITMAP
#define wxIMAGE_FORMAT_BITMAP_TYPE wxBITMAP_TYPE_PNG
#define wxIMAGE_FORMAT_NAME "PNG"
#elif defined(__WXOSX__)
#define wxIMAGE_FORMAT_DATA wxDF_BITMAP
#define wxIMAGE_FORMAT_BITMAP_TYPE wxBITMAP_TYPE_TIFF
#define wxIMAGE_FORMAT_NAME "TIFF"
#else
#define wxIMAGE_FORMAT_DATA wxDF_BITMAP
#define wxIMAGE_FORMAT_BITMAP_TYPE wxBITMAP_TYPE_PNG
#define wxIMAGE_FORMAT_NAME "PNG"
#endif

wxImageDataObject::wxImageDataObject(const wxImage& image)
    : wxCustomDataObject(wxIMAGE_FORMAT_DATA)
{
    if ( image.IsOk() )
    {
        SetImage(image);
    }
}

void wxImageDataObject::SetImage(const wxImage& image)
{
    wxCHECK_RET(wxImage::FindHandler(wxIMAGE_FORMAT_BITMAP_TYPE) != nullptr,
        wxIMAGE_FORMAT_NAME " image handler must be installed to use clipboard with image");

    wxMemoryOutputStream mem;
    image.SaveFile(mem, wxIMAGE_FORMAT_BITMAP_TYPE);

    SetData(mem.GetLength(), mem.GetOutputStreamBuffer()->GetBufferStart());
}

wxImage wxImageDataObject::GetImage() const
{
    wxCHECK_MSG(wxImage::FindHandler(wxIMAGE_FORMAT_BITMAP_TYPE) != nullptr, wxNullImage,
        wxIMAGE_FORMAT_NAME " image handler must be installed to use clipboard with image");

    wxMemoryInputStream mem(GetData(), GetSize());
    wxImage image;
    image.LoadFile(mem, wxIMAGE_FORMAT_BITMAP_TYPE);
    return image;
}

// ============================================================================
// some common dnd related code
// ============================================================================

#if wxUSE_DRAG_AND_DROP

#include "wx/dnd.h"

// ----------------------------------------------------------------------------
// wxTextDropTarget
// ----------------------------------------------------------------------------

// NB: we can't use "new" in ctor initializer lists because this provokes an
//     internal compiler error with VC++ 5.0 (hey, even gcc compiles this!),
//     so use SetDataObject() instead

wxTextDropTarget::wxTextDropTarget()
{
    SetDataObject(new wxTextDataObject);
}

wxDragResult wxTextDropTarget::OnData(wxCoord x, wxCoord y, wxDragResult def)
{
    if ( !GetData() )
        return wxDragNone;

    wxTextDataObject *dobj = (wxTextDataObject *)m_dataObject;
    return OnDropText( x, y, dobj->GetText() ) ? def : wxDragNone;
}

// ----------------------------------------------------------------------------
// wxFileDropTarget
// ----------------------------------------------------------------------------

wxFileDropTarget::wxFileDropTarget()
{
    SetDataObject(new wxFileDataObject);
}

wxDragResult wxFileDropTarget::OnData(wxCoord x, wxCoord y, wxDragResult def)
{
    if ( !GetData() )
        return wxDragNone;

    wxFileDataObject *dobj = (wxFileDataObject *)m_dataObject;
    return OnDropFiles( x, y, dobj->GetFilenames() ) ? def : wxDragNone;
}

#endif // wxUSE_DRAG_AND_DROP

#endif // wxUSE_DATAOBJ
