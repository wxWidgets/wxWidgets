///////////////////////////////////////////////////////////////////////////////
// Name:        dataobj.cpp
// Purpose:     wxDataObject class
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "dataobj.h"
#endif

#include "wx/dataobj.h"
#include "wx/app.h"
#include "wx/debug.h"
#include "wx/mstream.h"
#include "wx/image.h"

#include "gdk/gdk.h"

//-------------------------------------------------------------------------
// global data
//-------------------------------------------------------------------------

GdkAtom  g_textAtom        = 0;
GdkAtom  g_pngAtom         = 0;
GdkAtom  g_fileAtom        = 0;

//-------------------------------------------------------------------------
// wxDataFormat
//-------------------------------------------------------------------------

wxDataFormat::wxDataFormat()
{
    // do *not* call PrepareFormats() from here for 2 reasons:
    //
    // 1. we will have time to do it later because some other Set function
    //    must be called before we really need them
    //
    // 2. doing so prevents us from declaring global wxDataFormats because
    //    calling PrepareFormats (and thus gdk_atom_intern) before GDK is
    //    initialised will result in a crash
    m_type = wxDF_INVALID;
    m_format = (GdkAtom) 0;
}

wxDataFormat::wxDataFormat( wxDataFormatId type )
{
    PrepareFormats();
    SetType( type );
}

wxDataFormat::wxDataFormat( const wxChar *id )
{
    PrepareFormats();
    SetId( id );
}

wxDataFormat::wxDataFormat( const wxString &id )
{
    PrepareFormats();
    SetId( id );
}

wxDataFormat::wxDataFormat( NativeFormat format )
{
    PrepareFormats();
    SetId( format );
}

void wxDataFormat::SetType( wxDataFormatId type )
{
    PrepareFormats();
    m_type = type;

    if (m_type == wxDF_TEXT)
        m_format = g_textAtom;
    else
    if (m_type == wxDF_BITMAP)
        m_format = g_pngAtom;
    else
    if (m_type == wxDF_FILENAME)
        m_format = g_fileAtom;
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
    wxString ret( gdk_atom_name( m_format ) );  // this will convert from ascii to Unicode
    return ret;
}

void wxDataFormat::SetId( NativeFormat format )
{
    PrepareFormats();
    m_format = format;

    if (m_format == g_textAtom)
        m_type = wxDF_TEXT;
    else
    if (m_format == g_pngAtom)
        m_type = wxDF_BITMAP;
    else
    if (m_format == g_fileAtom)
        m_type = wxDF_FILENAME;
    else
        m_type = wxDF_PRIVATE;
}

void wxDataFormat::SetId( const wxChar *id )
{
    PrepareFormats();
    m_type = wxDF_PRIVATE;
    wxString tmp( id );
    m_format = gdk_atom_intern( wxMBSTRINGCAST tmp.mbc_str(), FALSE );  // what is the string cast for?
}

void wxDataFormat::PrepareFormats()
{
    // VZ: GNOME included in RedHat 6.1 uses the MIME types below and not the
    //     atoms STRING and file:ALL as the old code was, but normal X apps
    //     use STRING for text selection when transfering the data via
    //     clipboard, for example, so do use STRING for now (GNOME apps will
    //     probably support STRING as well for compatibility anyhow), but use
    //     text/uri-list for file dnd because compatibility is not important
    //     here (with whom?)
    if (!g_textAtom)
        g_textAtom = gdk_atom_intern( "STRING" /* "text/plain" */, FALSE );
    if (!g_pngAtom)
        g_pngAtom = gdk_atom_intern( "image/png", FALSE );
    if (!g_fileAtom)
        g_fileAtom = gdk_atom_intern( "text/uri-list", FALSE );
}

//-------------------------------------------------------------------------
// wxDataObject
//-------------------------------------------------------------------------

wxDataObject::wxDataObject()
{
}

