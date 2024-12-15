///////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/dataobj.cpp
// Purpose:     wxDataObject class
// Author:      Robert Roebling
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_DATAOBJ

#include "wx/dataobj.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/app.h"
    #include "wx/image.h"
#endif

#include "wx/mstream.h"
#include "wx/scopedarray.h"
#include "wx/uri.h"

#include "wx/gtk/private.h"

//-------------------------------------------------------------------------
// module data
//-------------------------------------------------------------------------

namespace
{

// Atom initialized on first access.
//
// Note that this is more than just an optimization, as we have to delay
// calling gdk_atom_intern() until after GDK initialization.
class wxGdkAtom
{
public:
    // Name is literal, so we don't copy it but just store the pointer.
    wxGdkAtom(const char* name) : m_name{name} {}

    wxGdkAtom(const wxGdkAtom&) = delete;
    wxGdkAtom& operator=(const wxGdkAtom&) = delete;

    GdkAtom Get() const
    {
        if ( !m_atom )
            m_atom = gdk_atom_intern(m_name, FALSE);

        return m_atom;
    }

private:
    const char* const m_name;
    mutable GdkAtom m_atom = nullptr;
};

inline bool operator==(wxDataFormat format, const wxGdkAtom& wxatom)
{
    return format.GetFormatId() == wxatom.Get();
}

inline bool operator==(GdkAtom atom, const wxGdkAtom& wxatom)
{
    return atom == wxatom.Get();
}

// Text is special as it can be represented by several different atoms: we
// accept all of them when pasting and provide all of them when copying.
//
// Note that there are also other atoms for text, e.g. "COMPOUND_TEXT" and
// "TEXT", but it doesn't seem necessary to support them, so we don't.
wxGdkAtom g_u8strAtom   {"UTF8_STRING"};
wxGdkAtom g_strAtom     {"STRING"};
wxGdkAtom g_u8textAtom  {"text/plain;charset=utf-8"};
wxGdkAtom g_textAtom    {"text/plain"};

wxGdkAtom g_pngAtom     {"image/png"};
wxGdkAtom g_fileAtom    {"text/uri-list"};
wxGdkAtom g_htmlAtom    {"text/html"};

} // anonymous namespace

//-------------------------------------------------------------------------
// wxDataFormat
//-------------------------------------------------------------------------

wxDataFormat::wxDataFormat()
{
    m_type = wxDF_INVALID;
    m_format = (GdkAtom) nullptr;
}

wxDataFormat::wxDataFormat( wxDataFormatId type )
{
    SetType( type );
}

void wxDataFormat::InitFromString( const wxString &id )
{
    SetId( id );
}

wxDataFormat::wxDataFormat( NativeFormat format )
{
    SetId( format );
}

void wxDataFormat::SetType( wxDataFormatId type )
{
    m_type = type;

    if (m_type == wxDF_UNICODETEXT)
        m_format = g_u8strAtom.Get();
    else if (m_type == wxDF_TEXT)
        m_format = g_strAtom.Get();
    else
    if (m_type == wxDF_BITMAP)
        m_format = g_pngAtom.Get();
    else
    if (m_type == wxDF_FILENAME)
        m_format = g_fileAtom.Get();
    else
    if (m_type == wxDF_HTML)
        m_format = g_htmlAtom.Get();
    else
    {
       wxFAIL_MSG( wxT("invalid dataformat") );
    }
}

wxDataFormatId wxDataFormat::GetType() const
{
    return m_type;
}

wxString wxDataFormat::GetId() const
{
    wxGtkString atom_name(gdk_atom_name(m_format));
    return wxString::FromAscii(atom_name);
}

void wxDataFormat::SetId( NativeFormat format )
{
    m_format = format;

    if (m_format == g_u8strAtom || m_format == g_u8textAtom)
        m_type = wxDF_UNICODETEXT;
    else
    if (m_format == g_strAtom || m_format == g_textAtom)
        m_type = wxDF_TEXT;
    else
    if (m_format == g_pngAtom)
        m_type = wxDF_BITMAP;
    else
    if (m_format == g_fileAtom)
        m_type = wxDF_FILENAME;
    else
    if (m_format == g_htmlAtom)
        m_type = wxDF_HTML;
    else
        m_type = wxDF_PRIVATE;
}

void wxDataFormat::SetId( const wxString& id )
{
    m_type = wxDF_PRIVATE;
    m_format = gdk_atom_intern( id.ToAscii(), FALSE );
}


// This is also used by wxClipboard to check if the given atom refer to the
// same format, so make it extern.
extern bool wxGTKIsSameFormat(GdkAtom atom1, GdkAtom atom2)
{
    if (atom1 == atom2)
        return true;

    if (atom1 == g_u8strAtom && atom2 == g_u8textAtom)
        return true;

    if (atom1 == g_strAtom && atom2 == g_textAtom)
        return true;

    return false;
}

