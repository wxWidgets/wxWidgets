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
    if (!g_textAtom)
        g_textAtom = gdk_atom_intern( "STRING", FALSE );
    if (!g_pngAtom)
        g_pngAtom = gdk_atom_intern( "image/png", FALSE );
    if (!g_fileAtom)
        g_fileAtom = gdk_atom_intern( "file:ALL", FALSE );
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


