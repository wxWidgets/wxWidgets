///////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/infobar.h
// Purpose:     generic wxInfoBar class declaration
// Author:      Vadim Zeitlin
// Created:     2009-07-28
// Copyright:   (c) 2009 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_INFOBAR_H_
#define _WX_GENERIC_INFOBAR_H_

#include "wx/containr.h"

class WXDLLIMPEXP_FWD_CORE wxBitmapButton;
class WXDLLIMPEXP_FWD_CORE wxStaticBitmap;
class WXDLLIMPEXP_FWD_CORE wxStaticText;
class WXDLLIMPEXP_FWD_CORE wxCheckBox;

// ----------------------------------------------------------------------------
// wxInfoBar
// ----------------------------------------------------------------------------

enum
{
    wxINFOBAR_CHECKBOX = 0x0010
};

class WXDLLIMPEXP_CORE wxInfoBarGeneric : public wxNavigationEnabled<wxInfoBarBase>
{
public:
    // the usual ctors and Create() but remember that info bar is created
    // hidden
    wxInfoBarGeneric() = default;

    wxInfoBarGeneric(wxWindow *parent, wxWindowID winid = wxID_ANY, long style = 0)
    {
        Create(parent, winid, style);
    }

    bool Create(wxWindow *parent, wxWindowID winid = wxID_ANY, long style = 0);


    // implement base class methods
    // ----------------------------

    virtual void ShowMessage(const wxString& msg,
                             int flags = wxICON_INFORMATION) override;

    virtual void Dismiss() override;

    virtual void AddButton(wxWindowID btnid, const wxString& label = wxString()) override;

    virtual void RemoveButton(wxWindowID btnid) override;

    virtual size_t GetButtonCount() const override;
    virtual wxWindowID GetButtonId(size_t idx) const override;
    virtual bool HasButtonId(wxWindowID btnid) const override;

    virtual void ShowCloseButton(ShowButton show) override;

    // methods specific to this version
    // --------------------------------

    // set the effect(s) to use when showing/hiding the bar, may be
    // wxSHOW_EFFECT_NONE to disable any effects entirely
    //
    // by default, slide to bottom/top is used when it's positioned on the top
    // of the window for showing/hiding it and top/bottom when it's positioned
    // at the bottom
    void SetShowHideEffects(wxShowEffect showEffect, wxShowEffect hideEffect)
    {
        m_showEffect = showEffect;
        m_hideEffect = hideEffect;
    }

    // get effect used when showing/hiding the window
    wxShowEffect GetShowEffect() const;
    wxShowEffect GetHideEffect() const;

    // set the duration of animation used when showing/hiding the bar, in ms
    void SetEffectDuration(int duration) { m_effectDuration = duration; }

    // get the currently used effect animation duration
    int GetEffectDuration() const { return m_effectDuration; }

    // Whether the checkbox was checked at the time of the window
    // being closed.
    // This should be called in a client's handler for the
    // wxID_CLOSE button being clicked.
    bool IsCheckBoxChecked() const { return m_checked; }

    // Sets whether the checkbox should be shown.
    void ShowCheckBox(const wxString& checkBoxText, bool checked);

    // overridden base class methods
    // -----------------------------

    // setting the font of this window sets it for the text control inside it
    // (default font is a larger and bold version of the normal one)
    virtual bool SetFont(const wxFont& font) override;

    // same thing with the colour: this affects the text and checkbox colour
    virtual bool SetForegroundColour(const wxColor& colour) override;

    // background colour also needs to be applied to the close button
    virtual bool SetBackgroundColour(const wxColour& colour) override;

#if wxUSE_ACCESSIBILITY
    virtual wxAccessible* CreateAccessible() override;
#endif // wxUSE_ACCESSIBILITY

protected:
    // info bar shouldn't have any border by default, the colour difference
    // between it and the main window separates it well enough
    virtual wxBorder GetDefaultBorder() const override { return wxBORDER_NONE; }


    // update the parent to take our new or changed size into account (notably
    // should be called when we're shown or hidden)
    void UpdateParent();

private:
    virtual bool UseNative() const { return false; }

    // handler for the close button
    void OnButton(wxCommandEvent& event);

    // show or hide the close button depending on m_showCloseButton and the
    // presence of the other buttons, also ensuring that it comes after them
    void UpdateCloseButton();

    // show/hide the bar
    void DoShow();
    void DoHide();

    // determine the placement of the bar from its position in the containing
    // sizer
    enum BarPlacement
    {
        BarPlacement_Top,
        BarPlacement_Bottom,
        BarPlacement_Unknown
    };

    BarPlacement GetBarPlacement() const;

    void UpdateColours();
    void UpdateCloseButtonBackground();
    void OnSysColourChanged(wxSysColourChangedEvent& event);

    // different controls making up the bar
    wxStaticBitmap *m_icon = nullptr;
    wxStaticText *m_text = nullptr;
    wxBitmapButton *m_button = nullptr;
    wxCheckBox *m_checkbox = nullptr;

    // the effects to use when showing/hiding and duration for them: by default
    // the effect is determined by the info bar automatically depending on its
    // position and the default duration is used
    wxShowEffect m_showEffect = wxSHOW_EFFECT_MAX,
                 m_hideEffect = wxSHOW_EFFECT_MAX;
    int m_effectDuration = 0;

    // when to show m_button
    ShowButton m_showCloseButton = ShowButton::Auto;

    bool m_checked = false;

#if wxUSE_ACCESSIBILITY
    friend class wxInfoBarGenericAccessible;
#endif // wxUSE_ACCESSIBILITY

    wxDECLARE_EVENT_TABLE();
    wxDECLARE_NO_COPY_CLASS(wxInfoBarGeneric);
};

#endif // _WX_GENERIC_INFOBAR_H_

