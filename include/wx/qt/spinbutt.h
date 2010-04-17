/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/spinbutt.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_SPINBUTT_H_
#define _WX_QT_SPINBUTT_H_

class WXDLLIMPEXP_CORE wxSpinButton : public wxSpinButtonBase
{
public:
    wxSpinButton();
    wxSpinButton(wxWindow *parent,
                 wxWindowID id = -1,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxSP_VERTICAL,
                 const wxString& name = wxSPIN_BUTTON_NAME);

    bool Create(wxWindow *parent,
                wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSP_VERTICAL,
                const wxString& name = wxSPIN_BUTTON_NAME);

    virtual int GetValue() const;
    virtual void SetValue(int val);

protected:

private:
};

#endif // _WX_QT_SPINBUTT_H_
