/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/appearance.cpp
// Purpose:     Common wx-to-WinUI appearance helpers
// Author:      wxWidgets development team
// Created:     2026-07-26
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/control.h"
#include "private.h"
#include "wx/winui/private/appearance.h"
#ifdef WXWINUI_TEST_SUPPORT
#include "appearance-test-access.h"
#endif // WXWINUI_TEST_SUPPORT

#include <cstdint>
#include <cmath>
#include <limits>
#include <winrt/Microsoft.UI.Text.h>
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Text.h>

#include "wx/winui/private/themecolours.h"
#include "wx/image.h"
#include <algorithm>

#include <string>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;
namespace MUXAP = winrt::Microsoft::UI::Xaml::Automation::Peers;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXM = winrt::Microsoft::UI::Xaml::Media;

namespace
{

#ifdef WXWINUI_TEST_SUPPORT
wxWinUIAppearanceTestAccess::HighContrastOverride gs_highContrastOverride =
    wxWinUIAppearanceTestAccess::HighContrastOverride::System;
#endif // WXWINUI_TEST_SUPPORT

struct wxWinUIAccessibleElementPeer
    : MUXAP::FrameworkElementAutomationPeerT<
          wxWinUIAccessibleElementPeer>
{
    wxWinUIAccessibleElementPeer(
        const MUX::FrameworkElement& owner,
        MUXAP::AutomationControlType controlType,
        const winrt::hstring& className)
        : FrameworkElementAutomationPeerT(owner),
          m_controlType(controlType),
          m_className(className)
    {
    }

    MUXAP::AutomationControlType GetAutomationControlTypeCore() const
    {
        return m_controlType;
    }

    winrt::hstring GetClassNameCore() const
    {
        return m_className;
    }

private:
    MUXAP::AutomationControlType m_controlType;
    winrt::hstring m_className;
};

struct wxWinUIAccessibleGrid
    : MUXC::GridT<wxWinUIAccessibleGrid>
{
    wxWinUIAccessibleGrid(MUXAP::AutomationControlType controlType,
                          const winrt::hstring& className)
        : m_controlType(controlType),
          m_className(className)
    {
    }

    MUXAP::AutomationPeer OnCreateAutomationPeer()
    {
        // GridT is the implementation object, not its projected Grid. Query
        // the composed inner object for the FrameworkElement projection that
        // FrameworkElementAutomationPeer requires as its owner.
        const MUX::FrameworkElement owner =
            this->try_as<MUX::FrameworkElement>();
        return winrt::make<wxWinUIAccessibleElementPeer>(
            owner,
            m_controlType,
            m_className);
    }

private:
    MUXAP::AutomationControlType m_controlType;
    winrt::hstring m_className;
};

#ifdef WXWINUI_TEST_SUPPORT
template <typename T>
bool wxWinUIHasLocalValue(const T& object,
                          const MUX::DependencyProperty& property)
{
    return object &&
           object.ReadLocalValue(property) != MUX::DependencyProperty::UnsetValue();
}
#endif // WXWINUI_TEST_SUPPORT

winrt::Windows::UI::Text::FontWeight
wxWinUIFontWeight(const wxFont& font)
{
    winrt::Windows::UI::Text::FontWeight weight{};
    weight.Weight =
        static_cast<std::uint16_t>(font.GetNumericWeight());
    return weight;
}

winrt::Windows::UI::Text::FontStyle
wxWinUIFontStyle(const wxFont& font)
{
    switch ( font.GetStyle() )
    {
        case wxFONTSTYLE_ITALIC:
            return winrt::Windows::UI::Text::FontStyle::Italic;

        case wxFONTSTYLE_SLANT:
            return winrt::Windows::UI::Text::FontStyle::Oblique;

        case wxFONTSTYLE_NORMAL:
        default:
            return winrt::Windows::UI::Text::FontStyle::Normal;
    }
}

template <typename T>
void wxWinUIApplyFontImpl(const T& element,
                          const wxFont& font,
                          const MUX::DependencyProperty& familyProperty,
                          const MUX::DependencyProperty& sizeProperty,
                          const MUX::DependencyProperty& weightProperty,
                          const MUX::DependencyProperty& styleProperty)
{
    if ( !element )
        return;

    if ( !font.IsOk() )
    {
        element.ClearValue(familyProperty);
        element.ClearValue(sizeProperty);
        element.ClearValue(weightProperty);
        element.ClearValue(styleProperty);
        return;
    }

    // The default wx font names the text cut of the WinUI body font: leave
    // the family to XAML then, which picks the cut from the size itself.
    const wxString face = font.GetFaceName();
    if ( face.empty() || face == wxS("Segoe UI Variable Text") )
        element.ClearValue(familyProperty);
    else
        element.FontFamily(MUXM::FontFamily(wxWinUIToHString(face)));

    const double pointSize = font.GetFractionalPointSize();
    if ( pointSize > 0.0 )
        element.FontSize(pointSize * 96.0 / 72.0);
    else
        element.ClearValue(sizeProperty);

    element.FontWeight(wxWinUIFontWeight(font));
    element.FontStyle(wxWinUIFontStyle(font));
}

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUICaptureAccessibleValues(
    const MUX::UIElement& element,
    wxWinUIAppearanceSnapshot& snapshot)
{
    if ( !element )
        return;

    snapshot.automationName =
        wxString(MUXA::AutomationProperties::GetName(element).c_str());
    snapshot.accessKey =
        wxString(MUXA::AutomationProperties::GetAccessKey(element).c_str());
    snapshot.localizedControlType =
        wxString(
            MUXA::AutomationProperties::GetLocalizedControlType(element).c_str());

    const MUX::FrameworkElement frameworkElement =
        element.try_as<MUX::FrameworkElement>();
    if ( !frameworkElement )
        return;

    const MUXAP::AutomationPeer peer =
        MUXAP::FrameworkElementAutomationPeer::CreatePeerForElement(
            frameworkElement);
    if ( !peer )
        return;

    snapshot.hasAutomationPeer = true;
    snapshot.automationControlType =
        static_cast<int>(peer.GetAutomationControlType());
    snapshot.automationClassName =
        wxString(peer.GetClassName().c_str());
    snapshot.peerName = wxString(peer.GetName().c_str());
    snapshot.peerLocalizedControlType =
        wxString(peer.GetLocalizedControlType().c_str());
}
#endif // WXWINUI_TEST_SUPPORT

} // namespace

