///////////////////////////////////////////////////////////////////////////////
// Name:        os2/dataform.h
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
    wxDataFormat(wxDataFormatId vType);
    wxDataFormat(const wxString& rId);
    wxDataFormat(const wxChar* pId);
    wxDataFormat(NativeFormat vFormat);

    wxDataFormat& operator=(NativeFormat vFormat)
        { SetId(vFormat); return *this; }

    // comparison (must have both versions)
    bool operator==(NativeFormat vFormat) const
        { return m_vFormat == (NativeFormat)vFormat; }
    bool operator!=(NativeFormat vFormat) const
        { return m_vFormat != (NativeFormat)vFormat; }

    // explicit and implicit conversions to NativeFormat which is one of
    // standard data types (implicit conversion is useful for preserving the
    // compatibility with old code)
    NativeFormat GetFormatId() const { return m_vFormat; }
    operator NativeFormat() const { return m_vFormat; }

    void SetId(NativeFormat vFormat);

    // string ids are used for custom types - this SetId() must be used for
    // application-specific formats
    wxString GetId() const;
    void SetId(const wxChar* pId);

    // implementation
    wxDataFormatId GetType() const;

private:
    wxDataFormatId                  m_vType;
    NativeFormat                    m_vFormat;

    void PrepareFormats();
    void SetType(wxDataFormatId vType);
};

#endif // _WX_GTK_DATAFORM_H
