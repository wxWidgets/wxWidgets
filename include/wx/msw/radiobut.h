/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/radiobut.h
// Purpose:     wxRadioButton class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RADIOBUT_H_
#define _WX_RADIOBUT_H_

class WXDLLIMPEXP_CORE wxRadioButton: public wxControl
{
public:
    // ctors and creation functions
    wxRadioButton() { Init(); }

    wxRadioButton(wxWindow *parent,
                  wxWindowID id,
                  const wxString& label,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = 0,
                  const wxValidator& validator = wxDefaultValidator,
                  const wxString& name = wxRadioButtonNameStr)
    {
        Init();

        Create(parent, id, label, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxRadioButtonNameStr);

    // implement the radio button interface
    virtual void SetValue(bool value);
    virtual bool GetValue() const;

    // implementation only from now on
    virtual bool MSWCommand(WXUINT param, WXWORD id);
    virtual void Command(wxCommandEvent& event);
    virtual bool SetForegroundColour(const wxColour& colour);
    virtual bool MSWOnDraw(WXDRAWITEMSTRUCT *item);

    virtual bool HasTransparentBackground() { return true; }

    // make the radiobutton owner drawn or reset it to normal style
    void MSWMakeOwnerDrawn(bool ownerDrawn);

    virtual WXDWORD MSWGetStyle(long style, WXDWORD *exstyle) const;

protected:
    virtual wxBorder GetDefaultBorder() const { return wxBORDER_NONE; }
    virtual wxSize DoGetBestSize() const;
    virtual void MSWDrawButtonBitmap(wxWindow *win, wxDC& dc,
                                     const wxRect& rect, int flags);

    // return true if this checkbox is owner drawn
    bool IsOwnerDrawn() const;

private:
    // common part of all ctors
    void Init();

    // event handlers used by owner-drawn radiobutton
    void OnMouseEnterOrLeave(wxMouseEvent& event);
    void OnMouseLeft(wxMouseEvent& event);
    void OnFocus(wxFocusEvent& event);

    // true if the radio button is currently pressed
    bool m_isPressed;

    // we need to store the state internally as the result of GetValue()
    // sometimes gets out of sync in WM_COMMAND handler
    bool m_isChecked;

    DECLARE_DYNAMIC_CLASS_NO_COPY(wxRadioButton)
};

#endif
    // _WX_RADIOBUT_H_