bool wxWinUIIsHighContrastActive()
{
#ifdef WXWINUI_TEST_SUPPORT
    switch ( gs_highContrastOverride )
    {
        case wxWinUIAppearanceTestAccess::HighContrastOverride::ForceOff:
            return false;

        case wxWinUIAppearanceTestAccess::HighContrastOverride::ForceOn:
            return true;

        case wxWinUIAppearanceTestAccess::HighContrastOverride::System:
            break;

        default:
            wxFAIL_MSG("unknown WinUI High Contrast override");
            return false;
    }
#endif // WXWINUI_TEST_SUPPORT

    HIGHCONTRAST highContrast{};
    highContrast.cbSize = sizeof(highContrast);
    if ( !::SystemParametersInfo(
            SPI_GETHIGHCONTRAST,
            sizeof(highContrast),
            &highContrast,
            0) )
    {
        // This helper is called from peer projection transactions.
        // Logging the failure here could invoke an application log
        // target and synchronously destroy the projecting control.
        // Conservatively relinquish default local brushes so WinUI's
        // ThemeResources remain authoritative; custom colours are
        // still kept by their per-state provenance.
        return true;
    }
    return (highContrast.dwFlags & HCF_HIGHCONTRASTON) != 0;
}

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUIAppearanceTestAccess::SetHighContrastOverride(
    wxWinUIAppearanceTestAccess::HighContrastOverride overrideValue)
{
    gs_highContrastOverride = overrideValue;
}

#endif // WXWINUI_TEST_SUPPORT

wxWinUILabelPresentation wxWinUIParseLabel(const wxString& label)
{
    wxWinUILabelPresentation result;
    const int mnemonic = wxControl::FindAccelIndex(label, &result.text);
    if ( mnemonic != wxNOT_FOUND &&
         static_cast<size_t>(mnemonic) < result.text.length() )
    {
        result.accessKey = result.text.Mid(mnemonic, 1);
    }

    return result;
}

MUXM::SolidColorBrush wxWinUICreateColourBrush(const wxColour& colour)
{
    return wxWinUIBrush(colour.Red(), colour.Green(), colour.Blue(),
                        colour.Alpha());
}

wxSize wxWinUIMeasureText(const wxWindow *window,
                          const wxString& text,
                          const wxFont& font)
{
    try
    {
        const float infinity = std::numeric_limits<float>::infinity();
        MUXC::TextBlock probe;
        probe.Text(wxWinUIToHString(text));
        wxWinUIApplyFont(probe, font);
        probe.Measure({ infinity, infinity });
        const auto desired = probe.DesiredSize();
        const wxSize dips(
            static_cast<int>(std::ceil(desired.Width)),
            static_cast<int>(std::ceil(desired.Height)));
        return window ? window->FromDIP(dips) : dips;
    }
    catch ( const winrt::hresult_error& )
    {
        return window ? window->GetTextExtent(text) : wxSize();
    }
}

void wxWinUIApplyFont(const MUXC::Control& control, const wxFont& font)
{
    wxWinUIApplyFontImpl(
        control, font,
        MUXC::Control::FontFamilyProperty(),
        MUXC::Control::FontSizeProperty(),
        MUXC::Control::FontWeightProperty(),
        MUXC::Control::FontStyleProperty());
}

void wxWinUIApplyFont(const MUXC::TextBlock& text, const wxFont& font)
{
    wxWinUIApplyFontImpl(
        text, font,
        MUXC::TextBlock::FontFamilyProperty(),
        MUXC::TextBlock::FontSizeProperty(),
        MUXC::TextBlock::FontWeightProperty(),
        MUXC::TextBlock::FontStyleProperty());

    if ( !text )
        return;
    if ( !font.IsOk() )
    {
        text.ClearValue(MUXC::TextBlock::TextDecorationsProperty());
        return;
    }

    using Decorations = winrt::Windows::UI::Text::TextDecorations;
    auto decorations = Decorations::None;
    if ( font.GetUnderlined() )
        decorations |= Decorations::Underline;
    if ( font.GetStrikethrough() )
        decorations |= Decorations::Strikethrough;
    text.TextDecorations(decorations);
}

void wxWinUIApplyForeground(const MUXC::Control& control,
                            const wxColour& colour)
{
    if ( !control )
        return;

    if ( colour.IsOk() )
        control.Foreground(wxWinUICreateColourBrush(colour));
    else
        control.ClearValue(MUXC::Control::ForegroundProperty());
}

void wxWinUIApplyForeground(const MUXC::TextBlock& text,
                            const wxColour& colour)
{
    if ( !text )
        return;

    if ( colour.IsOk() )
        text.Foreground(wxWinUICreateColourBrush(colour));
    else
        text.ClearValue(MUXC::TextBlock::ForegroundProperty());
}

void wxWinUIApplyBackground(const MUXC::Control& control,
                            const wxColour& colour)
{
    if ( !control )
        return;

    if ( colour.IsOk() )
        control.Background(wxWinUICreateColourBrush(colour));
    else
        control.ClearValue(MUXC::Control::BackgroundProperty());
}

void wxWinUIApplyBackground(const MUXC::Border& border,
                            const wxColour& colour)
{
    if ( !border )
        return;

    if ( colour.IsOk() )
        border.Background(wxWinUICreateColourBrush(colour));
    else
        border.ClearValue(MUXC::Border::BackgroundProperty());
}

