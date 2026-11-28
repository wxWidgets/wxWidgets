/////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/ribbonchromewinui.h
// Purpose:     What the containers of a ribbon show, as XAML content
// Author:      wxWidgets development team
// Created:     2026-11-22
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RIBBON_CHROME_WINUI_H_
#define _WX_RIBBON_CHROME_WINUI_H_

// A page and a group of a ribbon show what is theirs -- the strip behind the
// groups, the label and the divider of a group, the button a folded group
// turns into -- as XAML content of their window, under the windows of their
// children, in the way a static box does. wxRibbon keeps the layout, the
// state and the events; its paint handler, which used to draw all this with
// an art provider, brings the content up to date instead.
//
// Brushes, sizes and structure follow the Ribbon of the Windows Community
// Toolkit (Labs), RibbonStyle.xaml: the strip is a layer fill between two
// card strokes; a group has a label of secondary 12 pixel text under its
// content and a divider at its trailing edge; a folded group is a subtle
// button showing an icon over a small chevron. The matching metrics are in
// art_winui.cpp.

#include "wx/window.h"

#include "../winui/private.h"

#include <winrt/Microsoft.UI.Xaml.Markup.h>

#include <string>

class wxRibbonChromeWinUI
{
public:
    // Loads the markup as the content of the window. False if the window
    // cannot have XAML content, in which case nothing changed.
    bool Create(wxWindow *owner, const wchar_t *markup)
    {
        namespace X = winrt::Microsoft::UI::Xaml;

        try
        {
            if ( !m_host.Initialize(owner) )
            {
                return false;
            }

            m_root = X::Markup::XamlReader::Load(markup)
                        .as<X::FrameworkElement>();
            if ( m_host.SetContent(m_root) )
            {
                return true;
            }
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("ribbon chrome", e);
        }

        m_root = nullptr;
        m_host.Close();
        return false;
    }

    void Close()
    {
        m_root = nullptr;
        m_host.Close();
    }

    bool IsOk() const { return m_root != nullptr; }

    template <typename T>
    T Find(const wchar_t *name) const
    {
        return m_root ? m_root.FindName(name).try_as<T>() : T{ nullptr };
    }

    // What was last shown, for the callers not to write a property again.
    wxString label;
    int state = -1;
    int extent = -1;
    void *icon = nullptr;

    // Set while the content is brought up to date, for its own events not
    // to be taken for the user's.
    bool syncing = false;
    // When the user last chose a tab, see wxRibbonBar::WinUISyncTabs().
    unsigned long long chosen = 0;

private:
    wxWinUIControlHost m_host;
    winrt::Microsoft::UI::Xaml::FrameworkElement m_root{ nullptr };
};

// The strip of a page.
inline const wchar_t *wxRibbonPageChromeMarkup()
{
    return
        L"<Border xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation'"
        L" IsHitTestVisible='False'"
        L" Background='{ThemeResource LayerFillColorDefaultBrush}'"
        L" BorderBrush='{ThemeResource CardStrokeColorDefaultSolidBrush}'"
        L" BorderThickness='0,1,0,1'/>";
}

// A group: its divider and its label, and where the button it shows when
// it is folded goes (a real button, made in wxRibbonPanel::CommonInit():
// only it, not the whole group, answers the pointer).
// The content of an unfolded group is the windows of its children. The
// group a folded one is unfolded into, in a window of its own, is shown as
// the card of a flyout instead of with a divider.
inline const wchar_t *wxRibbonPanelChromeMarkup()
{
    return
        L"<Grid xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation'"
        L" xmlns:x='http://schemas.microsoft.com/winfx/2006/xaml'>"
        L"<Border x:Name='Card' Visibility='Collapsed' CornerRadius='8'"
        L" BorderThickness='1' IsHitTestVisible='False'"
        L" Background='{ThemeResource LayerFillColorDefaultBrush}'"
        L" BorderBrush='{ThemeResource SurfaceStrokeColorFlyoutBrush}'/>"
        L"<Border x:Name='Divider' HorizontalAlignment='Right' Width='1'"
        L" Margin='0,4,0,4' IsHitTestVisible='False'"
        L" Background='{ThemeResource DividerStrokeColorDefaultBrush}'/>"
        L"<Grid x:Name='Folded' Visibility='Collapsed' Margin='12,4,13,24'/>"
        L"<TextBlock x:Name='Label' HorizontalAlignment='Center'"
        L" VerticalAlignment='Bottom' Margin='12,0,13,4' FontSize='12'"
        L" TextTrimming='CharacterEllipsis' IsHitTestVisible='False'"
        L" Foreground='{ThemeResource TextFillColorSecondaryBrush}'/>"
        L"<Border x:Name='Launcher' Visibility='Collapsed' CornerRadius='3'"
        L" Width='16' Height='16' HorizontalAlignment='Right'"
        L" VerticalAlignment='Bottom' Margin='0,0,5,4' IsHitTestVisible='False'>"
        L"<FontIcon x:Name='LauncherGlyph' Glyph='&#xE8A7;' FontSize='10'"
        L" Foreground='{ThemeResource TextFillColorSecondaryBrush}'/>"
        L"</Border>"
        L"</Grid>";
}

