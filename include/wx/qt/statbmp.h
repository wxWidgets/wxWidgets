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
    wxStaticBitmap() = default;

    wxStaticBitmap( wxWindow *parent,
                    wxWindowID id,
                    const wxBitmapBundle& label,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = 0,
                    const wxString& name = wxASCII_STR(wxStaticBitmapNameStr) );

    bool Create( wxWindow *parent,
                 wxWindowID id,
                 const wxBitmapBundle& label,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxString& name = wxASCII_STR(wxStaticBitmapNameStr));

    virtual void SetBitmap(const wxBitmapBundle& bitmap) override;
    virtual wxBitmap GetBitmap() const override;

    QLabel* GetQLabel() const;

    wxDECLARE_DYNAMIC_CLASS(wxStaticBitmap);
};

#endif // _WX_QT_STATBMP_H_
