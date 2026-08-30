/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/tglbtn.h
// Purpose:     wxWinUI wxToggleButton declaration
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_TGLBTN_H_
#define _WX_WINUI_TGLBTN_H_

#include "wx/bmpbndl.h"

#include <cstdint>
#include <memory>

class wxWinUIToggleButtonImpl;
class wxDPIChangedEvent;

class WXDLLIMPEXP_CORE wxToggleButton : public wxToggleButtonBase
{
public:
    wxToggleButton();
    wxToggleButton(wxWindow *parent,
                   wxWindowID id,
                   const wxString& label,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxASCII_STR(wxCheckBoxNameStr));
    ~wxToggleButton() override;

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxCheckBoxNameStr));

    void SetValue(bool value) override;
    bool GetValue() const override;

    void SetLabel(const wxString& label) override;
    void Command(wxCommandEvent& event) override;
    bool SetFont(const wxFont& font) override;
    bool SetForegroundColour(const wxColour& colour) override;
    bool SetBackgroundColour(const wxColour& colour) override;
protected:
    bool MSWOnEffectiveLayoutDirectionChanged() override;
    wxBorder GetDefaultBorder() const override { return wxBORDER_NONE; }
    wxSize DoGetBestSize() const override;
    void DoEnable(bool enable) override;

    void SendToggleEvent();
    virtual bool UpdateWinUIContent(bool forceRender = true);

    std::unique_ptr<wxWinUIToggleButtonImpl> m_winui;
    bool m_state = false;

private:
    bool UpdateWinUIAppearance(bool forceRender = true);
    void OnDPIChanged(wxDPIChangedEvent& event);

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxToggleButton);
};

//-----------------------------------------------------------------------------
// wxBitmapToggleButton
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxBitmapToggleButton : public wxToggleButton
{
public:
    wxBitmapToggleButton() = default;
    ~wxBitmapToggleButton() override;

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

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxBitmapBundle& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxCheckBoxNameStr));

    void SetLabel(const wxString& label) override
        { wxToggleButton::SetLabel(label); }

protected:
    wxSize DoGetBestSize() const override;
    bool UpdateWinUIContent(bool forceRender = true) override;
    wxBitmap DoGetBitmap(State which) const override;
    void DoSetBitmap(const wxBitmapBundle& bitmap, State which) override;
    wxSize DoGetBitmapMargins() const override;
    void DoSetBitmapMargins(wxCoord x, wxCoord y) override;
    void DoSetBitmapPosition(wxDirection dir) override;

private:
    friend class wxWinUIButtonTestAccess;

    bool UpdateWinUIBitmapContent(bool forceRender,
                                  State forcedState,
                                  double requestedScale);
    wxBitmap GetBitmapForState(State which,
                               double requestedScale = 0.0) const;
    State GetCurrentBitmapState() const;

    // This revision is independent of the XAML peer: public bitmap accessors
    // are valid before Create() and must detect reentrant bundle replacement.
    std::uint64_t m_bitmapRevision = 0;
    // wxWeakRef remains valid until the wxWindow base destructor. Mark the
    // earlier derived-member teardown explicitly so bundle destructor reentry
    // cannot observe partially destroyed bitmap state.
    bool m_bitmapDestroying = false;
    // The lifetime sentinels above must outlive these application-owned
    // bundles, whose destructors are allowed to reenter the control.
    wxBitmapBundle m_bitmaps[State_Max];
    wxSize m_bitmapMargins{ 0, 0 };
    wxDirection m_bitmapPosition = wxLEFT;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxBitmapToggleButton);
};

#endif // _WX_WINUI_TGLBTN_H_
