///////////////////////////////////////////////////////////////////////////////
// Name:        dataobj.cpp
// Purpose:     wxDataObject class
// Author:      Julian Smart
// Id:          $Id$
// Copyright:   (c) 1998 Julian Smart
// Licence:   	wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dataobj.h"
#endif

#include "wx/dataobj.h"
#include "wx/app.h"

#include <Xm/Xm.h>
#include "wx/utils.h"

//-------------------------------------------------------------------------
// global data
//-------------------------------------------------------------------------

Atom  g_textAtom        = 0;

//-------------------------------------------------------------------------
// wxDataFormat
//-------------------------------------------------------------------------

IMPLEMENT_CLASS(wxDataFormat, wxObject)

wxDataFormat::wxDataFormat()
{
    if (!g_textAtom) g_textAtom = XInternAtom( (Display*) wxGetDisplay(), "STRING", FALSE );
    m_type = wxDF_INVALID;
    m_hasAtom = FALSE;
    m_atom = (Atom) 0;
}

wxDataFormat::wxDataFormat( wxDataFormatId type )
{
    if (!g_textAtom) g_textAtom = XInternAtom( (Display*) wxGetDisplay(), "STRING", FALSE );
    SetType( type );
}

wxDataFormat::wxDataFormat( const wxChar *id )
{
    if (!g_textAtom) g_textAtom = XInternAtom( (Display*) wxGetDisplay(), "STRING", FALSE );
    SetId( id );
}

wxDataFormat::wxDataFormat( const wxString &id )
{
    if (!g_textAtom) g_textAtom = XInternAtom( (Display*) wxGetDisplay(), "STRING", FALSE );
    SetId( id );
}

wxDataFormat::wxDataFormat( const wxDataFormat &format )
{
    if (!g_textAtom) g_textAtom = XInternAtom( (Display*) wxGetDisplay(), "STRING", FALSE );
    m_type = format.GetType();
    m_id = format.GetId();
    m_hasAtom = TRUE;
    m_atom = ((wxDataFormat &)format).GetAtom();    // const_cast
}

wxDataFormat::wxDataFormat( const Atom atom )
{
    if (!g_textAtom) g_textAtom = XInternAtom( (Display*) wxGetDisplay(), "STRING", FALSE );
    m_hasAtom = TRUE;

    m_atom = atom;

    if (m_atom == g_textAtom)
    {
        m_type = wxDF_TEXT;
    } else
/*
    if (m_atom == GDK_TARGET_BITMAP)
    {
        m_type = wxDF_BITMAP;
    } else
*/
    {
        m_type = wxDF_PRIVATE;
	m_id = XGetAtomName( (Display*) wxGetDisplay(),  m_atom );

	if (m_id == _T("file:ALL"))
	{
	    m_type = wxDF_FILENAME;
	}
    }
}

void wxDataFormat::SetType( wxDataFormatId type )
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

wxDataFormatId wxDataFormat::GetType() const
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

Atom wxDataFormat::GetAtom()
{
    if (!m_hasAtom)
    {
        m_hasAtom = TRUE;

	if (m_type == wxDF_TEXT)
	{
            m_atom = g_textAtom;
        }
	else
/*
        if (m_type == wxDF_BITMAP)
        {
            m_atom = GDK_TARGET_BITMAP;
        }
	else
*/
        if (m_type == wxDF_PRIVATE)
        {
            m_atom = XInternAtom( (Display*) wxGetDisplay(), MBSTRINGCAST m_id.mbc_str(), FALSE );
        }
	else
	if (m_type == wxDF_FILENAME)
	{
	    m_atom = XInternAtom( (Display*) wxGetDisplay(), "file:ALL", FALSE );
	}
	else
	{
	    m_hasAtom = FALSE;
	    m_atom = (Atom) 0;
	}
    }

    return m_atom;
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

wxDataFormatId wxDataObject::GetFormatType() const
{
    return m_format.GetType();
}

wxString wxDataObject::GetFormatId() const
{
    return m_format.GetId();
}

Atom wxDataObject::GetFormatAtom() const
{
    Atom ret = ((wxDataObject*) this)->m_format.GetAtom();
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
// wxPrivateDataObject
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( wxPrivateDataObject, wxDataObject )

void wxPrivateDataObject::Free()
{
    if ( m_data )
        free(m_data);
}

wxPrivateDataObject::wxPrivateDataObject()
{
    wxString id = _T("application/");
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

