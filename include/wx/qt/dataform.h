/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/dataform.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_DATAFORM_H_
#define _WX_QT_DATAFORM_H_

class WXDLLIMPEXP_CORE wxDataFormat
{
public:
    wxDataFormat();
    wxDataFormat( wxDataFormatId formatId );
    wxDataFormat(const wxString &id);
    wxDataFormat(const wxChar *id);

    void SetId( const wxChar *id );
    
    bool operator==(wxDataFormatId format) const;
    bool operator!=(wxDataFormatId format) const;
    bool operator==(const wxDataFormat& format) const;
    bool operator!=(const wxDataFormat& format) const;
};

#endif // _WX_QT_DATAFORM_H_