void wxWinUIApplyAccessKey(const MUX::UIElement& element,
                           const wxString& label)
{
    if ( !element )
        return;

    const wxWinUILabelPresentation presentation = wxWinUIParseLabel(label);
    const auto accessKey = wxWinUIToHString(presentation.accessKey);

    MUXA::AutomationProperties::SetAccessKey(element, accessKey);
    element.AccessKey(accessKey);
}

void wxWinUIApplyAccessibleLabel(const MUX::UIElement& element,
                                 const wxString& label,
                                 const wxString& localizedControlType)
{
    if ( !element )
        return;

    const wxWinUILabelPresentation presentation = wxWinUIParseLabel(label);
    MUXA::AutomationProperties::SetName(
        element, wxWinUIToHString(presentation.text));

    if ( localizedControlType.empty() )
    {
        element.ClearValue(
            MUXA::AutomationProperties::LocalizedControlTypeProperty());
    }
    else
    {
        MUXA::AutomationProperties::SetLocalizedControlType(
            element, wxWinUIToHString(localizedControlType));
    }
}

MUXC::Grid wxWinUICreateAccessibleGrid(
    MUXAP::AutomationControlType controlType,
    const wxString& className)
{
    MUXC::Grid root =
        winrt::make<wxWinUIAccessibleGrid>(
            controlType, wxWinUIToHString(className));
    MUXA::AutomationProperties::SetAutomationControlType(
        root, controlType);
    MUXA::AutomationProperties::SetAccessibilityView(
        root, MUXAP::AccessibilityView::Control);
    return root;
}

MUXC::Border
wxWinUICreateThemeBrushBorder(const wxString& resourceKey,
                              wxWinUIThemeBrushProperty property)
{
    const wxString propertyName =
        property == wxWinUIThemeBrushProperty::BorderBrush
            ? "BorderBrush"
            : "Background";
    const wxString xaml = wxString::Format(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\" "
        "%s=\"{ThemeResource %s}\" />",
        propertyName, resourceKey);

    try
    {
        return MUX::Markup::XamlReader::Load(wxWinUIToHString(xaml))
            .as<MUXC::Border>();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI theme brush lookup", e);
        return nullptr;
    }
}

// ----------------------------------------------------------------------------
// the colours of the theme, for what wx paints itself
// ----------------------------------------------------------------------------

wxBitmap wxWinUICreateColourSwatch(const wxColour& colour, const wxSize& size,
                                   double scale)
{
#if wxUSE_IMAGE
    const int w = wxMax(1, static_cast<int>(std::lround(size.x * scale))),
              h = wxMax(1, static_cast<int>(std::lround(size.y * scale)));
    const double radius = 4 * scale,
                 line = wxMax(1.0, scale);

    // Its outline: the strong stroke of a control at half its strength.
    const wxColour stroke =
        wxWinUIGetThemeColour(wxWinUIThemeColour::ControlStrongStroke);
    const double strength = stroke.IsOk() ? stroke.Alpha() / 255.0 / 2 : 0;

    wxImage image(w, h);
    image.InitAlpha();
    for ( int y = 0; y < h; ++y )
    {
        for ( int x = 0; x < w; ++x )
        {
            // How far inside the rounded rectangle the middle of the pixel
            // is: negative outside of it.
            const double qx = std::fabs(x + 0.5 - w / 2.0) - (w / 2.0 - radius),
                         qy = std::fabs(y + 0.5 - h / 2.0) - (h / 2.0 - radius);
            const double inside = radius -
                std::hypot(std::max(qx, 0.0), std::max(qy, 0.0)) -
                std::min(std::max(qx, qy), 0.0);
            const double covered = std::clamp(inside + 0.5, 0.0, 1.0);
            const double edge = strength *
                std::clamp(line + 0.5 - inside, 0.0, 1.0);
            const auto mix = [edge](unsigned char under, unsigned char over)
            {
                return static_cast<unsigned char>(
                    std::lround(under * (1 - edge) + over * edge));
            };
            image.SetRGB(x, y,
                         mix(colour.Red(), stroke.Red()),
                         mix(colour.Green(), stroke.Green()),
                         mix(colour.Blue(), stroke.Blue()));
            image.SetAlpha(x, y, static_cast<unsigned char>(
                std::lround(255 * covered)));
        }
    }

    return wxBitmap(image, wxBITMAP_SCREEN_DEPTH, scale);
#else
    wxUnusedVar(colour);
    wxUnusedVar(size);
    wxUnusedVar(scale);
    return wxBitmap();
#endif
}

