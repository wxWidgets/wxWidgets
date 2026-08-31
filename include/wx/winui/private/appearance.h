/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/private/appearance.h
// Purpose:     Common wx-to-WinUI appearance helpers
// Author:      wxWidgets development team
// Created:     2026-07-26
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_PRIVATE_APPEARANCE_H_
#define _WX_WINUI_PRIVATE_APPEARANCE_H_

#include "wx/colour.h"
#include "wx/defs.h"
#include "wx/font.h"
#include "wx/gdicmn.h"
#include "wx/string.h"

// Windows headers may define this function-like macro before a consumer
// includes us. It corrupts a C++/WinRT metadata method with the same name.
#ifdef GetCurrentTime
    #undef GetCurrentTime
#endif

#ifdef min
    #undef min
#endif
#ifdef max
    #undef max
#endif

#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>

// The native label is deliberately split once at the port boundary.  XAML's
// AccessKey and UIA AccessKey properties both expect the key without wx's '&'
// marker, while the visible and accessible names must contain the unescaped
// text.
struct WXDLLIMPEXP_CORE wxWinUILabelPresentation
{
    wxString text;
    wxString accessKey;
};

// A compact deterministic observation used by the WinUI-only tests.  The
// "has" flags report local DependencyProperty ownership, which is what matters
// for wxNullFont/wxNullColour: an inherited theme value is not a reset if a
// stale local value is still present.
struct WXDLLIMPEXP_CORE wxWinUIAppearanceSnapshot
{
    bool hasFontFamily = false;
    bool hasFontSize = false;
    bool hasFontWeight = false;
    bool hasFontStyle = false;
    bool hasForeground = false;
    bool hasBackground = false;
    wxString automationName;
    wxString accessKey;
    wxString localizedControlType;
    bool hasAutomationPeer = false;
    int automationControlType = -1;
    wxString automationClassName;
    wxString peerName;
    wxString peerLocalizedControlType;
};

enum class wxWinUIHighContrastOverrideForTesting
{
    System,
    ForceOff,
    ForceOn
};

class wxWindow;

// Centralize the native High Contrast policy so controls can relinquish local
// brushes to WinUI ThemeResources. The override exists only for deterministic
// tests and must be restored to System by their scope guard.
WXDLLIMPEXP_CORE bool wxWinUIIsHighContrastActive();
WXDLLIMPEXP_CORE void
wxWinUISetHighContrastOverrideForTesting(
    wxWinUIHighContrastOverrideForTesting overrideValue);

WXDLLIMPEXP_CORE wxWinUILabelPresentation
wxWinUIParseLabel(const wxString& label);

WXDLLIMPEXP_CORE
winrt::Microsoft::UI::Xaml::Media::SolidColorBrush
wxWinUICreateColourBrush(const wxColour& colour);

// Measure a single-line label with the actual WinUI typography. XAML reports
// DesiredSize in DIPs; this helper converts it to the wx window coordinate
// space so callers never mix DIPs with physical best-size padding.
WXDLLIMPEXP_CORE wxSize
wxWinUIMeasureText(const wxWindow *window,
                   const wxString& text,
                   const wxFont& font);

WXDLLIMPEXP_CORE void
wxWinUIApplyFont(
    const winrt::Microsoft::UI::Xaml::Controls::Control& control,
    const wxFont& font);

WXDLLIMPEXP_CORE void
wxWinUIApplyFont(
    const winrt::Microsoft::UI::Xaml::Controls::TextBlock& text,
    const wxFont& font);

WXDLLIMPEXP_CORE void
wxWinUIApplyForeground(
    const winrt::Microsoft::UI::Xaml::Controls::Control& control,
    const wxColour& colour);

WXDLLIMPEXP_CORE void
wxWinUIApplyForeground(
    const winrt::Microsoft::UI::Xaml::Controls::TextBlock& text,
    const wxColour& colour);

WXDLLIMPEXP_CORE void
wxWinUIApplyBackground(
    const winrt::Microsoft::UI::Xaml::Controls::Control& control,
    const wxColour& colour);

WXDLLIMPEXP_CORE void
wxWinUIApplyBackground(
    const winrt::Microsoft::UI::Xaml::Controls::Border& border,
    const wxColour& colour);

// Set the visual and UIA access-key properties only. Slot synchronization is
// the sole authority for AutomationProperties.Name on hosted controls, so
// normal control implementations should use this function.
WXDLLIMPEXP_CORE void
wxWinUIApplyAccessKey(
    const winrt::Microsoft::UI::Xaml::UIElement& element,
    const wxString& label);

// Explicitly author a UIA name for elements which are not slot-controlled
// (dialog internals and test probes). Passing an empty localized type clears
// the previous local type. Do not use this on an ordinary hosted control.
WXDLLIMPEXP_CORE void
wxWinUIApplyAccessibleLabel(
    const winrt::Microsoft::UI::Xaml::UIElement& element,
    const wxString& label,
    const wxString& localizedControlType = wxString());

// Grid, Border, StackPanel and ContentControl don't create an automation peer
// of their own. Composite wx controls must therefore use this root instead of
// relying on AutomationProperties alone: the latter decorate an existing peer
// but never create one. The returned Grid remains a normal layout container
// while exposing the requested stable UIA role and class name.
WXDLLIMPEXP_CORE
winrt::Microsoft::UI::Xaml::Controls::Grid
wxWinUICreateAccessibleGrid(
    winrt::Microsoft::UI::Xaml::Automation::Peers::AutomationControlType
        controlType,
    const wxString& className);

enum class wxWinUIThemeBrushProperty
{
    BorderBrush,
    Background
};

// Return a Border whose requested property still contains the ThemeResource
// expression. Returning the Brush value itself would detach the expression
// from its target and freeze the current theme. Failure intentionally returns
// null instead of substituting a fixed light/dark colour.
WXDLLIMPEXP_CORE
winrt::Microsoft::UI::Xaml::Controls::Border
wxWinUICreateThemeBrushBorder(
    const wxString& resourceKey,
    wxWinUIThemeBrushProperty property);

WXDLLIMPEXP_CORE wxWinUIAppearanceSnapshot
wxWinUICaptureAppearance(
    const winrt::Microsoft::UI::Xaml::Controls::TextBlock& text,
    const winrt::Microsoft::UI::Xaml::Controls::Border& background,
    const winrt::Microsoft::UI::Xaml::UIElement& accessibleElement);

WXDLLIMPEXP_CORE wxWinUIAppearanceSnapshot
wxWinUICaptureAppearance(
    const winrt::Microsoft::UI::Xaml::Controls::Control& control,
    const winrt::Microsoft::UI::Xaml::UIElement& accessibleElement);

#endif // _WX_WINUI_PRIVATE_APPEARANCE_H_
