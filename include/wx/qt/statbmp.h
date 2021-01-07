/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/statbmp.h
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_STATBMP_H_
#define _WX_QT_STATBMP_H_

class QLabel;

class WXDLLIMPEXP_CORE wxStaticBitmap : public wxStaticBitmapBase
{
public:
    wxStaticBitmap();
    wxStaticBitmap( wxWindow *parent,
                    wxWindowID id,
                    const wxBitmap& label,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = 0,
                    const wxString& name = wxASCII_STR(wxStaticBitmapNameStr) );

    bool Create( wxWindow *parent,
                 wxWindowID id,
                 const wxBitmap& label,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxString& name = wxASCII_STR(wxStaticBitmapNameStr));

    virtual void SetIcon(const wxIcon& icon) wxOVERRIDE;
    virtual void SetBitmap(const wxBitmap& bitmap) wxOVERRIDE;
    virtual wxBitmap GetBitmap() const wxOVERRIDE;
    virtual wxIcon GetIcon() const wxOVERRIDE;

    virtual QWidget *GetHandle() const wxOVERRIDE;
protected:

private:
    QLabel *m_qtLabel;

    wxDECLARE_DYNAMIC_CLASS(wxStaticBitmap);
};

#endif // _WX_QT_STATBMP_H_
