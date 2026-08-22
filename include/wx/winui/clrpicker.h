/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/clrpicker.h
// Purpose:     wxWinUI colour picker widget (WinUI DropDownButton + ColorPicker)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_CLRPICKER_H_
#define _WX_WINUI_CLRPICKER_H_

#include <memory>

class wxWinUIColourButtonImpl;

// The colour "picker widget" embedded by wxColourPickerCtrl: a button showing
// the current colour that drops down a WinUI ColorPicker flyout.
class WXDLLIMPEXP_CORE wxWinUIColourButton : public wxControl,
                                             public wxColourPickerWidgetBase
{
public:
    wxWinUIColourButton();
    wxWinUIColourButton(wxWindow *parent,
                        wxWindowID id,
                        const wxColour& col = *wxBLACK,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = wxCLRBTN_DEFAULT_STYLE,
                        const wxValidator& validator = wxDefaultValidator,
                        const wxString& name = wxASCII_STR(wxColourPickerWidgetNameStr));
    ~wxWinUIColourButton() override;

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxColour& col = *wxBLACK,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxCLRBTN_DEFAULT_STYLE,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxColourPickerWidgetNameStr));

    // Implementation-only deterministic seams. The setter crosses the real
    // WinRT Color property boundary, so its ColorChanged callback and public
    // event routing are exercised without synthesizing mouse input.
    bool WinUISetPeerColourForTesting(const wxColour& colour);
    bool WinUIGetPeerStateForTesting(wxColour *colour,
                                     bool *alphaEnabled,
                                     wxString *label) const;
    bool WinUIDeliverClosedForTesting();
    static unsigned WinUIGetLiveCallbackStateCountForTesting();

protected:
    void UpdateColour() override;
    wxSize DoGetBestSize() const override;

    void ApplyColourToPeer();
    void SendColourEvent(wxEventType type);

    std::unique_ptr<wxWinUIColourButtonImpl> m_winui;

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxWinUIColourButton);
};

#endif // _WX_WINUI_CLRPICKER_H_