bool wxDataObject::IsSupportedFormat(const wxDataFormat& format, Direction dir) const
{
    size_t nFormatCount = GetFormatCount(dir);
    if ( nFormatCount == 1 ) {
        return format == GetPreferredFormat();
    }
    else {
        wxDataFormat *formats = new wxDataFormat[nFormatCount];
        GetAllFormats(formats,dir);

        size_t n;
        for ( n = 0; n < nFormatCount; n++ ) {
            if ( formats[n] == format )
                break;
        }

        delete [] formats;

        // found?
        return n < nFormatCount;
    }
}

// ----------------------------------------------------------------------------
// wxFileDataObject
// ----------------------------------------------------------------------------

bool wxFileDataObject::GetDataHere(void *buf) const
{
    wxString filenames;

    for (size_t i = 0; i < m_filenames.GetCount(); i++)
    {
        filenames += m_filenames[i];
        filenames += (wxChar) 0;
    }

    memcpy( buf, filenames.mbc_str(), filenames.Len() + 1 );

    return TRUE;
}

size_t wxFileDataObject::GetDataSize() const
{
    size_t res = 0;

    for (size_t i = 0; i < m_filenames.GetCount(); i++)
    {
        res += m_filenames[i].Len();
        res += 1;
    }

    return res + 1;
}

bool wxFileDataObject::SetData(size_t size, const void *buf)
{
    // VZ: old format
#if 0
    // filenames are stores as a string with #0 as deliminators
    const char *filenames = (const char*) buf;
    size_t pos = 0;
    for(;;)
    {
        if (filenames[0] == 0)
            break;
        if (pos >= size)
            break;
        wxString file( filenames );  // this returns the first file
        AddFile( file );
        pos += file.Len()+1;
        filenames += file.Len()+1;
    }
#else // 1
    m_filenames.Empty();

    // the text/uri-list format is a sequence of URIs (filenames prefixed by
    // "file:" as far as I see) delimited by "\r\n" of total length size
    // (I wonder what happens if the file has '\n' in its filename??)
    wxString filename;
    for ( const char *p = (const char *)buf; ; p++ )
    {
        // some broken programs (testdnd GTK+ sample!) omit the trailing
        // "\r\n", so check for '\0' explicitly here instead of doing it in
        // the loop statement to account for it
        if ( (*p == '\r' && *(p+1) == '\n') || !*p )
        {
            size_t lenPrefix = 5; // strlen("file:")
            if ( filename.Left(lenPrefix).MakeLower() == _T("file:") )
            {
                // sometimes the syntax is "file:filename", sometimes it's
                // URL-like: "file://filename" - deal with both
                if ( filename[lenPrefix] == _T('/') &&
                     filename[lenPrefix + 1] == _T('/') )
                {
                    // skip the slashes
                    lenPrefix += 2;
                }

                AddFile(filename.c_str() + lenPrefix);
                filename.Empty();
            }
            else
            {
                wxLogDebug(_T("Unsupported URI '%s' in wxFileDataObject"),
                           filename.c_str());
            }

            if ( !*p )
                break;

            // skip '\r'
            p++;
        }
        else
        {
            filename += *p;
        }
    }
#endif // 0/1

    return TRUE;
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

        return FALSE;
    }

    memcpy(buf, m_pngData, m_pngSize);

    return TRUE;
}

bool wxBitmapDataObject::SetData(size_t size, const void *buf)
{
    Clear();

    m_pngSize = size;
    m_pngData = malloc(m_pngSize);

    memcpy( m_pngData, buf, m_pngSize );

    wxMemoryInputStream mstream( (char*) m_pngData, m_pngSize );
    wxImage image;
    wxPNGHandler handler;
    if ( !handler.LoadFile( &image, mstream ) )
    {
        return FALSE;
    }

    m_bitmap = image.ConvertToBitmap();

    return m_bitmap.Ok();
}

void wxBitmapDataObject::DoConvertToPng()
{
    if (!m_bitmap.Ok())
        return;

    wxImage image( m_bitmap );
    wxPNGHandler handler;

    wxCountingOutputStream count;
    handler.SaveFile( &image, count );

    m_pngSize = count.GetSize() + 100; // sometimes the size seems to vary ???
    m_pngData = malloc(m_pngSize);

    wxMemoryOutputStream mstream( (char*) m_pngData, m_pngSize );
    handler.SaveFile( &image, mstream );
}


