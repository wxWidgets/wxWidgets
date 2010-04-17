/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/dataobj.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_DATAOBJ_H_
#define _WX_QT_DATAOBJ_H_

class WXDLLIMPEXP_CORE wxDataObject : public wxDataObjectBase
{
public:
    wxDataObject();
    
    virtual wxDataFormat GetPreferredFormat(Direction dir = Get) const;
    virtual size_t GetFormatCount(Direction dir = Get) const;
    virtual void GetAllFormats(wxDataFormat *formats, Direction dir = Get) const;
    virtual size_t GetDataSize(const wxDataFormat& format) const;
    virtual bool GetDataHere(const wxDataFormat& format, void *buf) const;
};

#endif // _WX_QT_DATAOBJ_H_