// Also used by wxClipboard to use the alternative Wayland atoms for the text.
extern GdkAtom wxGTKGetAltWaylandFormat(GdkAtom atom)
{
    if (atom == g_u8strAtom)
        return g_u8textAtom.Get();
    if (atom == g_strAtom)
        return g_textAtom.Get();

    return nullptr;
}

bool wxDataFormat::operator==(const wxDataFormat& other) const
{
    if ( m_type != other.m_type )
        return false;

    return wxGTKIsSameFormat(m_format, other.m_format);
}


//-------------------------------------------------------------------------
// wxDataObject
//-------------------------------------------------------------------------

wxDataObject::wxDataObject()
{
}

wxDataObject::~wxDataObject()
{
    // dtor is empty but needed for Darwin and AIX -- otherwise it doesn't link
}

bool wxDataObject::IsSupportedFormat(const wxDataFormat& format, Direction dir) const
{
    size_t nFormatCount = GetFormatCount(dir);
    if ( nFormatCount == 1 )
    {
        return format == GetPreferredFormat();
    }
    else
    {
        wxScopedArray<wxDataFormat> formats(nFormatCount);
        GetAllFormats(formats.get(), dir);

        for ( size_t n = 0; n < nFormatCount; n++ )
        {
            if ( wxGTKIsSameFormat(formats[n], format) )
                return true;
        }

        return false;
    }
}

// ----------------------------------------------------------------------------
// wxFileDataObject
// ----------------------------------------------------------------------------

bool wxFileDataObject::GetDataHere(void *buf) const
{
    char* out = static_cast<char*>(buf);

    for (size_t i = 0; i < m_filenames.GetCount(); i++)
    {
        char* uri = g_filename_to_uri(m_filenames[i].mbc_str(), nullptr, nullptr);
        if (uri)
        {
            size_t const len = strlen(uri);
            memcpy(out, uri, len);
            out += len;
            *(out++) = '\r';
            *(out++) = '\n';
            g_free(uri);
        }
    }
    *out = 0;

    return true;
}

size_t wxFileDataObject::GetDataSize() const
{
    size_t res = 0;

    for (size_t i = 0; i < m_filenames.GetCount(); i++)
    {
        char* uri = g_filename_to_uri(m_filenames[i].mbc_str(), nullptr, nullptr);
        if (uri) {
            res += strlen(uri) + 2; // Including "\r\n"
            g_free(uri);
        }
    }

    return res + 1;
}

bool wxFileDataObject::SetData(size_t WXUNUSED(size), const void *buf)
{
    // we get data in the text/uri-list format, i.e. as a sequence of URIs
    // (filenames prefixed by "file:") delimited by "\r\n". size includes
    // the trailing zero (in theory, not for Nautilus in early GNOME
    // versions).

    m_filenames.Empty();

    const gchar *nexttemp = (const gchar*) buf;
    for ( ; ; )
    {
        int len = 0;
        const gchar *temp = nexttemp;
        for (;;)
        {
            if (temp[len] == 0)
            {
                if (len > 0)
                {
                    // if an app omits '\r''\n'
                    nexttemp = temp+len;
                    break;
                }

                return true;
            }
            if (temp[len] == '\r')
            {
                if (temp[len+1] == '\n')
                    nexttemp = temp+len+2;
                else
                    nexttemp = temp+len+1;
                break;
            }
            len++;
        }

        if (len == 0)
            break;

        // required to give it a trailing zero
        gchar *uri = g_strndup( temp, len );

        gchar *fn = g_filename_from_uri( uri, nullptr, nullptr );

        g_free( uri );

        if (fn)
        {
            AddFile( wxConvFileName->cMB2WX( fn ) );
            g_free( fn );
        }
    }

    return true;
}

void wxFileDataObject::AddFile( const wxString &filename )
{
   m_filenames.Add( filename );
}

// ----------------------------------------------------------------------------
// wxBitmapDataObject
// ----------------------------------------------------------------------------

wxBitmapDataObject::wxBitmapDataObject()
{
    Init();
}

wxBitmapDataObject::wxBitmapDataObject( const wxBitmap& bitmap )
                  : wxBitmapDataObjectBase(bitmap)
{
    Init();

    DoConvertToPng();
}

wxBitmapDataObject::~wxBitmapDataObject()
{
    Clear();
}

void wxBitmapDataObject::SetBitmap( const wxBitmap &bitmap )
{
    ClearAll();

    wxBitmapDataObjectBase::SetBitmap(bitmap);

    DoConvertToPng();
}

