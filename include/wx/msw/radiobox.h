/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/radiobox.h
// Purpose:     wxRadioBox class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RADIOBOX_H_
#define _WX_RADIOBOX_H_

#include "wx/compositewin.h"
#include "wx/statbox.h"

class WXDLLIMPEXP_FWD_CORE wxRadioButton;

// ----------------------------------------------------------------------------
// wxRadioBox
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxRadioBox : public wxCompositeWindow<wxStaticBox>,
                                    public wxRadioBoxBase
{
public:
    wxRadioBox() { Init(); }

    wxRadioBox(wxWindow *parent,
               wxWindowID id,
               const wxString& title,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               int n = 0, const wxString choices[] = nullptr,
               int majorDim = 0,
               long style = wxRA_SPECIFY_COLS,
               const wxValidator& val = wxDefaultValidator,
               const wxString& name = wxASCII_STR(wxRadioBoxNameStr))
    {
        Init();

        (void)Create(parent, id, title, pos, size, n, choices, majorDim,
                     style, val, name);
    }

    wxRadioBox(wxWindow *parent,
               wxWindowID id,
               const wxString& title,
               const wxPoint& pos,
               const wxSize& size,
               const wxArrayString& choices,
               int majorDim = 0,
               long style = wxRA_SPECIFY_COLS,
               const wxValidator& val = wxDefaultValidator,
               const wxString& name = wxASCII_STR(wxRadioBoxNameStr))
    {
        Init();

        (void)Create(parent, id, title, pos, size, choices, majorDim,
                     style, val, name);
    }

    virtual ~wxRadioBox();

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0, const wxString choices[] = nullptr,
                int majorDim = 0,
                long style = wxRA_SPECIFY_COLS,
                const wxValidator& val = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxRadioBoxNameStr));
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                int majorDim = 0,
                long style = wxRA_SPECIFY_COLS,
                const wxValidator& val = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxRadioBoxNameStr));

    // implement the radiobox interface
    virtual void SetSelection(int n) override;
    virtual int GetSelection() const override { return m_selectedButton; }
    virtual unsigned int GetCount() const override;
    virtual wxString GetString(unsigned int n) const override;
    virtual void SetString(unsigned int n, const wxString& label) override;
    virtual bool Enable(unsigned int n, bool enable = true) override;
    virtual bool Show(unsigned int n, bool show = true) override;
    virtual bool IsItemEnabled(unsigned int n) const override;
    virtual bool IsItemShown(unsigned int n) const override;
    virtual int GetItemFromPoint(const wxPoint& pt) const override;

    // override some base class methods
    virtual bool Show(bool show = true) override;
    virtual bool Enable(bool enable = true) override;
    virtual bool CanBeFocused() const override;
    virtual void SetFocus() override;
#if wxUSE_TOOLTIPS
    virtual bool HasToolTips() const override;
#endif // wxUSE_TOOLTIPS
#if wxUSE_HELP
    // override virtual function with a platform-independent implementation
    virtual wxString GetHelpTextAtPoint(const wxPoint & pt, wxHelpEvent::Origin origin) const override
    {
        return wxRadioBoxBase::DoGetHelpTextAtPoint( this, pt, origin );
    }
#endif // wxUSE_HELP

    // returns true if the platform should explicitly apply a theme border
    virtual bool CanApplyThemeBorder() const override { return false; }


    // Non-portable MSW-specific functions.
#if WXWIN_COMPATIBILITY_3_2
    wxDEPRECATED_MSG("Does nothing, don't use")
    void SetLabelFont(const wxFont& WXUNUSED(font)) {}
    wxDEPRECATED_MSG("Use portable SetFont() instead")
    void SetButtonFont(const wxFont& font) { SetFont(font); }
#endif // WXWIN_COMPATIBILITY_3_2


    // implementation only from now on
    // -------------------------------

    void Command(wxCommandEvent& event) override;

    void SendNotificationEvent();

protected:
    // common part of all ctors
    void Init();

#if wxUSE_TOOLTIPS
    virtual void DoSetItemToolTip(unsigned int n, wxToolTip * tooltip) override;
#endif

    // resolve ambiguity in base classes
    virtual wxBorder GetDefaultBorder() const override { return wxRadioBoxBase::GetDefaultBorder(); }

    virtual wxWindowList GetCompositeWindowParts() const override;

    void WXOnRadioButton(wxCommandEvent& event);
    void WXOnRadioKeyDown(wxKeyEvent& event);


    // the buttons we contain
    wxVector<wxRadioButton*> m_radioButtons;

    // currently selected button or wxNOT_FOUND if none
    int m_selectedButton;

private:
    wxDECLARE_DYNAMIC_CLASS(wxRadioBox);
    wxDECLARE_NO_COPY_CLASS(wxRadioBox);
};

#endif
    // _WX_RADIOBOX_H_
