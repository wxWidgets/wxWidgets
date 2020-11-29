///////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/dataform.h
// Purpose:     declaration of the wxDataFormat class
// Author:      Stefan Csomor (lifted from dnd.h)
// Modified by:
// Created:     10/21/99
// Copyright:   (c) 1999 Stefan Csomor
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MAC_DATAFORM_H
#define _WX_MAC_DATAFORM_H

#include "wx/osx/core/cfstring.h"

class WXDLLIMPEXP_CORE wxDataFormat
{
public:
    typedef CFStringRef NativeFormat;

    wxDataFormat();
    wxDataFormat(wxDataFormatId vType);
    wxDataFormat(const wxDataFormat& rFormat);
    wxDataFormat(const wxString& rId);
    wxDataFormat(const wxChar* pId);
    wxDataFormat(NativeFormat vFormat);
    ~wxDataFormat();

    wxDataFormat& operator=(NativeFormat vFormat)
        { SetId(vFormat); return *this; }

    // comparison (must have both versions)
    bool operator==(const wxDataFormat& format) const ;
    bool operator!=(const wxDataFormat& format) const
        { return ! ( *this == format ); }
    bool operator==(wxDataFormatId format) const
        { return m_type == (wxDataFormatId)format; }
    bool operator!=(wxDataFormatId format) const
        { return m_type != (wxDataFormatId)format; }

    wxDataFormat& operator=(const wxDataFormat& format);

    // explicit and implicit conversions to NativeFormat which is one of
    // standard data types (implicit conversion is useful for preserving the
    // compatibility with old code)
    NativeFormat GetFormatId() const { return m_format; }
    operator NativeFormat() const { return m_format; }

    void SetId(NativeFormat format);
    
    // string ids are used for custom types - this SetId() must be used for
    // application-specific formats
    wxString GetId() const;
    void SetId(const wxString& pId);

    // implementation
    wxDataFormatId GetType() const { return m_type; }
    void SetType( wxDataFormatId type );
    static NativeFormat GetFormatForType(wxDataFormatId type);

    // returns true if the format is one of those defined in wxDataFormatId
    bool IsStandard() const { return m_type > 0 && m_type < wxDF_PRIVATE; }

    // adds all the native formats for this format when calling a GetData
    void AddSupportedTypesForGetting(CFMutableArrayRef types) const;

    // adds all the native formats for this format when calling a SetData
    void AddSupportedTypesForSetting(CFMutableArrayRef types) const;
private:
    void DoAddSupportedTypes(CFMutableArrayRef types, bool forSetting) const;

    void ClearNativeFormat();

    wxDataFormatId  m_type;
    wxString        m_id;
    wxCFStringRef   m_format;
};

#endif // _WX_MAC_DATAFORM_H
