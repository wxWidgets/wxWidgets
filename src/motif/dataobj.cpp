///////////////////////////////////////////////////////////////////////////////
// Name:        dataobj.cpp
// Purpose:     wxDataObject class
// Author:      Julian Smart
// Id:          $Id$
// Copyright:   (c) 1998 Julian Smart
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dataobj.h"
#endif

#include "wx/defs.h"

#if wxUSE_CLIPBOARD

#include "wx/dataobj.h"
#include "wx/app.h"

#include <Xm/Xm.h>
#include "wx/utils.h"

//-------------------------------------------------------------------------
// global data
//-------------------------------------------------------------------------

Atom  g_textAtom = 0;

//-------------------------------------------------------------------------
// wxDataFormat
//-------------------------------------------------------------------------

wxDataFormat::wxDataFormat()
{
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

	if (m_id == wxT("file:ALL"))
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
        m_id = wxT("STRING");
    }
    else
    if (m_type == wxDF_BITMAP)
    {
        m_id = wxT("BITMAP");
    }
    else
    if (m_type == wxDF_FILENAME)
    {
        m_id = wxT("file:ALL");
    }
    else
    {
       wxFAIL_MSG( wxT("invalid dataformat") );
    }

    m_hasAtom = FALSE;
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
            m_atom = XInternAtom( (Display*) wxGetDisplay(), wxMBSTRINGCAST m_id.mbc_str(), FALSE );
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

#if 0

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

#endif // 0

#endif // wxUSE_CLIPBOARD
