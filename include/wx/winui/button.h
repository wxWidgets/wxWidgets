/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/button.h
// Purpose:     wxWinUI wxButton declaration
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_BUTTON_H_
#define _WX_WINUI_BUTTON_H_

#include "wx/bmpbndl.h"

#include <cstdint>
#include <memory>

class wxWinUIButtonImpl;
class wxDPIChangedEvent;

class WXDLLIMPEXP_CORE wxButton : public wxButtonBase
{
public:
    wxButton();
    wxButton(wxWindow *parent,
             wxWindowID id,
             const wxString& label = wxEmptyString,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxASCII_STR(wxButtonNameStr));
    ~wxButton() override;

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxButtonNameStr));

    void SetLabel(const wxString& label) override;
    wxWindow *SetDefault() override;
    void Command(wxCommandEvent& event) override;
    bool Show(bool show = true) override;
    bool SetBackgroundColour(const wxColour& colour) override;
    bool SetForegroundColour(const wxColour& colour) override;
    bool SetFont(const wxFont& font) override;
    // Implementation-only deterministic lifetime and bitmap-projection
    // seams. The projection seam rebuilds the real XAML Content tree without
    // synthesizing pointer/focus input.
    bool WinUIQueueClickForTesting();
    static unsigned WinUIGetLiveCallbackStateCountForTesting();
    static unsigned WinUIGetPeerInvokeAttemptCountForTesting();
    bool WinUIProjectBitmapStateForTesting(State state, double scale);
    bool WinUIGetPeerBitmapProjectionForTesting(
        wxSize *bitmapPixelSize,
        wxSize *authPixelSize,
        State *state,
        std::uint64_t *generation) const;

protected:
    bool MSWOnEffectiveLayoutDirectionChanged() override;
    bool SendClickEvent();
    void DoEnable(bool enable) override;
    wxSize DoGetBestSize() const override;
    wxBitmap DoGetBitmap(State which) const override;
    void DoSetBitmap(const wxBitmapBundle& bitmap, State which) override;
    wxSize DoGetBitmapMargins() const override;
    void DoSetBitmapMargins(wxCoord x, wxCoord y) override;
    void DoSetBitmapPosition(wxDirection dir) override;
    bool DoGetAuthNeeded() const override;
    void DoSetAuthNeeded(bool show) override;
#if wxUSE_MARKUP
    bool DoSetLabelMarkup(const wxString& markup) override;
#endif // wxUSE_MARKUP
#if wxUSE_TOOLTIPS
    void DoSetToolTipText(const wxString& tip) override;
    void DoSetToolTip(wxToolTip *tip) override;
#endif // wxUSE_TOOLTIPS

private:
    // Return false when a synchronous XAML/host boundary retired this exact
    // owner+implementation transaction.
    bool UpdateWinUIContent(bool forceRender = true,
                            State forcedState = State_Max,
                            double requestedScale = 0.0);
    bool UpdateWinUIAppearance(bool forceRender = true);
    // Apply or clear the WinUI "Accent" button style used for the default button.
    bool ApplyDefaultStyle(bool on);
    // Lazily attach the pointer/focus handlers needed to swap per-state bitmaps;
    // only done when an interactive-state bitmap (current/pressed/focused) is set.
    void EnsureStateHandlers();
    bool HasInteractiveStateBitmap() const;
    wxBitmap GetBitmapForState(State which,
                               double requestedScale = 0.0) const;
    State GetCurrentBitmapState() const;
    wxBitmap GetAuthBitmap(double requestedScale = 0.0) const;
    void OnDPIChanged(wxDPIChangedEvent& event);

    std::unique_ptr<wxWinUIButtonImpl> m_winui;
    std::uint64_t m_bitmapRevision = 0;
    bool m_bitmapDestroying = false;
    wxBitmapBundle m_bitmaps[State_Max];
    wxSize m_bitmapMargins{ 0, 0 };
    wxDirection m_bitmapPosition = wxLEFT;
    bool m_authNeeded = false;
    bool m_isDefault = false;
#if wxUSE_MARKUP
    wxString m_markup;
#endif // wxUSE_MARKUP
#if wxUSE_TOOLTIPS
    wxString m_tooltipText;
#endif // wxUSE_TOOLTIPS

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxButton);
};

#endif // _WX_WINUI_BUTTON_H_