wxColour wxWinUIGetThemeColour(wxWinUIThemeColour which)
{
    // The name of the brush, and its value in the default themes of WinUI
    // (alpha, red, green, blue) for when XAML cannot be asked yet.
    struct Entry
    {
        const wchar_t *key;
        unsigned long dark;
        unsigned long light;
    };
    static const Entry entries[] =
    {
        { L"AccentFillColorDefaultBrush",       0xFF60CDFFul, 0xFF005FB8ul },
        { L"TextOnAccentFillColorPrimaryBrush", 0xFF000000ul, 0xFFFFFFFFul },
        { L"TextFillColorPrimaryBrush",         0xFFFFFFFFul, 0xE4000000ul },
        { L"TextFillColorSecondaryBrush",       0xC5FFFFFFul, 0x9E000000ul },
        { L"TextFillColorDisabledBrush",        0x5DFFFFFFul, 0x5C000000ul },
        { L"SubtleFillColorSecondaryBrush",     0x0FFFFFFFul, 0x09000000ul },
        { L"SubtleFillColorTertiaryBrush",      0x0AFFFFFFul, 0x06000000ul },
        { L"DividerStrokeColorDefaultBrush",    0x15FFFFFFul, 0x0F000000ul },
        { L"ControlStrokeColorDefaultBrush",    0x12FFFFFFul, 0x0F000000ul },
        { L"ControlFillColorDefaultBrush",      0x0FFFFFFFul, 0xB3FFFFFFul },
        { L"ControlFillColorSecondaryBrush",    0x15FFFFFFul, 0x80F9F9F9ul },
        { L"ControlStrongStrokeColorDefaultBrush",
                                                0x8BFFFFFFul, 0x72000000ul },
        { L"CardBackgroundFillColorDefaultBrush",
                                                0x0DFFFFFFul, 0xB3FFFFFFul },
        { L"SolidBackgroundFillColorBaseBrush", 0xFF202020ul, 0xFFF3F3F3ul },
        { L"FocusStrokeColorOuterBrush",        0xFFFFFFFFul, 0xE4000000ul },
        { L"SolidBackgroundFillColorQuarternaryBrush",
                                                0xFF2C2C2Cul, 0xFFFFFFFFul },
        { L"LayerFillColorAltBrush",            0x0DFFFFFFul, 0xFFFFFFFFul },
    };

    const std::size_t index = static_cast<std::size_t>(which);
    if ( index >= WXSIZEOF(entries) )
        return wxColour();

    // Asked for at every paint: kept for a second, which is also how long a
    // change of theme or of accent takes to be seen by what wx paints.
    struct Cached
    {
        wxColour colour;
        ULONGLONG tick = 0;
        bool dark = false;
    };
    static Cached cache[WXSIZEOF(entries)];

    const bool dark = wxWinUIIsDarkTheme();
    const ULONGLONG now = ::GetTickCount64();
    Cached& cached = cache[index];
    if ( cached.colour.IsOk() && cached.dark == dark &&
         now - cached.tick < 1000 )
    {
        return cached.colour;
    }

    const unsigned long value = dark ? entries[index].dark
                                     : entries[index].light;
    wxColour colour(static_cast<unsigned char>((value >> 16) & 0xFF),
                    static_cast<unsigned char>((value >> 8) & 0xFF),
                    static_cast<unsigned char>(value & 0xFF),
                    static_cast<unsigned char>((value >> 24) & 0xFF));
    try
    {
        if ( const auto application = MUX::Application::Current() )
        {
            const auto found = application.Resources().TryLookup(
                winrt::box_value(entries[index].key));
            if ( const auto brush =
                    found.try_as<MUXM::SolidColorBrush>() )
            {
                const auto c = brush.Color();
                colour = wxColour(c.R, c.G, c.B, c.A);
            }
        }
    }
    catch ( const winrt::hresult_error& )
    {
        // Not started yet, or closing: the value of the default theme.
    }

    cached.colour = colour;
    cached.tick = now;
    cached.dark = dark;
    return colour;
}

wxColour wxWinUIBlendOver(const wxColour& colour, const wxColour& background)
{
    if ( !colour.IsOk() )
        return background;
    if ( !background.IsOk() || colour.Alpha() == 255 )
    {
        return wxColour(colour.Red(), colour.Green(), colour.Blue());
    }

    const double alpha = colour.Alpha() / 255.0;
    const auto mix = [alpha](unsigned char above, unsigned char below)
    {
        return static_cast<unsigned char>(
            below + (above - below) * alpha + 0.5);
    };
    return wxColour(mix(colour.Red(), background.Red()),
                    mix(colour.Green(), background.Green()),
                    mix(colour.Blue(), background.Blue()));
}

wxColour wxWinUIGetAccentTint(const wxColour& background)
{
    const wxColour accent = wxWinUIGetThemeColour(wxWinUIThemeColour::Accent);
    return wxWinUIBlendOver(
        wxColour(accent.Red(), accent.Green(), accent.Blue(), 77), background);
}

wxColour wxWinUIGetGridLineColour(const wxColour& background)
{
    // The stroke of a control, as measured on the tables of WinUI: white
    // at 7 % in the dark theme, where the lines are lighter than the ground,
    // and black at 6 % in the light one.
    return wxWinUIBlendOver(
        wxWinUIGetThemeColour(wxWinUIThemeColour::ControlStroke), background);
}

wxColour wxWinUIGetTableHeaderColour(const wxColour& background)
{
    return wxWinUIBlendOver(
        wxWinUIGetThemeColour(wxWinUIThemeColour::SubtleFill), background);
}

void wxWinUIApplyTableCellStyle(const MUXC::Button& button, bool hover)
{
    if ( !button )
        return;

    // The template of a Button changes its ground and its border under the
    // pointer for brushes of its own, and rounds its corners. A cell has the
    // ground and the lines of its table whatever the pointer does.
    static const wchar_t before[] =
        L"<ControlTemplate"
        L" xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation'"
        L" xmlns:x='http://schemas.microsoft.com/winfx/2006/xaml'"
        L" TargetType='Button'>"
        L"<Grid Background='{TemplateBinding Background}'>"
        L"<Border x:Name='Hover' Opacity='0'"
        L" Background='{ThemeResource SubtleFillColorSecondaryBrush}'/>"
        L"<ContentPresenter x:Name='ContentPresenter'"
        L" Content='{TemplateBinding Content}'"
        L" Padding='{TemplateBinding Padding}'"
        L" Foreground='{TemplateBinding Foreground}'"
        L" HorizontalContentAlignment="
        L"'{TemplateBinding HorizontalContentAlignment}'"
        L" VerticalContentAlignment="
        L"'{TemplateBinding VerticalContentAlignment}'/>"
        L"<Border IsHitTestVisible='False'"
        L" BorderBrush='{TemplateBinding BorderBrush}'"
        L" BorderThickness='{TemplateBinding BorderThickness}'"
        L" CornerRadius='{TemplateBinding CornerRadius}'/>"
        L"<VisualStateManager.VisualStateGroups>"
        L"<VisualStateGroup x:Name='CommonStates'>"
        L"<VisualState x:Name='Normal'/>";
    static const wchar_t hovering[] =
        L"<VisualState x:Name='PointerOver'><VisualState.Setters>"
        L"<Setter Target='Hover.Opacity' Value='1'/>"
        L"</VisualState.Setters></VisualState>"
        L"<VisualState x:Name='Pressed'><VisualState.Setters>"
        L"<Setter Target='Hover.Opacity' Value='1'/>"
        L"</VisualState.Setters></VisualState>";
    static const wchar_t still[] =
        L"<VisualState x:Name='PointerOver'/>"
        L"<VisualState x:Name='Pressed'/>";
    static const wchar_t after[] =
        L"<VisualState x:Name='Disabled'><VisualState.Setters>"
        L"<Setter Target='ContentPresenter.Opacity' Value='0.4'/>"
        L"</VisualState.Setters></VisualState>"
        L"</VisualStateGroup>"
        L"</VisualStateManager.VisualStateGroups>"
        L"</Grid></ControlTemplate>";

    try
    {
        std::wstring markup = before;
        markup += hover ? hovering : still;
        markup += after;
        button.Template(
            MUX::Markup::XamlReader::Load(winrt::hstring(markup))
                .as<MUXC::ControlTemplate>());
        button.CornerRadius({ 0, 0, 0, 0 });
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI table cell style", e);
    }
}

