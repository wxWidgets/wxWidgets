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
class wxWinUISlot;
class wxToolTip;

// A synchronous, non-owning view of the snapshots already on SyncSlot's
// stack. It neither reads live wx/XAML state nor copies owning values.
// Never retain it or send it to a deferred callback: all references end
// with this SyncSlot invocation. The mutable Name revision is shared so
// publication and every later geometry revalidation see the same epoch.
struct wxWinUISlotSyncTransaction
{
    const std::shared_ptr<wxWinUISlotLifetime>& state;
    wxWindow * const& w;
    const winrt::Microsoft::UI::Xaml::UIElement& content;
    const winrt::Microsoft::UI::Xaml::UIElement& semanticTarget;
    const unsigned long long& contentGeneration;
    const unsigned long long& syncEpoch;
    unsigned long long& automationNameStateRevision;
    const unsigned long long& automationNameStyleRevision;
    const unsigned long long& automationNameImplicitStyleRevision;
    const unsigned long long& clipRevision;
    const unsigned long long& automationNameOverrideRevision;
#if wxUSE_TOOLTIPS
    const bool& toolTipPolicyEnabled;
    wxToolTip * const& tip;
    const wxString& toolTipText;
    const int& toolTipMaximumWidth;
    const int& toolTipDPI;
#endif
    wxWinUISlot& slot;
    const bool& previousHelpTextSynced;
    const wxString& previousAppliedHelpText;
    const bool& helpTextPresent;
    const wxString& helpText;
    const bool& wxHelpTextChanged;
    bool& nextHelpTextOwned;
    wxString& nextAppliedHelpText;
    winrt::Windows::Foundation::IInspectable& nextOriginalHelpTextLocalValue;
    const bool& previousUIANameSynced;
    const unsigned long long&
        previousAutomationNameImplicitStyleInspectedRevision;
    const bool& hasApplicationAuthoredXaml;
    const bool& previousUIANameRelinquished;
    const bool& previousUIANameOwned;
    const bool& previousUIANameAppOwned;
    const bool& previousUIANameSuppressedByLabeledBy;
    const wxString& previousUIAName;
    const wxString& previousAppliedUIAName;
    const wxString& uiaLabel;
    bool& nextUIANameOwned;
    bool& nextUIANameAppOwned;
    bool& nextUIANameRelinquished;
    bool& nextUIANameSuppressedByLabeledBy;
    wxString& nextAppliedUIAName;
    winrt::Windows::Foundation::IInspectable& nextOriginalUIANameLocalValue;
};

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
#ifdef WXWINUI_TEST_SUPPORT
void wxWinUIMaybeInjectAutomationNameContentFault(unsigned phaseBit);
#endif // WXWINUI_TEST_SUPPORT

#endif // _WX_WINUI_HOSTUIA_H_
