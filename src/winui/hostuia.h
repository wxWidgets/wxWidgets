/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/hostuia.h
// Purpose:     Intracore accessibility projection and observer state
// Author:      wxWidgets development team
// Created:     2026-08-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_HOSTUIA_H_
#define _WX_WINUI_HOSTUIA_H_

#include "private.h"

class wxWinUISlotLifetime;

// Owned only by the existing slot and its revocable callbacks. The complete
// type is shared with projection code to suppress observer reentrancy; no
// registry, duplicate slot identity or installed API is introduced here.
class wxWinUIAutomationNameStylePropertyState
{
public:
    wxWinUIAutomationNameStylePropertyState();
    ~wxWinUIAutomationNameStylePropertyState();

    std::weak_ptr<wxWinUISlotLifetime> lifetime;
    winrt::Windows::Foundation::IInspectable localStyleSource{ nullptr };
    bool active = true;
    bool inCallback = false;
    bool localStyleSourceInitialized = false;
    bool localStyleSourceIsSet = false;
};

wxString wxWinUIGetAutomationName(wxWindow *window);
bool wxWinUIStyleAuthorsAutomationName(
    const winrt::Microsoft::UI::Xaml::UIElement& content,
    bool inspectImplicitStyle,
    const winrt::Microsoft::UI::Xaml::ResourceDictionary& slotResources =
        nullptr);
bool wxWinUITryGetLocalStringValue(
    const winrt::Windows::Foundation::IInspectable& value,
    wxString *result);
bool wxWinUIHasBinding(
    const winrt::Microsoft::UI::Xaml::UIElement& content,
    const winrt::Microsoft::UI::Xaml::DependencyProperty& property);
bool wxWinUIUpdateLocalStyleSourceSnapshot(
    const std::shared_ptr<wxWinUIAutomationNameStylePropertyState>& state,
    const winrt::Microsoft::UI::Xaml::FrameworkElement& framework);

// Consume the existing host fault phase without duplicating its owner/mask.
void wxWinUIMaybeInjectAutomationNameContentFault(unsigned phaseBit);

#endif // _WX_WINUI_HOSTUIA_H_
