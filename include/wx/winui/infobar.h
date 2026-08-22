/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/infobar.h
// Purpose:     wxWinUI wxInfoBar declaration (WinUI InfoBar)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_INFOBAR_H_
#define _WX_WINUI_INFOBAR_H_

#include "wx/vector.h"

#include <memory>

// Unlike wxGTK, this port does not build wxInfoBarGeneric (the generic
// implementation is deliberately excluded from the build, as for the other
// natively implemented controls), so the common style flag is defined here
// with the same value as in the generic header instead of including it; the
// guard keeps a single definition if both headers ever end up in one TU.
#ifndef _WX_GENERIC_INFOBAR_H_
enum
{
    wxINFOBAR_CHECKBOX = 0x0010
};
#endif // !_WX_GENERIC_INFOBAR_H_

class wxWinUIInfoBarImpl;
using wxWinUIInfoBarContentWriteHookForTesting = void (*)(void *);

class WXDLLIMPEXP_CORE wxInfoBar : public wxInfoBarBase
{
public:
    wxInfoBar();
    wxInfoBar(wxWindow *parent, wxWindowID winid = wxID_ANY, long style = 0);
    ~wxInfoBar() override;

    bool Create(wxWindow *parent, wxWindowID winid = wxID_ANY, long style = 0);

    // implement base class methods
    // ----------------------------

    void ShowMessage(const wxString& msg, int flags = wxICON_INFORMATION) override;
    void Dismiss() override;

    void AddButton(wxWindowID btnid, const wxString& label = wxString()) override;
    void RemoveButton(wxWindowID btnid) override;

    size_t GetButtonCount() const override;
    wxWindowID GetButtonId(size_t idx) const override;
    bool HasButtonId(wxWindowID btnid) const override;

    // methods with the same contract as the generic version
    // -----------------------------------------------------

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

    // set the duration of animation used when showing/hiding the bar, in ms;
    // the value is clamped to a sane range: negative values mean "use the
    // platform default" (0) and are never passed through to the unsigned
    // AnimateWindow() timeout, and absurdly long animations are capped
    void SetEffectDuration(int duration)
    {
        if ( duration < 0 )
            duration = 0;
        else if ( duration > 10000 )
            duration = 10000;
        m_effectDuration = duration;
    }

    // get the currently used (clamped) effect animation duration
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

    // setting the font of this window sets it for the message shown inside it
    bool SetFont(const wxFont& font) override;

    // same thing with the colour: this affects the text colour
    bool SetForegroundColour(const wxColor& colour) override;

    // implementation only from now on
    // -------------------------------

    // Whether the native InfoBar peer is currently open; lets the unit
    // tests check that the XAML side stays in sync with the wx one.
    bool WinUIIsPeerOpen() const;

    // Invoke the native close (X) button exactly like a user click does (via
    // its automation peer), so the real CloseButtonClick/Closing path runs
    // deterministically without the mouse.  Returns false while the control
    // template has not been realized yet.  Used by the unit tests.
    bool WinUIClickCloseButton();

    // Invoke a custom content button through its real XAML automation peer.
    // This is an implementation-only lifetime seam used to prove that a wx
    // handler may destroy the InfoBar while the native callback unwinds.
    bool WinUIClickButtonForTesting(wxWindowID btnid);

    // One-shot seam immediately after the candidate XAML Content write and
    // before its callback generation is published.
    void WinUISetNextContentWriteHookForTesting(
        wxWinUIInfoBarContentWriteHookForTesting hook,
        void *context);
    bool WinUIHasDeferredContentProjectionForTesting() const;
    bool WinUIIsContentProjectionQuarantinedForTesting() const;

protected:
    // info bar shouldn't have any border by default, the colour difference
    // between it and the main window separates it well enough
    wxBorder GetDefaultBorder() const override { return wxBORDER_NONE; }

    wxSize DoGetBestSize() const override;

    // rebuild the XAML content of the bar (custom buttons and checkbox)
    void RebuildContent(bool externalMutation = true);
    void UpdateParent();
    void OnButtonClick(wxWindowID btnid);

    // show/hide the bar using the configured effects
    void DoShow();
    void DoHide();

    struct ButtonInfo
    {
        wxWindowID id = wxID_NONE;
        wxString label;
    };

    std::unique_ptr<wxWinUIInfoBarImpl> m_winui;
    wxVector<ButtonInfo> m_buttons;

private:
    // determine the placement of the bar from its position in the containing
    // sizer, used to pick the default show/hide effects
    enum BarPlacement
    {
        BarPlacement_Top,
        BarPlacement_Bottom,
        BarPlacement_Unknown
    };

    BarPlacement GetBarPlacement() const;

    // the effects to use when showing/hiding and duration for them: by
    // default the effect is determined by the info bar automatically
    // depending on its position and the default duration is used
    wxShowEffect m_showEffect = wxSHOW_EFFECT_MAX;
    wxShowEffect m_hideEffect = wxSHOW_EFFECT_MAX;
    int m_effectDuration = 0;

    // the checkbox label ("" while the checkbox is hidden) and its state,
    // kept in sync with the XAML checkbox by its event handlers
    wxString m_checkBoxLabel;
    bool m_checked = false;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxInfoBar);
};

#endif // _WX_WINUI_INFOBAR_H_
