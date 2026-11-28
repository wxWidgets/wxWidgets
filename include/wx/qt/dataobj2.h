/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/dataobj2.h
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_DATAOBJ2_H_
#define _WX_QT_DATAOBJ2_H_

class WXDLLIMPEXP_CORE wxBitmapDataObject : public wxBitmapDataObjectBase
{
public:
    wxBitmapDataObject();
    wxBitmapDataObject(const wxBitmap& bitmap);

    // Overridden to set image data directly, which Qt will write to clipboard in many formats
    void QtAddDataTo(QMimeData &mimeData) const override;
    // Overridden to retrieve image data from any format that Qt can read from clipboard
    bool QtSetDataFrom(const QMimeData &mimeData) override;

protected:

private:
    wxDECLARE_CLASS(wxBitmapDataObject);
};



class WXDLLIMPEXP_CORE wxFileDataObject : public wxFileDataObjectBase
{
public:
    wxFileDataObject();

    void AddFile( const wxString &filename );

private:
    wxDECLARE_CLASS(wxFileDataObject);
};

#endif // _WX_QT_DATAOBJ2_H_
