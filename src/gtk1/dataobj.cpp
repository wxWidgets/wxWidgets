///////////////////////////////////////////////////////////////////////////////
// Name:        dataobj.cpp
// Purpose:     wxDataObject class
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dataobj.h"
#endif

#include "wx/dataobj.h"
#include "wx/app.h"
#include "wx/debug.h"

#include "gdk/gdk.h"


//-------------------------------------------------------------------------
// global data
//-------------------------------------------------------------------------

GdkAtom  g_textAtom        = 0;

//-------------------------------------------------------------------------
// wxDataFormat
//-------------------------------------------------------------------------

IMPLEMENT_CLASS(wxDataFormat, wxObject)

wxDataFormat::wxDataFormat()
{
    if (!g_textAtom) g_textAtom = gdk_atom_intern( "STRING", FALSE );
    m_type = wxDF_INVALID;
    m_hasAtom = FALSE;
    m_atom = (GdkAtom) 0;
}

wxDataFormat::wxDataFormat( wxDataType type )
{
    if (!g_textAtom) g_textAtom = gdk_atom_intern( "STRING", FALSE );
    SetType( type );
}

wxDataFormat::wxDataFormat( const wxChar *id )
{
    if (!g_textAtom) g_textAtom = gdk_atom_intern( "STRING", FALSE );
    SetId( id );
}

wxDataFormat::wxDataFormat( const wxString &id )
{
    if (!g_textAtom) g_textAtom = gdk_atom_intern( "STRING", FALSE );
    SetId( id );
}

wxDataFormat::wxDataFormat( wxDataFormat &format )
{
    if (!g_textAtom) g_textAtom = gdk_atom_intern( "STRING", FALSE );
    m_type = format.GetType();
    m_id = format.GetId();
    m_hasAtom = TRUE;
    m_atom = format.GetAtom();
}

wxDataFormat::wxDataFormat( const GdkAtom atom )
{
    if (!g_textAtom) g_textAtom = gdk_atom_intern( "STRING", FALSE );
    m_hasAtom = TRUE;
    
    m_atom = atom;
    
    if (m_atom == g_textAtom)
    {
        m_type = wxDF_TEXT;
    } else
    if (m_atom == GDK_TARGET_BITMAP)
    {
        m_type = wxDF_BITMAP;
    } else
    {
        m_type = wxDF_PRIVATE;
	m_id = gdk_atom_name( m_atom );
	
	if (m_id == _T("file:ALL"))
	{
	    m_type = wxDF_FILENAME;
	}
    }
}

void wxDataFormat::SetType( wxDataType type )
{
    m_type = type;
    
    if (m_type == wxDF_TEXT)
    {
        m_id = _T("STRING");
    } 
    else
    if (m_type == wxDF_BITMAP)
    {
        m_id = _T("BITMAP");
    } 
    else
    if (m_type == wxDF_FILENAME)
    {
        m_id = _T("file:ALL");
    }
    else
    {
       wxFAIL_MSG( _T("invalid dataformat") );
    }
    
    m_hasAtom = FALSE;
}
  
wxDataType wxDataFormat::GetType() const
{
    return m_type;
}

wxString wxDataFormat::GetId() const
{
    return m_id;
}

void wxDataFormat::SetId( const wxChar *id )
{
    m_type = wxDF_PRIVATE;
    m_id = id;
    m_hasAtom = FALSE;
}

GdkAtom wxDataFormat::GetAtom()
{
    if (!m_hasAtom)
    {
        m_hasAtom = TRUE;
	
	if (m_type == wxDF_TEXT)
	{
            m_atom = g_textAtom;
        }
	else
        if (m_type == wxDF_BITMAP)
        {
            m_atom = GDK_TARGET_BITMAP;
        } 
	else
        if (m_type == wxDF_PRIVATE)
        {
            m_atom = gdk_atom_intern( MBSTRINGCAST m_id.mbc_str(), FALSE );
        } 
	else
	if (m_type == wxDF_FILENAME)
	{
	    m_atom = gdk_atom_intern( "file:ALL", FALSE );
	} 
	else
	{
	    m_hasAtom = FALSE;
	    m_atom = (GdkAtom) 0;
	}
    }
    
    return m_atom;
}

//-------------------------------------------------------------------------
// wxDataBroker
//-------------------------------------------------------------------------

IMPLEMENT_CLASS(wxDataBroker,wxObject)

wxDataBroker::wxDataBroker() 
{ 
    m_dataObjects.DeleteContents(TRUE);
    m_preferred = 0;
}

void wxDataBroker::Add( wxDataObject *dataObject, bool preferred )
{ 
    if (preferred) m_preferred = m_dataObjects.GetCount();
    m_dataObjects.Append( dataObject );
}  
  
size_t wxDataBroker::GetFormatCount() const
{ 
    return m_dataObjects.GetCount(); 
}
    
wxDataFormat &wxDataBroker::GetPreferredFormat() const
{ 
    wxNode *node = m_dataObjects.Nth( m_preferred );
    
    wxASSERT( node );
    
    wxDataObject* data_obj = (wxDataObject*)node->Data();
    
    return data_obj->GetFormat();
}
    
wxDataFormat &wxDataBroker::GetNthFormat( size_t nth ) const
{ 
    wxNode *node = m_dataObjects.Nth( nth );

    wxASSERT( node );
    
    wxDataObject* data_obj = (wxDataObject*)node->Data();
    
    return data_obj->GetFormat();
}
    