bool wxBitmapDataObject::GetDataHere(void *buf) const
{
    if ( !m_pngSize )
    {
        wxFAIL_MSG( wxT("attempt to copy empty bitmap failed") );

        return false;
    }

    memcpy(buf, m_pngData, m_pngSize);

    return true;
}

bool wxBitmapDataObject::SetData(size_t size, const void *buf)
{
    Clear();

    wxCHECK_MSG( wxImage::FindHandler(wxBITMAP_TYPE_PNG) != nullptr,
                 false, wxT("You must call wxImage::AddHandler(new wxPNGHandler); to be able to use clipboard with bitmaps!") );

    m_pngSize = size;
    m_pngData = malloc(m_pngSize);

    memcpy(m_pngData, buf, m_pngSize);

    wxMemoryInputStream mstream((char*) m_pngData, m_pngSize);
    wxImage image;
    if ( !image.LoadFile( mstream, wxBITMAP_TYPE_PNG ) )
    {
        return false;
    }

    m_bitmap = wxBitmap(image);

    return m_bitmap.IsOk();
}

void wxBitmapDataObject::DoConvertToPng()
{
    if ( !m_bitmap.IsOk() )
        return;

    wxCHECK_RET( wxImage::FindHandler(wxBITMAP_TYPE_PNG) != nullptr,
                 wxT("You must call wxImage::AddHandler(new wxPNGHandler); to be able to use clipboard with bitmaps!") );

    wxImage image = m_bitmap.ConvertToImage();

    wxCountingOutputStream count;
    image.SaveFile(count, wxBITMAP_TYPE_PNG);

    m_pngSize = count.GetSize() + 100; // sometimes the size seems to vary ???
    m_pngData = malloc(m_pngSize);

    wxMemoryOutputStream mstream((char*) m_pngData, m_pngSize);
    image.SaveFile(mstream, wxBITMAP_TYPE_PNG);
}

// ----------------------------------------------------------------------------
// wxURLDataObject
// ----------------------------------------------------------------------------

class wxTextURIListDataObject : public wxDataObjectSimple
{
public:
    wxTextURIListDataObject(const wxString& url)
        : wxDataObjectSimple(wxDataFormat(g_fileAtom.Get())),
          m_url(url)
    {
    }

    const wxString& GetURL() const { return m_url; }
    void SetURL(const wxString& url) { m_url = url; }


    virtual size_t GetDataSize() const override
    {
        // It is not totally clear whether we should include "\r\n" at the end
        // of the string if there is only one URL or not, but not doing it
        // doesn't seem to create any problems, so keep things simple.
        return strlen(m_url.utf8_str()) + 1;
    }

    virtual bool GetDataHere(void *buf) const override
    {
        char* const dst = static_cast<char*>(buf);

        strcpy(dst, m_url.utf8_str());

        return true;
    }

    virtual bool SetData(size_t len, const void *buf) override
    {
        const char* const src = static_cast<const char*>(buf);

        // Length here includes the trailing NUL, but we don't want to include
        // it into the string contents.
        wxCHECK_MSG( len != 0 && !src[len], false, "must have trailing NUL" );

        len--;

        // The string might be "\r\n"-terminated but this is not necessarily
        // the case (e.g. when dragging an URL from Firefox, it isn't).
        if ( len > 1 && src[len - 1] == '\n' )
        {
            if ( len > 2 && src[len - 2] == '\r' )
                len--;

            len--;
        }

        m_url = wxString::FromUTF8(src, len);

        return true;
    }

    // Must provide overloads to avoid hiding them (and warnings about it)
    virtual size_t GetDataSize(const wxDataFormat&) const override
    {
        return GetDataSize();
    }
    virtual bool GetDataHere(const wxDataFormat&, void *buf) const override
    {
        return GetDataHere(buf);
    }
    virtual bool SetData(const wxDataFormat&, size_t len, const void *buf) override
    {
        return SetData(len, buf);
    }

private:
    wxString m_url;
};

wxURLDataObject::wxURLDataObject(const wxString& url) :
    m_dobjURIList(new wxTextURIListDataObject(url)),
    m_dobjText(new wxTextDataObject(url))
{
    // Use both URL-specific format and a plain text one to ensure that URLs
    // can be pasted into any application.
    Add(m_dobjURIList, true /* preferred */);
    Add(m_dobjText);
}

void wxURLDataObject::SetURL(const wxString& url)
{
    m_dobjURIList->SetURL(url);
    m_dobjText->SetText(url);
}

wxString wxURLDataObject::GetURL() const
{
    if ( GetReceivedFormat() == g_fileAtom )
    {
        // If we received the URL as an URI, use it.
        return m_dobjURIList->GetURL();
    }
    else // Otherwise we either got it as text or didn't get anything yet.
    {
        // In either case using the text format should be fine.
        return m_dobjText->GetText();
    }
}

#endif // wxUSE_DATAOBJ
