///////////////////////////////////////////////////////////////////////////////
// Name:        gtk/dataform.h
// Purpose:     declaration of the wxDataFormat class
// Author:      David Webster (lifted from dnd.h)
// Modified by:
// Created:     10/21/99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 David Webster
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_OS2_DATAFORM_H
#define _WX_OS2_DATAFORM_H

class wxDataFormat
{
public:
    // the clipboard formats under GDK are GdkAtoms
    typedef unsigned short NativeFormat;

    wxDataFormat();
    wxDataFormat( wxDataFormatId type );
    wxDataFormat( const wxString &id );
    wxDataFormat( const wxChar *id );
    wxDataFormat( NativeFormat format );

    wxDataFormat& operator=(NativeFormat format)
        { SetId(format); return *this; }

    // comparison (must have both versions)
    bool operator==(NativeFormat format) const
        { return m_format == (NativeFormat)format; }
    bool operator!=(NativeFormat format) const
        { return m_format != (NativeFormat)format; }

    // explicit and implicit conversions to NativeFormat which is one of
    // standard data types (implicit conversion is useful for preserving the
    // compatibility with old code)
    NativeFormat GetFormatId() const { return m_format; }
    operator NativeFormat() const { return m_format; }

    void SetId( NativeFormat format );

    // string ids are used for custom types - this SetId() must be used for
    // application-specific formats
    wxString GetId() const;
    void SetId( const wxChar *id );

    // implementation
    wxDataFormatId GetType() const;

private:
    wxDataFormatId   m_type;
    NativeFormat     m_format;

    void PrepareFormats();
    void SetType( wxDataFormatId type );
};

#endif // _WX_GTK_DATAFORM_H
