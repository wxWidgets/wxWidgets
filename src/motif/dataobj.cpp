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

#ifdef __VMS__
#pragma message disable nosimpint
#endif
#include <Xm/Xm.h>
#ifdef __VMS__
#pragma message enable nosimpint
#endif
#include "wx/utils.h"

//-------------------------------------------------------------------------
// global data
//-------------------------------------------------------------------------

Atom  g_textAtom        = 0;
Atom  g_pngAtom         = 0;
Atom  g_fileAtom        = 0;

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
    m_format = (Atom) 0;
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
    char *t = XGetAtomName ((Display*) wxGetDisplay(), m_format);
    wxString ret( t );  // this will convert from ascii to Unicode
    if (t) 
        XFree( t );
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
    m_format = XInternAtom( (Display*) wxGetDisplay(), wxMBSTRINGCAST tmp.mbc_str(), FALSE );  // what is the string cast for?
}

void wxDataFormat::PrepareFormats()
{
    if (!g_textAtom)
        g_textAtom = XInternAtom( (Display*) wxGetDisplay(), "STRING", FALSE );
    if (!g_pngAtom)
        g_pngAtom = XInternAtom( (Display*) wxGetDisplay(), "image/png", FALSE );
    if (!g_fileAtom)
        g_fileAtom = XInternAtom( (Display*) wxGetDisplay(), "file:ALL", FALSE );
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
