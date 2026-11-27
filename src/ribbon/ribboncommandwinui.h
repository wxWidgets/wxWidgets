/////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/ribboncommandwinui.h
// Purpose:     Shared native visuals for wxWinUI ribbon commands
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RIBBON_COMMAND_WINUI_H_
#define _WX_RIBBON_COMMAND_WINUI_H_

#include "wx/bitmap.h"
#include "wx/ribbon/art.h"
#include "../winui/private.h"
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <limits>

// This source-private factory owns only visual construction. Each command
// adapter retains its own model, callback identity and publication transaction.
// In particular, no event is emitted while applying initial toggle state.
//
// Each kind of command is the WinUI control made for it:
//
// - a plain command is a Button, a toggle a ToggleButton;
// - a command which only opens a menu (wxRIBBON_BUTTON_DROPDOWN) is a
//   DropDownButton, one control with its chevron;
// - a command with an action and a menu (wxRIBBON_BUTTON_HYBRID) is a
//   SplitButton: its first part is the action, its second part the menu.
//
// All are given the subtle look of the commands of a command bar (no fill
// nor border at rest) through the resources of their own styles, and all
// hold the icon and the label of the command as their content: they measure
// to what they show, whatever the length of the label. An AppBarButton does
// not, outside a CommandBar -- it is as wide as its minimum and cuts its
// label -- which is why it is only the last resort here.
struct wxRibbonWinUICommandSpec
{
    wxRibbonButtonKind kind;
    wxString label;
    wxBitmap bitmap;
    bool enabled;
    bool checked;
    double iconSize;
    double primaryHeight;
    double minWidth;
    winrt::Microsoft::UI::Xaml::Controls::CommandBarLabelPosition labelPosition;
};

struct wxRibbonWinUICommandVisual
{
    winrt::Microsoft::UI::Xaml::Controls::Grid root{nullptr};
    winrt::Microsoft::UI::Xaml::Controls::Control primary{nullptr};
    // The part of a hybrid command which opens its menu.
    winrt::Microsoft::UI::Xaml::Controls::Button arrow{nullptr};
    // Set for a hybrid command shown by a SplitButton, which is then also
    // "primary": its Click is the action of the command.
    winrt::Microsoft::UI::Xaml::Controls::SplitButton split{nullptr};
};

namespace wxRibbonWinUICommandDetail
{

// A control of the given type whose style resources with the given prefix
// are those of a subtle button, in every theme but the high contrast ones.
inline winrt::Windows::Foundation::IInspectable
LoadSubtle(const wchar_t *type, const wchar_t *prefix)
{
    std::wstring keys;
    const auto add = [&](const wchar_t *state, const wchar_t *brush)
    {
        keys += L"<StaticResource x:Key='";
        keys += prefix;
        keys += state;
        keys += L"' ResourceKey='";
        keys += brush;
        keys += L"'/>";
    };
    add(L"Background", L"SubtleFillColorTransparentBrush");
    add(L"BackgroundPointerOver", L"SubtleFillColorSecondaryBrush");
    add(L"BackgroundPressed", L"SubtleFillColorTertiaryBrush");
    add(L"BackgroundDisabled", L"SubtleFillColorTransparentBrush");
    add(L"BorderBrush", L"SubtleFillColorTransparentBrush");
    add(L"BorderBrushPointerOver", L"SubtleFillColorTransparentBrush");
    add(L"BorderBrushPressed", L"SubtleFillColorTransparentBrush");
    add(L"BorderBrushDisabled", L"SubtleFillColorTransparentBrush");

    std::wstring markup = L"<";
    markup += type;
    markup +=
        L" xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation'"
        L" xmlns:x='http://schemas.microsoft.com/winfx/2006/xaml'><";
    markup += type;
    markup += L".Resources><ResourceDictionary>"
              L"<ResourceDictionary.ThemeDictionaries>"
              L"<ResourceDictionary x:Key='Default'>";
    markup += keys;
    markup += L"</ResourceDictionary><ResourceDictionary x:Key='Light'>";
    markup += keys;
    markup += L"</ResourceDictionary>"
              L"<ResourceDictionary x:Key='HighContrast'/>"
              L"</ResourceDictionary.ThemeDictionaries>"
              L"</ResourceDictionary></";
    markup += type;
    markup += L".Resources></";
    markup += type;
    markup += L">";

    return winrt::Microsoft::UI::Xaml::Markup::XamlReader::Load(markup);
}

// The icon and the label of a command, for the controls which take content
// rather than an icon and a label of their own.
inline winrt::Microsoft::UI::Xaml::UIElement
MakeContent(const wxRibbonWinUICommandSpec& spec)
{
    namespace Xaml = winrt::Microsoft::UI::Xaml;
    namespace Controls = Xaml::Controls;

    const bool large = spec.iconSize > 16.0;
    const bool labelled =
        spec.labelPosition != Controls::CommandBarLabelPosition::Collapsed &&
        !spec.label.empty();

    Controls::StackPanel panel;
    panel.Orientation(large ? Controls::Orientation::Vertical
                            : Controls::Orientation::Horizontal);
    panel.Spacing(large ? 2.0 : 8.0);
    panel.HorizontalAlignment(Xaml::HorizontalAlignment::Center);
    panel.VerticalAlignment(Xaml::VerticalAlignment::Center);

    if ( spec.bitmap.IsOk() )
    {
        Controls::Image image;
        image.Source(wxWinUIWriteableBitmapFromBitmap(spec.bitmap));
        image.Width(spec.iconSize);
        image.Height(spec.iconSize);
        panel.Children().Append(image);
    }
    if ( labelled )
    {
        Controls::TextBlock text;
        text.Text(wxWinUIToHString(spec.label));
        text.FontSize(12.0);
        text.TextAlignment(Xaml::TextAlignment::Center);
        text.VerticalAlignment(Xaml::VerticalAlignment::Center);
        if ( large )
        {
            // Under a large icon a long label takes two lines, as in the
            // ribbon of Office, rather than making the command very wide.
            text.TextWrapping(Xaml::TextWrapping::Wrap);
            text.MaxLines(2);
            text.MaxWidth(84.0);
            text.TextTrimming(Xaml::TextTrimming::CharacterEllipsis);
            text.HorizontalAlignment(Xaml::HorizontalAlignment::Center);
        }
        panel.Children().Append(text);
    }
    return panel;
}

// The button named so in the template of a control, once it has one.
inline winrt::Microsoft::UI::Xaml::Controls::Button
FindTemplateButton(const winrt::Microsoft::UI::Xaml::DependencyObject& parent,
                   const wchar_t *name)
{
    namespace Xaml = winrt::Microsoft::UI::Xaml;

    const int count = Xaml::Media::VisualTreeHelper::GetChildrenCount(parent);
    for ( int n = 0; n < count; ++n )
    {
        const auto child = Xaml::Media::VisualTreeHelper::GetChild(parent, n);
        if ( const auto element = child.try_as<Xaml::FrameworkElement>() )
        {
            if ( element.Name() == name )
            {
                if ( const auto button =
                        element.try_as<Xaml::Controls::Button>() )
                {
                    return button;
                }
            }
        }
        if ( const auto found = FindTemplateButton(child, name) )
            return found;
    }
    return nullptr;
}

} // namespace wxRibbonWinUICommandDetail

