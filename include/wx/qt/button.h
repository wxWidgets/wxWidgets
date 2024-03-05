/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/button.h
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_BUTTON_H_
#define _WX_QT_BUTTON_H_

#include "wx/control.h"
#include "wx/button.h"

class WXDLLIMPEXP_CORE wxButton : public wxButtonBase
{
public:
    wxButton();
    wxButton(wxWindow *parent, wxWindowID id,
           const wxString& label = wxEmptyString,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxASCII_STR(wxButtonNameStr));

    bool Create(wxWindow *parent, wxWindowID id,
           const wxString& label = wxEmptyString,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxASCII_STR(wxButtonNameStr));

   virtual wxWindow *SetDefault() override;

    // implementation only
    virtual int QtGetEventType() const override { return wxEVT_BUTTON; }

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxButton);
};


#endif // _WX_QT_BUTTON_H_
