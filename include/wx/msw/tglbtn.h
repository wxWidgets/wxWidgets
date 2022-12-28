/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/tglbtn.h
// Purpose:     Declaration of the wxToggleButton class, which implements a
//              toggle button under wxMSW.
// Author:      John Norris, minor changes by Axel Schlueter
// Modified by:
// Created:     08.02.01
// Copyright:   (c) 2000 Johnny C. Norris II
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TOGGLEBUTTON_H_
#define _WX_TOGGLEBUTTON_H_

#include "wx/bmpbndl.h"

// Checkbox item (single checkbox)
class WXDLLIMPEXP_CORE wxToggleButton : public wxToggleButtonBase
{
public:
    wxToggleButton() { Init(); }
    wxToggleButton(wxWindow *parent,
                   wxWindowID id,
                   const wxString& label,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxASCII_STR(wxCheckBoxNameStr))
    {
        Create(parent, id, label, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxCheckBoxNameStr));

    virtual void SetValue(bool value) override;
    virtual bool GetValue() const override;

    virtual bool MSWCommand(WXUINT param, WXWORD id) override;
    virtual void Command(wxCommandEvent& event) override;

    // returns true if the platform should explicitly apply a theme border
    virtual bool CanApplyThemeBorder() const override { return false; }

protected:
    virtual wxBorder GetDefaultBorder() const override { return wxBORDER_NONE; }

    virtual WXDWORD MSWGetStyle(long flags, WXDWORD *exstyle = nullptr) const override;

    virtual bool MSWIsPushed() const override;

    void Init();

    // current state of the button (when owner-drawn)
    bool m_state;

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxToggleButton);
};

//-----------------------------------------------------------------------------
// wxBitmapToggleButton
//-----------------------------------------------------------------------------


class WXDLLIMPEXP_CORE wxBitmapToggleButton: public wxToggleButton
{
public:
    // construction/destruction
    wxBitmapToggleButton() {}
    wxBitmapToggleButton(wxWindow *parent,
                   wxWindowID id,
                   const wxBitmapBundle& label,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxASCII_STR(wxCheckBoxNameStr))
    {
        Create(parent, id, label, pos, size, style, validator, name);
    }

    // Create the control
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxBitmapBundle& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxCheckBoxNameStr));

    // deprecated synonym for SetBitmapLabel()
    wxDEPRECATED_INLINE( void SetLabel(const wxBitmap& bitmap),
       SetBitmapLabel(bitmap); )
    // prevent virtual function hiding
    virtual void SetLabel(const wxString& label) override { wxToggleButton::SetLabel(label); }

private:
    wxDECLARE_DYNAMIC_CLASS(wxBitmapToggleButton);
};

#endif // _WX_TOGGLEBUTTON_H_