namespace
{

// The states of a Button take their brushes from resources looked up by
// name: giving it these names again, for each theme, restyles it without
// another template. High contrast keeps the colours of the system.
void wxWinUISetQuietButtonResources(
    const MUXC::Button& button,
    const wchar_t *textKey,
    const wchar_t *borderKey,
    const wchar_t *restKey = L"SubtleFillColorTransparentBrush",
    const wchar_t *hoverKey = L"SubtleFillColorSecondaryBrush",
    const wchar_t *pressedKey = L"SubtleFillColorTertiaryBrush")
{
    std::wstring keys =
        L"<StaticResource x:Key='ButtonForegroundPointerOver'"
        L" ResourceKey='TextFillColorPrimaryBrush'/>"
        L"<StaticResource x:Key='ButtonForegroundDisabled'"
        L" ResourceKey='TextFillColorDisabledBrush'/>";

    const std::pair<const wchar_t *, const wchar_t *> grounds[] =
    {
        { L"ButtonBackground", restKey },
        { L"ButtonBackgroundPointerOver", hoverKey },
        { L"ButtonBackgroundPressed", pressedKey },
        { L"ButtonBackgroundDisabled", restKey },
    };
    for ( const auto& ground : grounds )
    {
        keys += L"<StaticResource x:Key='";
        keys += ground.first;
        keys += L"' ResourceKey='";
        keys += ground.second;
        keys += L"'/>";
    }

    for ( const wchar_t *name : { L"ButtonForeground",
                                  L"ButtonForegroundPressed" } )
    {
        keys += L"<StaticResource x:Key='";
        keys += name;
        keys += L"' ResourceKey='";
        keys += textKey;
        keys += L"'/>";
    }
    for ( const wchar_t *name : { L"ButtonBorderBrush",
                                  L"ButtonBorderBrushPointerOver",
                                  L"ButtonBorderBrushPressed",
                                  L"ButtonBorderBrushDisabled" } )
    {
        keys += L"<StaticResource x:Key='";
        keys += name;
        keys += L"' ResourceKey='";
        keys += borderKey;
        keys += L"'/>";
    }

    std::wstring markup =
        L"<ResourceDictionary"
        L" xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation'"
        L" xmlns:x='http://schemas.microsoft.com/winfx/2006/xaml'>"
        L"<ResourceDictionary.ThemeDictionaries>"
        L"<ResourceDictionary x:Key='Default'>";
    markup += keys;
    markup += L"</ResourceDictionary><ResourceDictionary x:Key='Light'>";
    markup += keys;
    markup += L"</ResourceDictionary>"
              L"<ResourceDictionary x:Key='HighContrast'/>"
              L"</ResourceDictionary.ThemeDictionaries>"
              L"</ResourceDictionary>";

    button.Resources(
        MUX::Markup::XamlReader::Load(winrt::hstring(markup))
            .as<MUX::ResourceDictionary>());
}

} // anonymous namespace

