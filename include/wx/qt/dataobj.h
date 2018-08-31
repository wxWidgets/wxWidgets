/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/dataobj.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_DATAOBJ_H_
#define _WX_QT_DATAOBJ_H_

class QMimeData;

class WXDLLIMPEXP_CORE wxDataObject : public wxDataObjectBase
{
public:
    wxDataObject();
    ~wxDataObject();
    
    virtual bool IsSupportedFormat(const wxDataFormat& format, Direction dir) const;
    virtual wxDataFormat GetPreferredFormat(Direction dir = Get) const;
    virtual size_t GetFormatCount(Direction dir = Get) const;
    virtual void GetAllFormats(wxDataFormat *formats, Direction dir = Get) const;
    virtual size_t GetDataSize(const wxDataFormat& format) const;
    virtual bool GetDataHere(const wxDataFormat& format, void *buf) const;
    virtual bool SetData(const wxDataFormat& format, size_t len, const void * buf);

private:
    QMimeData *m_qtMimeData; // to handle formats that have no helper classes
};

#endif // _WX_QT_DATAOBJ_H_
