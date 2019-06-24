/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/dataobj2.h
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_DATAOBJ2_H_
#define _WX_QT_DATAOBJ2_H_

#include <wx/scopedptr.h>

class QByteArray;

class WXDLLIMPEXP_CORE wxBitmapDataObject : public wxBitmapDataObjectBase
{
public:
    wxBitmapDataObject();
    wxBitmapDataObject(const wxBitmap& bitmap);

    ~wxBitmapDataObject();

    void SetBitmap(const wxBitmap& bitmap) wxOVERRIDE;
    size_t GetDataSize() const wxOVERRIDE;
    bool GetDataHere(void *buf) const wxOVERRIDE;
    bool SetData(const wxDataFormat& format, size_t len, const void *buf) wxOVERRIDE;

protected:
    void DoConvertToPng();

private:
    wxScopedPtr<QByteArray> m_imageBytes;
};



class WXDLLIMPEXP_CORE wxFileDataObject : public wxFileDataObjectBase
{
public:
    wxFileDataObject();

    void AddFile( const wxString &filename );
};

#endif // _WX_QT_DATAOBJ2_H_
