///////////////////////////////////////////////////////////////////////////////
// Name:        msw/ole/dataform.h
// Purpose:     declaration of the wxDataFormat class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     19.10.99 (extracted from msw/ole/dataobj.h)
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef   _WX_MSW_OLE_DATAFORM_H
#define   _WX_MSW_OLE_DATAFORM_H

// ----------------------------------------------------------------------------
// wxDataFormat identifies the single format of data
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxDataFormat
{
public:
    // the clipboard formats under Win32 are UINTs
    typedef unsigned int NativeFormat;

    wxDataFormat(NativeFormat format = wxDF_INVALID) { m_format = format; }
    wxDataFormat(const wxChar *format) { SetId(format); }

    wxDataFormat& operator=(NativeFormat format)
        { m_format = format; return *this; }
    wxDataFormat& operator=(const wxDataFormat& format)
        { m_format = format.m_format; return *this; }

    // default copy ctor/assignment operators ok

    // comparison (must have both versions)
    bool operator==(wxDataFormatId format) const
        { return m_format == (NativeFormat)format; }
    bool operator!=(wxDataFormatId format) const
        { return m_format != (NativeFormat)format; }
    bool operator==(const wxDataFormat& format) const
        { return m_format == format.m_format; }
    bool operator!=(const wxDataFormat& format) const
        { return m_format != format.m_format; }

    // explicit and implicit conversions to NativeFormat which is one of
    // standard data types (implicit conversion is useful for preserving the
    // compatibility with old code)
    NativeFormat GetFormatId() const { return m_format; }
    operator NativeFormat() const { return m_format; }

    // this only works with standard ids
    void SetType(wxDataFormatId format) { m_format = format; }
    wxDataFormatId GetType() const { return m_format; }

    // string ids are used for custom types - this SetId() must be used for
    // application-specific formats
    wxString GetId() const;
    void SetId(const wxChar *format);

private:
    // returns TRUE if the format is one of those defined in wxDataFormatId
    bool IsStandard() const { return m_format > 0 && m_format < wxDF_MAX; }

    NativeFormat m_format;
};

#endif //  _WX_MSW_OLE_DATAFORM_H

