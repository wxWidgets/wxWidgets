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
    PrepareFormats();
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

IMPLEMENT_ABSTRACT_CLASS( wxDataObject, wxObject )

wxDataObject::wxDataObject()
{
}

wxDataObject::~wxDataObject()
{
}

bool wxDataObject::IsSupportedFormat(const wxDataFormat& format) const
{
    size_t nFormatCount = GetFormatCount();
    if ( nFormatCount == 1 ) {
        return format == GetPreferredFormat();
    }
    else {
        wxDataFormat *formats = new wxDataFormat[nFormatCount];
        GetAllFormats(formats);

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
// wxTextDataObject
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( wxTextDataObject, wxDataObject )

wxTextDataObject::wxTextDataObject() 
{ 
}

wxTextDataObject::wxTextDataObject(const wxString& strText) 
    : m_strText(strText) 
{ 
}

size_t wxTextDataObject::GetDataSize(const wxDataFormat& format) const
{ 
    return m_strText.Len() + 1;   // +1 for trailing '\0'of course
}

bool wxTextDataObject::GetDataHere(const wxDataFormat& format, void *buf) const
{ 
    memcpy(buf, m_strText.c_str(), GetDataSize(format)); 
    return TRUE; 
}

bool wxTextDataObject::SetData(const wxDataFormat& format, const void *buf)
{ 
    m_strText = (const wxChar *)buf; 
    return TRUE;
}

// ----------------------------------------------------------------------------
// wxFileDataObject
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( wxFileDataObject, wxDataObject )

wxFileDataObject::wxFileDataObject()
{
}

void wxFileDataObject::AddFile( const wxString &file )
{
    m_files += file;
    m_files += (wxChar)0;
}

wxString wxFileDataObject::GetFiles() const
{
    return m_files;
}

bool wxFileDataObject::GetDataHere(const wxDataFormat& format, void *buf) const
{
    if (format == wxDF_FILENAME)
    {
        memcpy( buf, m_files.mbc_str(), m_files.Len() + 1 );
	return TRUE;
    }
    
    return FALSE;
}

size_t wxFileDataObject::GetDataSize(const wxDataFormat& format) const
{
    if (format != wxDF_FILENAME) return 0;
    
    return m_files.Len() + 1;
}

bool wxFileDataObject::SetData(const wxDataFormat& format, const void *buf)
{
    if (format != wxDF_FILENAME)
        return FALSE;
	
    m_files = (char*)(buf);  // this is so ugly, I cannot look at it
    
    return TRUE;
}

// ----------------------------------------------------------------------------
// wxBitmapDataObject
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( wxBitmapDataObject, wxDataObject )

wxBitmapDataObject::wxBitmapDataObject()
{
    m_pngData = (void*)NULL;
    m_pngSize = 0;
}

wxBitmapDataObject::wxBitmapDataObject( const wxBitmap& bitmap )
{
    m_pngData = (void*)NULL;
    m_pngSize = 0;
    m_bitmap = bitmap;
    DoConvertToPng();
}

wxBitmapDataObject::~wxBitmapDataObject()
{
    if (m_pngData) 
        delete[] m_pngData;
}

void wxBitmapDataObject::SetBitmap( const wxBitmap &bitmap )
{
    if (m_pngData) 
        delete[] m_pngData;
    m_pngData = (void*)NULL;
    m_pngSize = 0;
	
    m_bitmap = bitmap;
    DoConvertToPng();
}

size_t wxBitmapDataObject::GetDataSize(const wxDataFormat& format) const
{
    if (format != wxDF_BITMAP) return 0;
    
    return m_pngSize;
}

bool wxBitmapDataObject::GetDataHere(const wxDataFormat& format, void *buf) const
{
    if (format != wxDF_BITMAP) return FALSE;
    
    if (m_pngSize > 0)
    {
        memcpy(buf, m_pngData, m_pngSize); 
        return TRUE;
    }
    
    return FALSE;
}

bool wxBitmapDataObject::SetData(const wxDataFormat& format, const void *buf)
{
    if (m_pngData) delete[] m_pngData;
    m_pngData = (void*) NULL;
    m_pngSize = 0;
    m_bitmap = wxNullBitmap;
    
    return FALSE;
}

void wxBitmapDataObject::SetPngData(const void *buf, size_t size)
{
    if (m_pngData) delete[] m_pngData;
    m_pngSize = size;
    m_pngData = (void*) new char[m_pngSize];
    
    memcpy( m_pngData, buf, m_pngSize );
    
    wxMemoryInputStream mstream( (char*) m_pngData, m_pngSize );
    wxImage image;
    wxPNGHandler handler;
    handler.LoadFile( &image, mstream );
    m_bitmap = image.ConvertToBitmap();
}

void wxBitmapDataObject::DoConvertToPng()
{
    if (!m_bitmap.Ok()) return;
    
    wxImage image( m_bitmap );
    wxPNGHandler handler;
    
    wxCountingOutputStream count;
    handler.SaveFile( &image, count );
    m_pngSize = count.GetSize() + 100; // sometimes the size seems to vary ???
    m_pngData = (void*) new char[m_pngSize];
    
    wxMemoryOutputStream mstream( (char*) m_pngData, m_pngSize );
    handler.SaveFile( &image, mstream );
}

// ----------------------------------------------------------------------------
// wxPrivateDataObject
// ----------------------------------------------------------------------------

IMPLEMENT_CLASS( wxPrivateDataObject, wxDataObject )

void wxPrivateDataObject::Free()
{
    if ( m_data )
        free(m_data);
}

wxPrivateDataObject::wxPrivateDataObject()
{
    wxString id = wxT("application/");
    id += wxTheApp->GetAppName();

    m_format.SetId( id );

    m_size = 0;
    m_data = (void *)NULL;
}

void wxPrivateDataObject::SetData( const void *data, size_t size )
{
    Free();

    m_size = size;
    m_data = malloc(size);

    memcpy( m_data, data, size );
}

void wxPrivateDataObject::WriteData( void *dest ) const
{
    WriteData( m_data, dest );
}

size_t wxPrivateDataObject::GetSize() const
{
    return m_size;
}

void wxPrivateDataObject::WriteData( const void *data, void *dest ) const
{
    memcpy( dest, data, GetSize() );
}