MUXC::ListBox wxWinUICreateStyledListBox()
{
    // The states of ListBoxItem, with the brushes ListViewItem uses.
    static const wchar_t markup[] =
        L"<ListBox"
        L" xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation'"
        L" xmlns:x='http://schemas.microsoft.com/winfx/2006/xaml'"
        L" Background='Transparent' BorderThickness='1' CornerRadius='4'"
        L" Padding='0,2,0,2'"
        L" BorderBrush='{ThemeResource ControlStrokeColorDefaultBrush}'>"
        L"<ListBox.Resources>"
        L"<Style TargetType='ListBoxItem'>"
        L"<Setter Property='Padding' Value='16,0,12,0'/>"
        L"<Setter Property='MinHeight' Value='36'/>"
        L"<Setter Property='HorizontalContentAlignment' Value='Stretch'/>"
        L"<Setter Property='VerticalContentAlignment' Value='Center'/>"
        L"<Setter Property='Template'><Setter.Value>"
        L"<ControlTemplate TargetType='ListBoxItem'>"
        L"<Grid Background='Transparent'>"
        L"<Border x:Name='Fill' Margin='4,2,4,2' CornerRadius='4'"
        L" Background='Transparent'/>"
        L"<Border x:Name='Pill' Width='3' Height='16' CornerRadius='1.5'"
        L" Margin='4,0,0,0' HorizontalAlignment='Left'"
        L" VerticalAlignment='Center' Opacity='0'"
        L" Background='{ThemeResource AccentFillColorDefaultBrush}'/>"
        L"<ContentPresenter x:Name='ContentPresenter'"
        L" Margin='{TemplateBinding Padding}'"
        L" Content='{TemplateBinding Content}'"
        L" ContentTemplate='{TemplateBinding ContentTemplate}'"
        L" HorizontalAlignment="
        L"'{TemplateBinding HorizontalContentAlignment}'"
        L" VerticalAlignment="
        L"'{TemplateBinding VerticalContentAlignment}'/>"
        L"<VisualStateManager.VisualStateGroups>"
        L"<VisualStateGroup x:Name='CommonStates'>"
        L"<VisualState x:Name='Normal'/>"
        L"<VisualState x:Name='PointerOver'><VisualState.Setters>"
        L"<Setter Target='Fill.Background'"
        L" Value='{ThemeResource SubtleFillColorSecondaryBrush}'/>"
        L"</VisualState.Setters></VisualState>"
        L"<VisualState x:Name='Pressed'><VisualState.Setters>"
        L"<Setter Target='Fill.Background'"
        L" Value='{ThemeResource SubtleFillColorTertiaryBrush}'/>"
        L"</VisualState.Setters></VisualState>"
        L"<VisualState x:Name='Disabled'><VisualState.Setters>"
        L"<Setter Target='ContentPresenter.Opacity' Value='0.4'/>"
        L"</VisualState.Setters></VisualState>"
        L"<VisualState x:Name='Selected'><VisualState.Setters>"
        L"<Setter Target='Fill.Background'"
        L" Value='{ThemeResource SubtleFillColorSecondaryBrush}'/>"
        L"<Setter Target='Pill.Opacity' Value='1'/>"
        L"</VisualState.Setters></VisualState>"
        L"<VisualState x:Name='SelectedUnfocused'><VisualState.Setters>"
        L"<Setter Target='Fill.Background'"
        L" Value='{ThemeResource SubtleFillColorSecondaryBrush}'/>"
        L"<Setter Target='Pill.Opacity' Value='1'/>"
        L"</VisualState.Setters></VisualState>"
        L"<VisualState x:Name='SelectedDisabled'><VisualState.Setters>"
        L"<Setter Target='Fill.Background'"
        L" Value='{ThemeResource SubtleFillColorSecondaryBrush}'/>"
        L"<Setter Target='ContentPresenter.Opacity' Value='0.4'/>"
        L"</VisualState.Setters></VisualState>"
        L"<VisualState x:Name='SelectedPointerOver'><VisualState.Setters>"
        L"<Setter Target='Fill.Background'"
        L" Value='{ThemeResource SubtleFillColorTertiaryBrush}'/>"
        L"<Setter Target='Pill.Opacity' Value='1'/>"
        L"</VisualState.Setters></VisualState>"
        L"<VisualState x:Name='SelectedPressed'><VisualState.Setters>"
        L"<Setter Target='Fill.Background'"
        L" Value='{ThemeResource SubtleFillColorTertiaryBrush}'/>"
        L"<Setter Target='Pill.Opacity' Value='1'/>"
        L"</VisualState.Setters></VisualState>"
        L"</VisualStateGroup>"
        L"</VisualStateManager.VisualStateGroups>"
        L"</Grid></ControlTemplate>"
        L"</Setter.Value></Setter>"
        L"</Style>"
        L"</ListBox.Resources>"
        L"</ListBox>";

    try
    {
        return MUX::Markup::XamlReader::Load(markup).as<MUXC::ListBox>();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI styled list box", e);
        return MUXC::ListBox();
    }
}

void wxWinUIApplyChromelessTextBoxStyle(const MUXC::Control& textBox)
{
    if ( !textBox )
        return;

    // The ground and the border of a TextBox come from resources looked up
    // by name in each of its states: made transparent, and of no thickness.
    static const wchar_t *const names[] =
    {
        L"TextControlBackground", L"TextControlBackgroundPointerOver",
        L"TextControlBackgroundFocused", L"TextControlBackgroundDisabled",
        L"TextControlBorderBrush", L"TextControlBorderBrushPointerOver",
        L"TextControlBorderBrushFocused", L"TextControlBorderBrushDisabled"
    };

    try
    {
        std::wstring keys;
        for ( const wchar_t *name : names )
        {
            keys += L"<StaticResource x:Key='";
            keys += name;
            keys += L"' ResourceKey='SubtleFillColorTransparentBrush'/>";
        }

        std::wstring markup =
            L"<ResourceDictionary"
            L" xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation'"
            L" xmlns:x='http://schemas.microsoft.com/winfx/2006/xaml'>"
            L"<ResourceDictionary.ThemeDictionaries>"
            L"<ResourceDictionary x:Key='Default'>";
        markup += keys;
        markup += L"</ResourceDictionary><ResourceDictionary x:Key='Light'>";
        markup += keys;
        markup += L"</ResourceDictionary>"
                  L"<ResourceDictionary x:Key='HighContrast'/>"
                  L"</ResourceDictionary.ThemeDictionaries>"
                  L"<Thickness x:Key='TextControlBorderThemeThickness'>0"
                  L"</Thickness>"
                  L"<Thickness x:Key='TextControlBorderThemeThicknessFocused'>0"
                  L"</Thickness>"
                  L"</ResourceDictionary>";

        // The values of the resting state too, set on the control itself:
        // the style gave them their brushes before these resources were
        // looked at.
        MUXM::SolidColorBrush none;
        none.Color(winrt::Windows::UI::Color{ 0, 0, 0, 0 });
        textBox.Background(none);
        textBox.BorderBrush(none);
        textBox.BorderThickness({ 0, 0, 0, 0 });
        textBox.MinHeight(0);

        textBox.Resources(
            MUX::Markup::XamlReader::Load(winrt::hstring(markup))
                .as<MUX::ResourceDictionary>());
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI chromeless text box style", e);
    }
}

