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
#include <limits>

// This source-private factory owns only visual construction. Each command
// adapter retains its own model, callback identity and publication transaction.
// In particular, no event is emitted while applying initial toggle state.
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
    winrt::Microsoft::UI::Xaml::Controls::Button arrow{nullptr};
};

inline wxRibbonWinUICommandVisual
wxRibbonWinUIBuildCommand(const wxRibbonWinUICommandSpec& spec)
{
    namespace Xaml = winrt::Microsoft::UI::Xaml;
    namespace Controls = Xaml::Controls;
    namespace Automation = Xaml::Automation;

    wxRibbonWinUICommandVisual visual;
    visual.root = Controls::Grid();
    visual.root.Background(Xaml::Media::SolidColorBrush(
        winrt::Windows::UI::Colors::Transparent()));
    Controls::ColumnDefinition primaryColumn;
    primaryColumn.Width(Xaml::GridLengthHelper::Auto());
    visual.root.ColumnDefinitions().Append(primaryColumn);

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
    visual.primary.IsEnabled(spec.enabled);
    visual.primary.MinWidth(spec.minWidth);
    visual.primary.Width(std::numeric_limits<double>::quiet_NaN());
    visual.primary.Height(spec.primaryHeight);
    Automation::AutomationProperties::SetName(
        visual.primary, wxWinUIToHString(spec.label));
    visual.root.Children().Append(visual.primary);

    if ( spec.kind == wxRIBBON_BUTTON_HYBRID ||
         spec.kind == wxRIBBON_BUTTON_DROPDOWN )
    {
        Controls::ColumnDefinition arrowColumn;
        arrowColumn.Width(Xaml::GridLengthHelper::Auto());
        visual.root.ColumnDefinitions().Append(arrowColumn);
        visual.arrow = Controls::Button();
        Controls::FontIcon chevron;
        chevron.Glyph(L"\uE70D");
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
