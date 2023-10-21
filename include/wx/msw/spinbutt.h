/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/spinbutt.h
// Purpose:     wxSpinButton class
// Author:      Julian Smart
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SPINBUTT_H_
#define _WX_SPINBUTT_H_

#include "wx/control.h"
#include "wx/event.h"

#if wxUSE_SPINBTN

class WXDLLIMPEXP_CORE wxSpinButton : public wxSpinButtonBase
{
public:
    // construction
    wxSpinButton() { }

    wxSpinButton(wxWindow *parent,
                 wxWindowID id = wxID_ANY,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxSP_VERTICAL | wxSP_ARROW_KEYS,
                 const wxString& name = wxSPIN_BUTTON_NAME)
    {
        Create(parent, id, pos, size, style, name);
    }

    virtual ~wxSpinButton();

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSP_VERTICAL | wxSP_ARROW_KEYS,
                const wxString& name = wxSPIN_BUTTON_NAME);


    // accessors
    virtual int GetValue() const override;
    virtual void SetValue(int val) override;
    virtual void SetRange(int minVal, int maxVal) override;

    // implementation
    virtual bool MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result) override;
    virtual bool MSWOnScroll(int orientation, WXWORD wParam,
                             WXWORD pos, WXHWND control) override;

    // a wxSpinButton can't do anything useful with focus, only wxSpinCtrl can
    virtual bool AcceptsFocus() const override { return false; }

    // returns true if the platform should explicitly apply a theme border
    virtual bool CanApplyThemeBorder() const override { return false; }
    virtual void SetIncrement(int value) override;
    virtual int  GetIncrement() const override;

protected:
   virtual wxSize DoGetBestSize() const override;

   // ensure that the control displays a value in the current range
   virtual void NormalizeValue();

private:
    void OnPaint(wxPaintEvent& event);

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxSpinButton);
};

#endif // wxUSE_SPINBTN

#endif // _WX_SPINBUTT_H_