// The tabs of a ribbon: a SelectorBar, the control WinUI has for choosing
// among a few views (Pivot is the one it replaces), and after it the small
// buttons of the bar -- fold or unfold the ribbon, help. The rest of the
// window of the bar, where its page is, is left empty.
inline const wchar_t *wxRibbonBarChromeMarkup()
{
    return
        L"<Grid xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation'"
        L" xmlns:x='http://schemas.microsoft.com/winfx/2006/xaml'>"
        L"<Grid x:Name='Strip' VerticalAlignment='Top'>"
        L"<Grid.ColumnDefinitions>"
        L"<ColumnDefinition Width='*'/><ColumnDefinition Width='Auto'/>"
        L"</Grid.ColumnDefinitions>"
        L"<ScrollViewer x:Name='TabScroll' HorizontalScrollMode='Enabled'"
        L" VerticalScrollMode='Disabled'"
        L" HorizontalScrollBarVisibility='Hidden'"
        L" VerticalScrollBarVisibility='Disabled'>"
        L"<SelectorBar x:Name='Tabs' HorizontalAlignment='Left'"
        L" VerticalAlignment='Stretch'/>"
        L"</ScrollViewer>"
        L"<StackPanel x:Name='Buttons' Grid.Column='1' Orientation='Horizontal'"
        L" VerticalAlignment='Center' Margin='0,0,4,0' Spacing='2'/>"
        L"</Grid>"
        L"</Grid>";
}

// A button which scrolls the groups of a page: opaque, as it lies over
// them, in the colours of the scroll buttons of the ribbon of the Toolkit.
inline std::wstring wxRibbonScrollButtonChromeMarkup(const wchar_t *glyph)
{
    const std::wstring keys =
        L"<StaticResource x:Key='ButtonBackground' ResourceKey='ControlSolidFillColorDefaultBrush'/>"
        L"<StaticResource x:Key='ButtonBackgroundPointerOver' ResourceKey='SolidBackgroundFillColorQuarternaryBrush'/>"
        L"<StaticResource x:Key='ButtonBackgroundPressed' ResourceKey='SolidBackgroundFillColorBaseBrush'/>"
        L"<StaticResource x:Key='ButtonBorderBrush' ResourceKey='CardStrokeColorDefaultSolidBrush'/>"
        L"<StaticResource x:Key='ButtonBorderBrushPointerOver' ResourceKey='CardStrokeColorDefaultSolidBrush'/>"
        L"<StaticResource x:Key='ButtonBorderBrushPressed' ResourceKey='CardStrokeColorDefaultSolidBrush'/>";

    std::wstring markup =
        L"<Button xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation'"
        L" xmlns:x='http://schemas.microsoft.com/winfx/2006/xaml'"
        L" x:Name='Scroll' HorizontalAlignment='Stretch'"
        L" VerticalAlignment='Stretch' Padding='0' MinWidth='0' MinHeight='0'>"
        L"<Button.Resources><ResourceDictionary>"
        L"<ResourceDictionary.ThemeDictionaries>"
        L"<ResourceDictionary x:Key='Default'>";
    markup += keys;
    markup += L"</ResourceDictionary><ResourceDictionary x:Key='Light'>";
    markup += keys;
    markup += L"</ResourceDictionary><ResourceDictionary x:Key='HighContrast'/>"
              L"</ResourceDictionary.ThemeDictionaries>"
              L"</ResourceDictionary></Button.Resources>"
              L"<FontIcon FontSize='8' Glyph='";
    markup += glyph;
    markup += L"'/></Button>";
    return markup;
}

// A brush of the theme by its name, or none.
inline winrt::Microsoft::UI::Xaml::Media::Brush
wxRibbonChromeBrush(const wchar_t *key)
{
    try
    {
        const auto resources =
            winrt::Microsoft::UI::Xaml::Application::Current().Resources();
        return resources.Lookup(winrt::box_value(key))
                    .try_as<winrt::Microsoft::UI::Xaml::Media::Brush>();
    }
    catch ( const winrt::hresult_error& )
    {
        return nullptr;
    }
}

#endif // _WX_RIBBON_CHROME_WINUI_H_
