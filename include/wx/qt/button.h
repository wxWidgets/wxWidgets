/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/button.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_BUTTON_H_
#define _WX_QT_BUTTON_H_

#include <QtGui/QPushButton>

class WXDLLIMPEXP_CORE wxButton : public wxButtonBase
{
public:
    wxButton();
    wxButton(wxWindow *parent, wxWindowID id,
           const wxString& label = wxEmptyString,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxButtonNameStr);

    bool Create(wxWindow *parent, wxWindowID id,
           const wxString& label = wxEmptyString,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxButtonNameStr);

protected:
    virtual QPushButton *GetHandle() const;
};

#endif // _WX_QT_BUTTON_H_