void wxWinUIApplySideTabStyle(const MUXC::TabViewItem& item, int side)
{
    if ( !item )
        return;

    // Where the bar of the selected tab is, and its shape.
    const wchar_t * const bar =
        side == 2 ? L" Width='16' Height='3' HorizontalAlignment='Center'"
                    L" VerticalAlignment='Bottom' Margin='0,0,0,2'"
      : side == 1 ? L" Width='3' Height='16' HorizontalAlignment='Right'"
                    L" VerticalAlignment='Center' Margin='0,0,2,0'"
                  : L" Width='3' Height='16' HorizontalAlignment='Left'"
                    L" VerticalAlignment='Center' Margin='2,0,0,0'";

    static const wchar_t before[] =
        L"<ControlTemplate"
        L" xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation'"
        L" xmlns:x='http://schemas.microsoft.com/winfx/2006/xaml'"
        L" TargetType='TabViewItem'>"
        L"<Grid Background='Transparent'>"
        L"<Border x:Name='Fill' Margin='2' CornerRadius='4'"
        L" Background='Transparent'/>"
        L"<Border x:Name='Bar' CornerRadius='1.5' Opacity='0'"
        L" Background='{ThemeResource AccentFillColorDefaultBrush}'";
    static const wchar_t after[] =
        L"/>"
        L"<ContentPresenter x:Name='ContentPresenter'"
        L" Content='{TemplateBinding Content}'"
        L" Margin='{TemplateBinding Padding}'"
        L" HorizontalAlignment="
        L"'{TemplateBinding HorizontalContentAlignment}'"
        L" VerticalAlignment="
        L"'{TemplateBinding VerticalContentAlignment}'/>"
        L"<VisualStateManager.VisualStateGroups>"
        L"<VisualStateGroup x:Name='CommonStates'>"
        L"<VisualState x:Name='Normal'/>"
        L"<VisualState x:Name='PointerOver'><VisualState.Setters>"
        L"<Setter Target='Fill.Background'"
        L" Value='{ThemeResource SubtleFillColorSecondaryBrush}'/>"
        L"</VisualState.Setters></VisualState>"
        L"<VisualState x:Name='Pressed'><VisualState.Setters>"
        L"<Setter Target='Fill.Background'"
        L" Value='{ThemeResource SubtleFillColorTertiaryBrush}'/>"
        L"</VisualState.Setters></VisualState>"
        L"<VisualState x:Name='Selected'><VisualState.Setters>"
        L"<Setter Target='Fill.Background'"
        L" Value='{ThemeResource SubtleFillColorSecondaryBrush}'/>"
        L"<Setter Target='Bar.Opacity' Value='1'/>"
        L"</VisualState.Setters></VisualState>"
        L"<VisualState x:Name='PointerOverSelected'><VisualState.Setters>"
        L"<Setter Target='Fill.Background'"
        L" Value='{ThemeResource SubtleFillColorTertiaryBrush}'/>"
        L"<Setter Target='Bar.Opacity' Value='1'/>"
        L"</VisualState.Setters></VisualState>"
        L"<VisualState x:Name='PressedSelected'><VisualState.Setters>"
        L"<Setter Target='Fill.Background'"
        L" Value='{ThemeResource SubtleFillColorTertiaryBrush}'/>"
        L"<Setter Target='Bar.Opacity' Value='1'/>"
        L"</VisualState.Setters></VisualState>"
        L"</VisualStateGroup>"
        L"<VisualStateGroup x:Name='DisabledStates'>"
        L"<VisualState x:Name='Enabled'/>"
        L"<VisualState x:Name='Disabled'><VisualState.Setters>"
        L"<Setter Target='ContentPresenter.Opacity' Value='0.4'/>"
        L"</VisualState.Setters></VisualState>"
        L"</VisualStateGroup>"
        L"</VisualStateManager.VisualStateGroups>"
        L"</Grid></ControlTemplate>";

    try
    {
        std::wstring markup = before;
        markup += bar;
        markup += after;
        item.Template(
            MUX::Markup::XamlReader::Load(winrt::hstring(markup))
                .as<MUXC::ControlTemplate>());
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI side tab style", e);
    }
}

void wxWinUIApplySubtleButtonStyle(const MUXC::Button& button)
{
    if ( !button )
        return;

    try
    {
        wxWinUISetQuietButtonResources(
            button, L"TextFillColorPrimaryBrush",
            L"SubtleFillColorTransparentBrush");
        button.BorderThickness({ 0, 0, 0, 0 });
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI subtle button style", e);
    }
}

namespace
{

bool wxWinUIHeaderFontIsPlain(const wxFont& font)
{
    return !font.IsOk() || font.GetNumericWeight() <= wxFONTWEIGHT_NORMAL;
}

} // anonymous namespace

void wxWinUIApplyTableHeaderFont(const MUXC::Control& control,
                                 const wxFont& font)
{
    wxWinUIApplyFont(control, font);
    if ( !control || !wxWinUIHeaderFontIsPlain(font) )
        return;

    try
    {
        control.FontWeight(winrt::Microsoft::UI::Text::FontWeights::SemiBold());
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI table header font", e);
    }
}

void wxWinUIApplyTableHeaderFont(const MUXC::TextBlock& text,
                                 const wxFont& font)
{
    wxWinUIApplyFont(text, font);
    if ( !text || !wxWinUIHeaderFontIsPlain(font) )
        return;

    try
    {
        text.FontWeight(winrt::Microsoft::UI::Text::FontWeights::SemiBold());
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI table header font", e);
    }
}

MUXC::Border wxWinUICreateTableHeaderBand(double topRadius,
                                          double bottomRadius)
{
    try
    {
        const MUXC::Border band = MUX::Markup::XamlReader::Load(
            L"<Border"
            L" xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation'"
            L" IsHitTestVisible='False' BorderThickness='0,0,0,1'"
            L" Background='{ThemeResource SubtleFillColorSecondaryBrush}'"
            L" BorderBrush='{ThemeResource ControlStrokeColorDefaultBrush}'/>")
                .as<MUXC::Border>();
        band.CornerRadius({ topRadius, topRadius, bottomRadius, bottomRadius });
        return band;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI table header band", e);
        return nullptr;
    }
}

