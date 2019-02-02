/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/dataform.h
// Author:      Sean D'Epagnier
// Copyright:   (c) Sean D'Epagnier 2014
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_DATAFORM_H_
#define _WX_QT_DATAFORM_H_

class WXDLLIMPEXP_CORE wxDataFormat
{
public:
    wxDataFormat(wxDataFormatId formatId = wxDF_INVALID);
    wxDataFormat(const wxString &id);

    // Standard methods
    const wxString& GetId() const;
    void SetId(const wxString& id);

    wxDataFormatId GetType() const;
    void SetType(wxDataFormatId type);

    bool operator==(wxDataFormatId format) const;
    bool operator!=(wxDataFormatId format) const;
    bool operator==(const wxDataFormat& format) const;
    bool operator!=(const wxDataFormat& format) const;

    // Direct access to the underlying mime type.
    // Equivalent to "id", except "id" is supposed to be
    // invalid for standard types, whereas this should
    // always be valid (if meaningful).
    const wxString& GetMimeType() const;
    void SetMimeType(const wxString& mimeType);

private:
    wxString m_mimeType;
    wxDataFormatId m_formatId;
};

#endif // _WX_QT_DATAFORM_H_
