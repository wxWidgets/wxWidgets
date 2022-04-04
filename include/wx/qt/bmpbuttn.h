/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/bmpbuttn.h
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_BMPBUTTN_H_
#define _WX_QT_BMPBUTTN_H_

class WXDLLIMPEXP_CORE wxBitmapButton : public wxBitmapButtonBase
{
public:
    wxBitmapButton();

    wxBitmapButton(wxWindow *parent,
                   wxWindowID id,
                   const wxBitmapBundle& bitmap,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxASCII_STR(wxButtonNameStr));

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxBitmapBundle& bitmap,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxButtonNameStr));

    bool CreateCloseButton(wxWindow* parent,
                           wxWindowID winid,
                           const wxString& name = wxString());

protected:
    wxDECLARE_DYNAMIC_CLASS(wxBitmapButton);

private:
    // We re-use wxButton
};

#endif // _WX_QT_BMPBUTTN_H_