inline wxRibbonWinUICommandVisual
wxRibbonWinUIBuildCommand(const wxRibbonWinUICommandSpec& spec)
{
    namespace Xaml = winrt::Microsoft::UI::Xaml;
    namespace Controls = Xaml::Controls;
    namespace Automation = Xaml::Automation;
    namespace Detail = wxRibbonWinUICommandDetail;

    wxRibbonWinUICommandVisual visual;
    visual.root = Controls::Grid();
    visual.root.Background(Xaml::Media::SolidColorBrush(
        winrt::Windows::UI::Colors::Transparent()));
    Controls::ColumnDefinition primaryColumn;
    primaryColumn.Width(Xaml::GridLengthHelper::Auto());
    visual.root.ColumnDefinitions().Append(primaryColumn);

    // A control which holds its content measures to it: it is only kept
    // from being smaller than the size class of the command, and an icon
    // alone gets a square.
    const bool large = spec.iconSize > 16.0;
    const bool labelled =
        spec.labelPosition != Controls::CommandBarLabelPosition::Collapsed &&
        !spec.label.empty();
    const auto finish = [&]()
    {
        visual.primary.IsEnabled(spec.enabled);
        visual.primary.MinWidth(
            labelled ? spec.minWidth
                     : std::max(spec.minWidth, spec.primaryHeight));
        // All the commands of a size class are as high as each other,
        // whatever their labels: a row of them is even, and the height of
        // a ribbon does not depend on which of its layouts is in use.
        visual.primary.Height(spec.primaryHeight);
        visual.primary.VerticalContentAlignment(
            labelled && large ? Xaml::VerticalAlignment::Top
                              : Xaml::VerticalAlignment::Center);
        visual.primary.Padding(
            !labelled ? Xaml::ThicknessHelper::FromUniformLength(0)
                      : large ? Xaml::ThicknessHelper::FromLengths(6, 6, 6, 6)
                              : Xaml::ThicknessHelper::FromLengths(8, 0, 8, 0));
        visual.primary.VerticalAlignment(Xaml::VerticalAlignment::Stretch);
        visual.primary.HorizontalAlignment(Xaml::HorizontalAlignment::Stretch);
        Automation::AutomationProperties::SetName(
            visual.primary, wxWinUIToHString(spec.label));
        visual.root.Children().Append(visual.primary);
    };
    const auto finishAppBar = [&]()
    {
        visual.primary.IsEnabled(spec.enabled);
        visual.primary.MinWidth(spec.minWidth);
        visual.primary.Width(std::numeric_limits<double>::quiet_NaN());
        visual.primary.Height(spec.primaryHeight);
        Automation::AutomationProperties::SetName(
            visual.primary, wxWinUIToHString(spec.label));
        visual.root.Children().Append(visual.primary);
    };

    // A command with an action and a menu: a SplitButton, whose second part
    // is found in its template once it has been measured.
    if ( spec.kind == wxRIBBON_BUTTON_HYBRID )
    {
        try
        {
            const auto split = Detail::LoadSubtle(L"SplitButton", L"SplitButton")
                                .as<Controls::SplitButton>();
            split.Content(Detail::MakeContent(spec));
            visual.primary = split;
            finish();

            visual.root.Measure(winrt::Windows::Foundation::Size{
                std::numeric_limits<float>::infinity(),
                std::numeric_limits<float>::infinity() });
            visual.arrow = Detail::FindTemplateButton(split, L"SecondaryButton");
            if ( visual.arrow )
            {
                Automation::AutomationProperties::SetName(
                    visual.arrow, wxWinUIToHString(spec.label + " menu"));
                visual.split = split;
                return visual;
            }
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("ribbon split button", e);
        }

        // Without that part the menu could not be opened: the command is
        // shown as a button followed by an arrow of its own instead.
        visual.root.Children().Clear();
        visual.primary = nullptr;
        visual.arrow = nullptr;
    }

    // A command which only opens a menu: a DropDownButton.
    if ( spec.kind == wxRIBBON_BUTTON_DROPDOWN )
    {
        try
        {
            const auto button =
                Detail::LoadSubtle(L"DropDownButton", L"Button")
                    .as<Controls::DropDownButton>();
            button.Content(Detail::MakeContent(spec));
            visual.primary = button;
            finish();
            return visual;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("ribbon drop down button", e);
            visual.root.Children().Clear();
            visual.primary = nullptr;
        }
    }

    // A plain command or a toggle.
    if ( spec.kind == wxRIBBON_BUTTON_NORMAL ||
         spec.kind == wxRIBBON_BUTTON_TOGGLE )
    {
        try
        {
            if ( spec.kind == wxRIBBON_BUTTON_TOGGLE )
            {
                const auto button =
                    Detail::LoadSubtle(L"ToggleButton", L"ToggleButton")
                        .as<Controls::Primitives::ToggleButton>();
                button.Content(Detail::MakeContent(spec));
                button.IsChecked(spec.checked);
                visual.primary = button;
            }
            else
            {
                const auto button = Detail::LoadSubtle(L"Button", L"Button")
                                        .as<Controls::Button>();
                button.Content(Detail::MakeContent(spec));
                visual.primary = button;
            }
            finish();
            return visual;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("ribbon button", e);
            visual.root.Children().Clear();
            visual.primary = nullptr;
        }
    }

    Controls::IconElement icon{nullptr};
    if ( spec.bitmap.IsOk() )
    {
        Controls::ImageIcon image;
        image.Source(wxWinUIWriteableBitmapFromBitmap(spec.bitmap));
        image.Width(spec.iconSize);
        image.Height(spec.iconSize);
        icon = image;
    }
    if ( spec.kind == wxRIBBON_BUTTON_TOGGLE )
    {
        Controls::AppBarToggleButton button;
        button.Label(wxWinUIToHString(spec.label));
        button.LabelPosition(spec.labelPosition);
        button.Icon(icon);
        button.IsChecked(spec.checked);
        visual.primary = button;
    }
    else
    {
        Controls::AppBarButton button;
        button.Label(wxWinUIToHString(spec.label));
        button.LabelPosition(spec.labelPosition);
        button.Icon(icon);
        visual.primary = button;
    }
    finishAppBar();

    if ( spec.kind == wxRIBBON_BUTTON_HYBRID ||
         spec.kind == wxRIBBON_BUTTON_DROPDOWN )
    {
        Controls::ColumnDefinition arrowColumn;
        arrowColumn.Width(Xaml::GridLengthHelper::Auto());
        visual.root.ColumnDefinitions().Append(arrowColumn);
        visual.arrow = Controls::Button();
        Controls::FontIcon chevron;
        chevron.Glyph(L"");
        chevron.FontSize(12);
        visual.arrow.Content(chevron);
        visual.arrow.Width(24.0);
        visual.arrow.Padding({0, 0, 0, 0});
        visual.arrow.VerticalAlignment(Xaml::VerticalAlignment::Stretch);
        visual.arrow.IsEnabled(spec.enabled);
        Automation::AutomationProperties::SetName(
            visual.arrow, wxWinUIToHString(spec.label + " menu"));
        Controls::Grid::SetColumn(visual.arrow, 1);
        visual.root.Children().Append(visual.arrow);
    }
    return visual;
}

#endif // _WX_RIBBON_COMMAND_WINUI_H_
