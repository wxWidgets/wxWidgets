/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/timectrl.h
// Purpose:     wxWinUI wxTimePickerCtrl declaration (WinUI TimePicker)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_TIMECTRL_H_
#define _WX_WINUI_TIMECTRL_H_

#include <cstdint>
#include <memory>

class wxWinUITimePickerImpl;

class WXDLLIMPEXP_CORE wxTimePickerCtrl : public wxTimePickerCtrlBase
{
public:
    wxTimePickerCtrl();
    wxTimePickerCtrl(wxWindow *parent,
                     wxWindowID id,
                     const wxDateTime& dt = wxDefaultDateTime,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxTP_DEFAULT,
                     const wxValidator& validator = wxDefaultValidator,
                     const wxString& name = wxTimePickerCtrlNameStr);
    ~wxTimePickerCtrl() override;

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxDateTime& dt = wxDefaultDateTime,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTP_DEFAULT,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxTimePickerCtrlNameStr);

    void SetValue(const wxDateTime& dt) override;
    wxDateTime GetValue() const override;
    void SetMinSize(const wxSize& minSize) override;

    // Deterministic seams exercising the real TimePicker property and
    // SelectedTimeChanged delegate without synthesizing input.
    bool WinUISetPeerTimeForTesting(const wxDateTime& dt);
    bool WinUIGetPeerTimeForTesting(wxDateTime *value) const;
    wxString WinUIGetLocaleTimePatternForTesting() const;
    bool WinUIGetTimeFieldOrderForTesting(int *hour,
                                          int *minute,
                                          int *second,
                                          int *period) const;
    bool WinUIGetHourSpinBindingForTesting(
        std::uintptr_t *incrementIdentity,
        std::uintptr_t *decrementIdentity,
        std::uint64_t *generation = nullptr) const;
    using WinUIHourLoadedHookForTesting =
        void (*)(wxTimePickerCtrl *, void *);
    static void WinUISetHourLoadedHookForTesting(
        WinUIHourLoadedHookForTesting hook,
        void *data = nullptr);
    static wxString WinUISetLanguageForTesting(const wxString& language);

protected:
    wxSize DoGetBestSize() const override;
    void DoSetSize(int x, int y, int width, int height,
                   int sizeFlags) override;

    bool ApplyInitialSizeTransaction(const wxSize& size,
                                     std::uint64_t expectedRevision);
    void OnLayoutLoaded();
    bool ApplyToPeer();
    bool ApplyValueToPeer(const wxDateTime& value);
    bool OnPeerHourChanged(double oldValue,
                           double newValue,
                           int stepDirection);
    bool OnPeerTimeChanged();
    void ResolveHourSpinButtons(bool updateLayout = false);

    std::unique_ptr<wxWinUITimePickerImpl> m_winui;
    wxDateTime m_value;

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxTimePickerCtrl);
};

#endif // _WX_WINUI_TIMECTRL_H_