bool wxDataBroker::IsSupportedFormat( wxDataFormat &format ) const
{ 
    wxNode *node = m_dataObjects.First();
    while (node)
    {
        wxDataObject *dobj = (wxDataObject*)node->Data();
	
	if (dobj->GetFormat().GetAtom() == format.GetAtom())
	{
	    return TRUE;
	}
    
        node = node->Next();
    }
    
    return FALSE;
}
  
size_t wxDataBroker::GetSize( wxDataFormat& format ) const
{
    wxNode *node = m_dataObjects.First();
    while (node)
    {
        wxDataObject *dobj = (wxDataObject*)node->Data();
	
	if (dobj->GetFormat().GetAtom() == format.GetAtom())
	{
	    return dobj->GetSize();
	}
    
        node = node->Next();
    }
    
    return 0;
}
    
void wxDataBroker::WriteData( wxDataFormat& format, void *dest ) const
{
    wxNode *node = m_dataObjects.First();
    while (node)
    {
        wxDataObject *dobj = (wxDataObject*)node->Data();
	
	if (dobj->GetFormat().GetAtom() == format.GetAtom())
	{
	    dobj->WriteData( dest );
	}
    
        node = node->Next();
    }
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

wxDataFormat &wxDataObject::GetFormat()
{
    return m_format;
}

wxDataType wxDataObject::GetFormatType() const
{
    return m_format.GetType();
}

wxString wxDataObject::GetFormatId() const
{
    return m_format.GetId();
}

GdkAtom wxDataObject::GetFormatAtom() const
{
    GdkAtom ret = ((wxDataObject*) this)->m_format.GetAtom();
    return ret;
}  

// ----------------------------------------------------------------------------
// wxTextDataObject
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( wxTextDataObject, wxDataObject )

wxTextDataObject::wxTextDataObject()
{
    m_format.SetType( wxDF_TEXT );
}

wxTextDataObject::wxTextDataObject( const wxString& data )
{
    m_format.SetType( wxDF_TEXT );
    
    m_data = data;
}

void wxTextDataObject::SetText( const wxString& data ) 
{
    m_data = data;
}

wxString wxTextDataObject::GetText() const
{
    return m_data;
}

void wxTextDataObject::WriteData( void *dest ) const
{
    WriteString( m_data, dest );
}

size_t wxTextDataObject::GetSize() const
{
    return m_data.Len() + 1;
}

void wxTextDataObject::WriteString( const wxString &str, void *dest ) const
{
    memcpy( dest, str.mb_str(), str.Len()+1 );
}
    
// ----------------------------------------------------------------------------
// wxFileDataObject
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( wxFileDataObject, wxDataObject )

wxFileDataObject::wxFileDataObject()
{
    m_format.SetType( wxDF_FILENAME );
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
    
void wxFileDataObject::WriteData( void *dest ) const
{
    memcpy( dest, m_files.mbc_str(), GetSize() );
}
 
size_t wxFileDataObject::GetSize() const
{
    return m_files.Len() + 1;
}
  
// ----------------------------------------------------------------------------
// wxBitmapDataObject
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( wxBitmapDataObject, wxDataObject )

wxBitmapDataObject::wxBitmapDataObject()
{
    m_format.SetType( wxDF_BITMAP );
}

wxBitmapDataObject::wxBitmapDataObject( const wxBitmap& bitmap )
{
    m_format.SetType( wxDF_BITMAP );
    
    m_bitmap = bitmap;
}

void wxBitmapDataObject::SetBitmap( const wxBitmap &bitmap )
{
    m_bitmap = bitmap;
}

wxBitmap wxBitmapDataObject::GetBitmap() const
{
    return m_bitmap;
}

void wxBitmapDataObject::WriteData( void *dest ) const
{
    WriteBitmap( m_bitmap, dest );
}

size_t wxBitmapDataObject::GetSize() const
{
    return 0;
}

void wxBitmapDataObject::WriteBitmap( const wxBitmap &bitmap, void *dest ) const
{
    memcpy( dest, m_bitmap.GetPixmap(), GetSize() );
}
    
// ----------------------------------------------------------------------------
// wxPrivateDataObject
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( wxPrivateDataObject, wxDataObject )

wxPrivateDataObject::wxPrivateDataObject() 
{ 
    m_id = _T("application/");
    m_id += wxTheApp->GetAppName();
    
    m_format.SetId( m_id );
    
    m_size = 0; 
    m_data = (char*) NULL; 
}
    
wxPrivateDataObject::~wxPrivateDataObject()
{ 
    if (m_data) delete[] m_data; 
}
  
void wxPrivateDataObject::SetId( const wxString& id )
{ 
    m_id = id;
    m_format.SetId( m_id );
}
    
wxString wxPrivateDataObject::GetId() const
{ 
    return m_id; 
}

void wxPrivateDataObject::SetData( const char *data, size_t size )
{
    m_size = size;
    
    if (m_data) delete[] m_data;
    
    m_data = new char[size];

    memcpy( m_data, data, size );  
}

char* wxPrivateDataObject::GetData() const
{
    return m_data;
}

void wxPrivateDataObject::WriteData( void *dest ) const
{
    WriteData( m_data, dest );
}

size_t wxPrivateDataObject::GetSize() const
{
   return m_size;
}

void wxPrivateDataObject::WriteData( const char *data, void *dest ) const
{
    memcpy( dest, data, GetSize() );
}