void wxWinUIApplyTableCellValueStyle(const MUXC::Control& control)
{
    if ( !control )
        return;

    // The grounds and the strokes of the controls which show a value, by
    // the names their templates look them up under.
    static const wchar_t * const cleared[] =
    {
        L"TextControlBackground", L"TextControlBackgroundPointerOver",
        L"TextControlBackgroundDisabled",
        L"TextControlBorderBrush", L"TextControlBorderBrushPointerOver",
        L"TextControlBorderBrushDisabled",
        L"ComboBoxBackground", L"ComboBoxBackgroundPointerOver",
        L"ComboBoxBackgroundPressed", L"ComboBoxBackgroundDisabled",
        L"ComboBoxBorderBrush", L"ComboBoxBorderBrushPointerOver",
        L"ComboBoxBorderBrushPressed", L"ComboBoxBorderBrushDisabled",
        L"CalendarDatePickerBackground",
        L"CalendarDatePickerBackgroundPointerOver",
        L"CalendarDatePickerBackgroundPressed",
        L"CalendarDatePickerBackgroundDisabled",
        L"CalendarDatePickerBorderBrush",
        L"CalendarDatePickerBorderBrushPointerOver",
        L"CalendarDatePickerBorderBrushPressed",
        L"CalendarDatePickerBorderBrushDisabled",
    };

    std::wstring keys;
    for ( const wchar_t *name : cleared )
    {
        keys += L"<StaticResource x:Key='";
        keys += name;
        keys += L"' ResourceKey='SubtleFillColorTransparentBrush'/>";
    }

    std::wstring markup =
        L"<ResourceDictionary"
        L" xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation'"
        L" xmlns:x='http://schemas.microsoft.com/winfx/2006/xaml'>"
        L"<ResourceDictionary.ThemeDictionaries>"
        L"<ResourceDictionary x:Key='Default'>";
    markup += keys;
    markup += L"</ResourceDictionary><ResourceDictionary x:Key='Light'>";
    markup += keys;
    markup += L"</ResourceDictionary>"
              L"<ResourceDictionary x:Key='HighContrast'/>"
              L"</ResourceDictionary.ThemeDictionaries>"
              L"</ResourceDictionary>";

    try
    {
        control.Resources(
            MUX::Markup::XamlReader::Load(winrt::hstring(markup))
                .as<MUX::ResourceDictionary>());
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI table cell value style", e);
    }
}

MUXC::Border wxWinUICreateTableCellBorder(bool rightLine, bool bottomLine)
{
    MUXC::Border border = wxWinUICreateThemeBrushBorder(
        "ControlStrokeColorDefaultBrush",
        wxWinUIThemeBrushProperty::BorderBrush);
    if ( !border )
        border = MUXC::Border();

    try
    {
        border.BorderThickness({ 0, 0, rightLine ? 1.0 : 0.0,
                                 bottomLine ? 1.0 : 0.0 });
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI table cell border", e);
    }

    return border;
}

void wxWinUIApplyTableHeaderStyle(const MUXC::Button& button)
{
    if ( !button )
        return;

    try
    {
        // As the column headers of WinUI.TableView, the reference for the
        // tables of this port: the text of the theme in semi-bold over the
        // subtle fill of the header row, with the line of the table at the
        // right.
        // The fill is the band under the buttons (see
        // wxWinUICreateTableHeaderBand()): a button adds its own only under
        // the pointer.
        wxWinUISetQuietButtonResources(
            button, L"TextFillColorPrimaryBrush",
            L"ControlStrokeColorDefaultBrush",
            L"SubtleFillColorTransparentBrush",
            L"SubtleFillColorSecondaryBrush",
            L"SubtleFillColorTertiaryBrush");
        button.FontWeight(winrt::Microsoft::UI::Text::FontWeights::SemiBold());

        // The line between two columns; the one under the header is the
        // one of the band.
        button.BorderThickness({ 0, 0, 1, 0 });
        button.CornerRadius({ 0, 0, 0, 0 });
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI table header style", e);
    }
}

#ifdef WXWINUI_TEST_SUPPORT

wxWinUIAppearanceSnapshot wxWinUIAppearanceTestAccess::Capture(
    const MUXC::TextBlock& text,
    const MUXC::Border& background,
    const MUX::UIElement& accessibleElement)
{
    wxWinUIAppearanceSnapshot snapshot;
    if ( text )
    {
        snapshot.hasFontFamily =
            wxWinUIHasLocalValue(text, MUXC::TextBlock::FontFamilyProperty());
        snapshot.hasFontSize =
            wxWinUIHasLocalValue(text, MUXC::TextBlock::FontSizeProperty());
        snapshot.hasFontWeight =
            wxWinUIHasLocalValue(text, MUXC::TextBlock::FontWeightProperty());
        snapshot.hasFontStyle =
            wxWinUIHasLocalValue(text, MUXC::TextBlock::FontStyleProperty());
        snapshot.hasForeground =
            wxWinUIHasLocalValue(text, MUXC::TextBlock::ForegroundProperty());
    }
    if ( background )
    {
        snapshot.hasBackground =
            wxWinUIHasLocalValue(background, MUXC::Border::BackgroundProperty());
    }
    wxWinUICaptureAccessibleValues(accessibleElement, snapshot);
    return snapshot;
}

wxWinUIAppearanceSnapshot wxWinUIAppearanceTestAccess::Capture(
    const MUXC::Control& control,
    const MUX::UIElement& accessibleElement)
{
    wxWinUIAppearanceSnapshot snapshot;
    if ( control )
    {
        snapshot.hasFontFamily =
            wxWinUIHasLocalValue(control, MUXC::Control::FontFamilyProperty());
        snapshot.hasFontSize =
            wxWinUIHasLocalValue(control, MUXC::Control::FontSizeProperty());
        snapshot.hasFontWeight =
            wxWinUIHasLocalValue(control, MUXC::Control::FontWeightProperty());
        snapshot.hasFontStyle =
            wxWinUIHasLocalValue(control, MUXC::Control::FontStyleProperty());
        snapshot.hasForeground =
            wxWinUIHasLocalValue(control, MUXC::Control::ForegroundProperty());
        snapshot.hasBackground =
            wxWinUIHasLocalValue(control, MUXC::Control::BackgroundProperty());
    }
    wxWinUICaptureAccessibleValues(accessibleElement, snapshot);
    return snapshot;
}

#endif // WXWINUI_TEST_SUPPORT
