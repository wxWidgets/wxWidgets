/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/toolbar.cpp
// Purpose:     wxToolBar for wxWinUI, made of a WinUI CommandBar
// Author:      wxWidgets development team
// Created:     2026-07-20
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_TOOLBAR

#include "wx/toolbar.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/control.h"
    #include "wx/event.h"
#endif

#if wxUSE_MENUS
    #include "wx/menu.h"
#endif

#include "private.h"
#ifdef WXWINUI_TEST_SUPPORT
    #include "../../tests/winui/test-support/toolbar-test-access.h"
#endif
#include "wx/winui/private/appearance.h"

#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#ifdef WXWINUI_TEST_SUPPORT
    #include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>
#endif
#include <winrt/Microsoft.UI.Xaml.Input.h>

#include <atomic>
#include <cmath>
#include <limits>
#include <memory>
#include <vector>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;
namespace MUXAP = winrt::Microsoft::UI::Xaml::Automation::Peers;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXI = winrt::Microsoft::UI::Xaml::Input;

namespace
{

// The measures of the theme of WinUI: the height of a CommandBar
// (AppBarThemeCompactHeight), the size its buttons give their icon
// (AppBarButtonContentHeight), and the width of a button which has no label,
// which leaves the frame of 36 by 36 of the icon buttons of Windows 11.
const int BAR_DIP = 48;
const int ICON_DIP = 16;
const int BUTTON_DIP = 40;

// The width and the height of a button with its label under its icon
// (the width of the style of AppBarButton, AppBarThemeMinHeight).
const int LABELLED_DIP = 68;
const int LABELLED_HEIGHT_DIP = 64;

// The tool bars alive, for the tests which look for what outlives its owner.
std::atomic<size_t> gs_liveToolBars{0};

// The width of the label of a button, which is written at 12.
double wxWinUIMeasureToolLabel(const winrt::hstring& label)
{
    if ( label.empty() )
        return 0;

    try
    {
        const float unlimited = std::numeric_limits<float>::infinity();
        MUXC::TextBlock block;
        block.FontSize(12);
        block.Text(label);
        block.Measure({ unlimited, unlimited });
        return block.DesiredSize().Width;
    }
    catch ( const winrt::hresult_error& )
    {
        return 7.0 * label.size();
    }
}

} // anonymous namespace

// ----------------------------------------------------------------------------
// wxToolBarTool
// ----------------------------------------------------------------------------

wxIMPLEMENT_CLASS(wxToolBarTool, wxToolBarToolBase);

wxToolBarTool::wxToolBarTool(wxToolBar *tbar,
                             int toolid,
                             const wxString& label,
                             const wxBitmapBundle& bmpNormal,
                             const wxBitmapBundle& bmpDisabled,
                             wxItemKind kind,
                             wxObject *clientData,
                             const wxString& shortHelp,
                             const wxString& longHelp)
    : wxToolBarToolBase(tbar, toolid, label, bmpNormal, bmpDisabled,
                        kind, clientData, shortHelp, longHelp)
{
}

wxToolBarTool::wxToolBarTool(wxToolBar *tbar,
                             wxControl *control,
                             const wxString& label)
    : wxToolBarToolBase(tbar, control, label)
{
}

wxToolBarTool::~wxToolBarTool() = default;

void wxToolBarTool::SetLabel(const wxString& label)
{
    wxToolBarToolBase::SetLabel(label);

    if ( wxToolBarBase * const tbar = GetToolBar() )
        static_cast<wxToolBar *>(tbar)->WinUIUpdateLabel(this);
}

// ----------------------------------------------------------------------------
// wxWinUIToolBarImpl
// ----------------------------------------------------------------------------

class wxWinUIToolBarImpl
{
public:
    // What stands for one tool.
    struct Entry
    {
        wxToolBarToolBase *tool = nullptr;

        // In the bar: the button, the separator, or what holds the place of
        // a control. Null for the stretchable space, which is no element
        // but the gap between the two bars.
        MUX::FrameworkElement element{ nullptr };

        // The same when the tool can be checked.
        MUXC::AppBarToggleButton toggle{ nullptr };

        // The picture of the tool.
        MUXC::ImageIcon icon{ nullptr };
        bool usesDisabledBitmap = false;

        // The button with the chevron of a tool with a menu, and what
        // holds it in the bar.
        MUXC::Button arrow{ nullptr };
        MUX::FrameworkElement arrowElement{ nullptr };

        // The place kept for the window of a control, and its size.
        MUXC::Border place{ nullptr };
        double placeHeight = 0;
    };

    using Owner = std::shared_ptr<wxToolBar *>;

    wxWinUIToolBarImpl() { ++gs_liveToolBars; }
    ~wxWinUIToolBarImpl() { --gs_liveToolBars; }

    Entry *Find(const wxToolBarToolBase *tool)
    {
        for ( Entry& entry : entries )
        {
            if ( entry.tool == tool )
                return &entry;
        }
        return nullptr;
    }

    // True while the element is in the menu of the tools which do not fit.
    static bool IsInOverflow(const MUX::FrameworkElement& element)
    {
        if ( !element )
            return false;

        try
        {
            const auto command = element.try_as<MUXC::ICommandBarElement>();
            return command && command.IsInOverflow();
        }
        catch ( const winrt::hresult_error& )
        {
            return false;
        }
    }

    // In pixels, in the client area of the tool bar.
    bool GetRect(const MUX::FrameworkElement& element, double scale,
                 wxRect *rect) const
    {
        if ( !element || !root )
            return false;

        try
        {
            if ( !element.IsLoaded() || element.ActualWidth() <= 0 )
                return false;

            const auto origin = element.TransformToVisual(root)
                                    .TransformPoint({ 0, 0 });
            *rect = wxRect(wxRound(origin.X * scale),
                           wxRound(origin.Y * scale),
                           wxRound(element.ActualWidth() * scale),
                           wxRound(element.ActualHeight() * scale));
            return true;
        }
        catch ( const winrt::hresult_error& )
        {
            // In the menu of the tools which do not fit: another tree.
            return false;
        }
    }

    // The picture of the tool for the state it is in: the bitmap the
    // program gave for the disabled state, or the normal one made fainter.
    void ApplyIcon(Entry& entry, bool enabled) const
    {
        if ( !entry.icon || !entry.tool )
            return;

        const wxBitmapBundle disabled = entry.tool->GetDisabledBitmapBundle();
        const bool useDisabled = !enabled && disabled.IsOk();
        const wxBitmapBundle bundle =
            useDisabled ? disabled : entry.tool->GetNormalBitmapBundle();
        const wxBitmap bitmap =
            bundle.IsOk() ? bundle.GetBitmap(iconPixels) : wxBitmap();

        entry.usesDisabledBitmap = useDisabled;
        try
        {
            if ( bitmap.IsOk() )
                entry.icon.Source(wxWinUIWriteableBitmapFromBitmap(bitmap));
            else
                entry.icon.Source(nullptr);
            entry.icon.Opacity(enabled || useDisabled ? 1.0 : 0.4);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI tool bar icon", e);
        }
    }

    void ApplyEnabled(Entry& entry, bool enabled) const
    {
        try
        {
            if ( entry.element )
            {
                if ( const auto control =
                        entry.element.try_as<MUXC::Control>() )
                {
                    control.IsEnabled(enabled);
                }
            }
            if ( entry.arrow )
                entry.arrow.IsEnabled(enabled);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI tool bar tool state", e);
        }

        ApplyIcon(entry, enabled);
    }

    // Takes the element out of whichever bar holds it.
    void Remove(const MUX::FrameworkElement& element) const
    {
        if ( !element )
            return;

        uint32_t index = 0;
        if ( column )
        {
            const auto children = column.Children();
            if ( children.IndexOf(element, index) )
                children.RemoveAt(index);
            return;
        }

        const auto command = element.try_as<MUXC::ICommandBarElement>();
        if ( !command )
            return;

        for ( const MUXC::CommandBar& holder : { bar, endBar } )
        {
            if ( !holder )
                continue;

            const auto commands = holder.PrimaryCommands();
            if ( commands.IndexOf(command, index) )
            {
                commands.RemoveAt(index);
                return;
            }
        }
    }

    wxWinUIControlHost host;
    MUXC::Grid root{ nullptr };

    // Horizontal: the tools before the stretchable space, and those after
    // it if there is one. Vertical: a column.
    MUXC::CommandBar bar{ nullptr };
    MUXC::CommandBar endBar{ nullptr };
    MUXC::StackPanel column{ nullptr };

    std::vector<Entry> entries;

    // The size of the pictures of the tools.
    wxSize iconPixels;

    // What the tools take when nothing limits the bar, in DIPs.
    wxSize bestDip;

    // Given to every handler: holds null once the tool bar is gone.
    Owner owner;

    winrt::event_token layoutToken{};

    // Realize() was called: later changes are shown without another call.
    bool realized = false;
    bool rebuildPending = false;

    // The program is changing a state: what XAML says of it is no click.
    bool syncing = false;
    bool placing = false;
};

// ----------------------------------------------------------------------------
// creation
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxToolBar, wxControl);

wxToolBar::wxToolBar() = default;

wxToolBar::wxToolBar(wxWindow *parent,
                     wxWindowID id,
                     const wxPoint& pos,
                     const wxSize& size,
                     long style,
                     const wxString& name)
{
    Create(parent, id, pos, size, style, name);
}

wxToolBar::~wxToolBar()
{
    if ( m_winui )
    {
        // Nothing queued by XAML or by wx finds this object again.
        if ( m_winui->owner )
            *m_winui->owner = nullptr;

        try
        {
            if ( m_winui->root && m_winui->layoutToken )
                m_winui->root.LayoutUpdated(m_winui->layoutToken);
        }
        catch ( const winrt::hresult_error& )
        {
        }
    }
}

bool wxToolBar::Create(wxWindow *parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name)
{
    style = (style & ~wxBORDER_MASK) | wxBORDER_NONE;
    if ( !wxControl::Create(parent, id, pos, size, style,
                            wxDefaultValidator, name) )
    {
        return false;
    }

    FixupStyle();

    m_winui.reset(new wxWinUIToolBarImpl);
    m_winui->owner = std::make_shared<wxToolBar *>(this);
    if ( !m_winui->host.Initialize(this) )
        return false;

    try
    {
        // A tool bar for a screen reader too, whatever is inside.
        m_winui->root = wxWinUICreateAccessibleGrid(
            MUXAP::AutomationControlType::ToolBar, "wxToolBar");
        if ( !m_winui->root )
            m_winui->root = MUXC::Grid();
        MUXA::AutomationProperties::SetLocalizedControlType(
            m_winui->root, wxWinUIToHString(_("tool bar")));
        m_winui->host.SetContent(m_winui->root);

        // The controls among the tools follow the places the bar gives
        // them, which change with its width.
        const std::weak_ptr<wxToolBar *> weak = m_winui->owner;
        m_winui->layoutToken = m_winui->root.LayoutUpdated(
            [weak](const auto&, const auto&)
            {
                const auto owner = weak.lock();
                if ( owner && *owner )
                    (*owner)->WinUIPlaceControls();
            });
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI tool bar creation", e);
        return false;
    }

    Bind(wxEVT_DPI_CHANGED, &wxToolBar::OnDPIChanged, this);

    return true;
}

wxToolBarToolBase *
wxToolBar::CreateTool(int toolid,
                      const wxString& label,
                      const wxBitmapBundle& bmpNormal,
                      const wxBitmapBundle& bmpDisabled,
                      wxItemKind kind,
                      wxObject *clientData,
                      const wxString& shortHelp,
                      const wxString& longHelp)
{
    return new wxToolBarTool(this, toolid, label, bmpNormal, bmpDisabled,
                             kind, clientData, shortHelp, longHelp);
}

wxToolBarToolBase *
wxToolBar::CreateTool(wxControl *control, const wxString& label)
{
    return new wxToolBarTool(this, control, label);
}

// ----------------------------------------------------------------------------
// the tools as XAML elements
// ----------------------------------------------------------------------------

bool wxToolBar::Realize()
{
    if ( !m_winui )
        return false;

    // False when there is no tool, which is no failure here: the bar is
    // then shown empty.
    wxToolBarBase::Realize();

    // As in the other ports: the first tool of a group of radio tools is
    // down unless the program put another one down, and only one is.
    wxToolBarToolBase *first = nullptr;
    wxToolBarToolBase *down = nullptr;
    for ( wxToolBarToolsList::compatibility_iterator
            node = m_tools.GetFirst(); ; node = node->GetNext() )
    {
        wxToolBarToolBase * const
            tool = node ? node->GetData() : nullptr;
        if ( tool && tool->IsButton() && tool->GetKind() == wxITEM_RADIO )
        {
            if ( !first )
                first = tool;
            if ( tool->IsToggled() )
            {
                if ( down )
                    down->Toggle(false);
                down = tool;
            }
        }
        else // the end of a group, if there was one
        {
            if ( first && !down )
                first->Toggle(true);
            first =
            down = nullptr;
        }

        if ( !node )
            break;
    }

    m_winui->realized = true;
    Rebuild();

    // The frame or the sizer which holds the bar gives it its new size.
    InvalidateBestSize();
    SendSizeEventToParent();

    return true;
}

void wxToolBar::ScheduleRebuild()
{
    if ( !m_winui || !m_winui->realized || m_winui->rebuildPending )
        return;

    m_winui->rebuildPending = true;

    const std::weak_ptr<wxToolBar *> weak = m_winui->owner;
    CallAfter([weak]()
    {
        const auto owner = weak.lock();
        if ( !owner || !*owner )
            return;

        wxToolBar * const self = *owner;
        if ( !self->m_winui->rebuildPending )
            return;

        self->Rebuild();
        self->InvalidateBestSize();
        self->SendSizeEventToParent();
    });
}

void wxToolBar::Rebuild()
{
    if ( !m_winui || !m_winui->root )
        return;

    wxWinUIToolBarImpl& impl = *m_winui;
    impl.rebuildPending = false;

    const std::weak_ptr<wxToolBar *> weak = impl.owner;
    const double scale = GetDPIScaleFactor();

    const bool vertical = IsVertical();
    const bool icons = !HasFlag(wxTB_NOICONS);
    const bool tips = !HasFlag(wxTB_NO_TOOLTIPS);
    const bool barEnabled = IsEnabled();

    // Beside the icons in a CommandBar, as in the command bars of Windows
    // 11; under them in a column, as in its navigation rails.
    const bool text = HasFlag(wxTB_TEXT);

    // The size of the pictures: the one the program asked for, or the one
    // of the largest of the bitmaps it gave.
    wxSize iconDip = GetRequestedToolBitmapSizeInDIPs();
    if ( iconDip.x <= 0 || iconDip.y <= 0 )
    {
        iconDip = wxSize(0, 0);
        for ( wxToolBarToolsList::compatibility_iterator
                node = m_tools.GetFirst(); node; node = node->GetNext() )
        {
            const wxToolBarToolBase * const tool = node->GetData();
            if ( !tool->IsButton() )
                continue;

            const wxBitmapBundle bundle = tool->GetNormalBitmapBundle();
            if ( bundle.IsOk() )
                iconDip.IncTo(bundle.GetDefaultSize());
        }

        if ( iconDip.x <= 0 || iconDip.y <= 0 )
            iconDip = wxSize(ICON_DIP, ICON_DIP);
    }
    impl.iconPixels = wxSize(wxRound(iconDip.x * scale),
                             wxRound(iconDip.y * scale));

    // A button with no label: the frame of 36 by 36 around the icon.
    const double plainWidth = wxMax(BUTTON_DIP, iconDip.x + 24);

    // What is above the icon, and under it unless the label is there.
    const double edge = wxMax(4.0, (BAR_DIP - iconDip.y) / 2.0);
    const bool below = text && vertical;

    // The height of a button: of the bar, or of a button of a column.
    const double plainHeight = wxMax<double>(BAR_DIP, iconDip.y + 2 * edge);
    const double labelledHeight =
        wxMax<double>(LABELLED_HEIGHT_DIP, edge + iconDip.y + 2 + 24);

    // The margins which keep an icon of another height than 16 in the
    // middle of the bar, and the label of a tool with no icon at the same
    // distance from both edges of its button.
    const auto setIconMetrics =
        [&iconDip, edge, below](const MUXC::Control& button, bool hasIcon)
        {
            if ( iconDip.y == ICON_DIP && hasIcon )
                return;

            const auto resources = button.Resources();
            resources.Insert(
                winrt::box_value(L"AppBarButtonContentHeight"),
                winrt::box_value(static_cast<double>(iconDip.y)));
            resources.Insert(
                winrt::box_value(L"AppBarButtonContentViewboxCollapsedMargin"),
                winrt::box_value(
                    MUX::Thickness{ 0, edge, 0, below ? 2.0 : edge }));
            resources.Insert(
                winrt::box_value(L"AppBarButtonContentViewboxMargin"),
                winrt::box_value(
                    MUX::Thickness{ hasIcon ? 12.0 : 4.0, edge, 0, edge }));
        };

    // The room the tools take: XAML only knows it once it has shown them.
    // Along the bar, starting with the padding of a CommandBar, and across
    // it for a column.
    double along = vertical ? 0 : 4,
           across = 0;

    const auto makeBar = [text]()
    {
        MUXC::CommandBar bar;

        // Beside the icons: under them, a CommandBar shows the labels only
        // while it is open.
        bar.DefaultLabelPosition(
            text ? MUXC::CommandBarDefaultLabelPosition::Right
                 : MUXC::CommandBarDefaultLabelPosition::Collapsed);

        // From the start of the bar, as the tools of a tool bar are; a
        // CommandBar left to itself puts its commands at its end.
        bar.HorizontalAlignment(MUX::HorizontalAlignment::Left);
        bar.IsDynamicOverflowEnabled(true);
        bar.OverflowButtonVisibility(
            MUXC::CommandBarOverflowButtonVisibility::Auto);
        return bar;
    };

    try
    {
        impl.entries.clear();
        impl.root.Children().Clear();
        impl.root.ColumnDefinitions().Clear();
        impl.bar = nullptr;
        impl.endBar = nullptr;
        impl.column = nullptr;
        impl.bestDip = wxSize();

        impl.root.FlowDirection(
            GetLayoutDirection() == wxLayout_RightToLeft
                ? MUX::FlowDirection::RightToLeft
                : MUX::FlowDirection::LeftToRight);

        if ( vertical )
        {
            impl.column = MUXC::StackPanel();
            impl.column.Orientation(MUXC::Orientation::Vertical);
            impl.column.HorizontalAlignment(MUX::HorizontalAlignment::Center);
            impl.root.Children().Append(impl.column);
        }
        else
        {
            MUXC::ColumnDefinition start;
            start.Width(MUX::GridLength{ 1, MUX::GridUnitType::Star });
            MUXC::ColumnDefinition end;
            end.Width(MUX::GridLength{ 0, MUX::GridUnitType::Auto });
            impl.root.ColumnDefinitions().Append(start);
            impl.root.ColumnDefinitions().Append(end);

            impl.bar = makeBar();
            impl.root.Children().Append(impl.bar);
        }

        // The bar the next tools go to.
        MUXC::CommandBar target = impl.bar;
        const auto append =
            [&impl, &target](const MUX::FrameworkElement& element)
            {
                if ( impl.column )
                {
                    impl.column.Children().Append(element);
                }
                else
                {
                    target.PrimaryCommands().Append(
                        element.as<MUXC::ICommandBarElement>());
                }
            };

        // What is not a command, in a CommandBar.
        const auto contain =
            [vertical](const MUX::FrameworkElement& content)
                -> MUX::FrameworkElement
            {
                if ( vertical )
                    return content;

                MUXC::AppBarElementContainer container;
                container.Content(content);
                container.IsTabStop(false);
                container.VerticalContentAlignment(
                    MUX::VerticalAlignment::Center);
                return container;
            };

        for ( wxToolBarToolsList::compatibility_iterator
                node = m_tools.GetFirst(); node; node = node->GetNext() )
        {
            wxToolBarToolBase * const tool = node->GetData();

            wxWinUIToolBarImpl::Entry entry;
            entry.tool = tool;

            if ( tool->IsStretchableSpace() )
            {
                if ( impl.bar && !impl.endBar )
                {
                    // The tools after it are at the end of the bar: they
                    // have a CommandBar of their own there.
                    impl.endBar = makeBar();
                    MUXC::Grid::SetColumn(impl.endBar, 1);
                    impl.root.Children().Append(impl.endBar);
                    target = impl.endBar;
                    along += 4;
                }
                else // only one space takes what is left
                {
                    MUXC::Border space;
                    space.Width(vertical ? 0 : 16);
                    space.Height(vertical ? 16 : 0);
                    entry.element = contain(space);
                    append(entry.element);
                    along += 16;
                }

                impl.entries.push_back(entry);
                continue;
            }

            if ( tool->IsSeparator() )
            {
                if ( vertical )
                {
                    // AppBarSeparator is a vertical line: lying down here.
                    MUXC::Border line = wxWinUICreateThemeBrushBorder(
                        "DividerStrokeColorDefaultBrush",
                        wxWinUIThemeBrushProperty::Background);
                    if ( !line )
                        line = MUXC::Border();
                    line.Height(1);
                    line.Margin({ 4, 4, 4, 4 });
                    entry.element = line;
                }
                else
                {
                    entry.element = MUXC::AppBarSeparator();
                }

                along += vertical ? 9 : 5;
                append(entry.element);
                impl.entries.push_back(entry);
                continue;
            }

            if ( tool->IsControl() )
            {
                // The control keeps its window, a child of this one: the
                // bar only keeps its place, see WinUIPlaceControls().
                // The size the program gave it, as in wxMSW, and not its
                // best size: a combo box with one long item must not push
                // all the tools after it out of the bar.
                wxControl * const control = tool->GetControl();
                wxSize best = control ? control->GetSize() : wxSize();
                if ( control && (best.x <= 0 || best.y <= 0) )
                    best = control->GetBestSize();
                entry.place = MUXC::Border();
                entry.placeHeight = best.y / scale;
                entry.place.Width(best.x / scale);
                entry.place.Height(entry.placeHeight);
                entry.place.Margin({ 4, 0, 4, 0 });
                entry.place.VerticalAlignment(MUX::VerticalAlignment::Center);
                entry.element = contain(entry.place);

                if ( vertical )
                {
                    along += best.y / scale;
                    across = wxMax(across, best.x / scale + 8);
                }
                else
                {
                    along += best.x / scale + 8;
                }
                append(entry.element);
                impl.entries.push_back(entry);
                continue;
            }

            // A button.
            const bool hasIcon =
                icons && tool->GetNormalBitmapBundle().IsOk();
            if ( hasIcon )
                entry.icon = MUXC::ImageIcon();

            const winrt::hstring label = wxWinUIToHString(
                wxControl::RemoveMnemonics(tool->GetLabel()));

            MUXC::Control button{ nullptr };
            if ( tool->CanBeToggled() )
            {
                MUXC::AppBarToggleButton toggle;
                toggle.Label(label);
                if ( entry.icon )
                    toggle.Icon(entry.icon);
                toggle.IsChecked(tool->IsToggled());
                if ( !text )
                {
                    toggle.LabelPosition(
                        MUXC::CommandBarLabelPosition::Collapsed);
                }

                // Not Click: a screen reader checks the button without
                // clicking it.
                const auto changed =
                    [weak, tool](const auto& sender, const auto&)
                    {
                        const auto owner = weak.lock();
                        if ( !owner || !*owner )
                            return;

                        const auto checked = sender
                            .template as<MUXC::AppBarToggleButton>()
                            .IsChecked();
                        (*owner)->WinUIOnClick(tool,
                                               checked && checked.Value());
                    };
                toggle.Checked(changed);
                toggle.Unchecked(changed);
                entry.toggle = toggle;
                button = toggle;
            }
            else
            {
                MUXC::AppBarButton plain;
                plain.Label(label);
                if ( entry.icon )
                    plain.Icon(entry.icon);
                if ( !text )
                {
                    plain.LabelPosition(
                        MUXC::CommandBarLabelPosition::Collapsed);
                }
                plain.Click([weak, tool](const auto&, const auto&)
                {
                    const auto owner = weak.lock();
                    if ( owner && *owner )
                        (*owner)->WinUIOnClick(tool, false);
                });
                button = plain;
            }

            setIconMetrics(button, hasIcon);
            const double labelWidth =
                text ? wxWinUIMeasureToolLabel(label) : 0;
            if ( !text )
            {
                button.Width(plainWidth);
            }
            else if ( vertical )
            {
                // As wide as the widest label of the column.
                button.Width(std::numeric_limits<double>::quiet_NaN());
                button.MinWidth(LABELLED_DIP);
                button.HorizontalAlignment(MUX::HorizontalAlignment::Stretch);
            }

            if ( vertical )
            {
                along += text ? labelledHeight : plainHeight;
                across = wxMax(across,
                               text ? wxMax<double>(LABELLED_DIP,
                                                    labelWidth + 8)
                                    : plainWidth);
            }
            else if ( text )
            {
                // XAML lays each button out on whole pixels.
                along += (hasIcon ? 12 + iconDip.x : 4) + 8 +
                         std::ceil(labelWidth) + 1 + 12;
            }
            else
            {
                along += plainWidth;
            }

            // What a screen reader says of a tool which shows no label.
            if ( label.empty() && !tool->GetShortHelp().empty() )
            {
                MUXA::AutomationProperties::SetName(
                    button, wxWinUIToHString(tool->GetShortHelp()));
            }

            if ( tips && !tool->GetShortHelp().empty() )
            {
                MUXC::ToolTipService::SetToolTip(
                    button,
                    winrt::box_value(wxWinUIToHString(tool->GetShortHelp())));
            }

            // The long help of the tool under the pointer goes to the status
            // bar of the frame.
            button.PointerEntered([weak, tool](const auto&, const auto&)
            {
                const auto owner = weak.lock();
                if ( owner && *owner && (*owner)->m_winui->Find(tool) )
                    (*owner)->OnMouseEnter(tool->GetId());
            });
            button.PointerExited([weak](const auto&, const auto&)
            {
                const auto owner = weak.lock();
                if ( owner && *owner )
                    (*owner)->OnMouseEnter(wxID_ANY);
            });
            button.RightTapped(
                [weak, tool](const auto&,
                             const MUXI::RightTappedRoutedEventArgs& args)
                {
                    const auto owner = weak.lock();
                    if ( !owner || !*owner )
                        return;

                    wxToolBar * const self = *owner;
                    if ( !self->m_winui->Find(tool) )
                        return;

                    const double dpi = self->GetDPIScaleFactor();
                    const auto where = args.GetPosition(self->m_winui->root);
                    self->OnRightClick(tool->GetId(),
                                       wxRound(where.X * dpi),
                                       wxRound(where.Y * dpi));
                });

            entry.element = button;
            append(entry.element);

            if ( tool->GetKind() == wxITEM_DROPDOWN )
            {
                // The second half of a split button: a chevron, with no
                // frame until the pointer is over it.
                entry.arrow = MUXC::Button();
                wxWinUIApplySubtleButtonStyle(entry.arrow);
                MUXC::FontIcon chevron;
                chevron.Glyph(L"\xE70D");
                chevron.FontSize(10);
                entry.arrow.Content(chevron);
                entry.arrow.Padding({ 0, 0, 0, 0 });
                entry.arrow.Width(vertical ? plainWidth - 4 : 20);
                entry.arrow.Height(vertical ? 16 : 36);
                entry.arrow.HorizontalAlignment(
                    MUX::HorizontalAlignment::Center);
                MUXA::AutomationProperties::SetName(
                    entry.arrow, wxWinUIToHString(_("More options")));
                entry.arrow.Click([weak, tool](const auto&, const auto&)
                {
                    const auto owner = weak.lock();
                    if ( owner && *owner )
                        (*owner)->WinUIOnDropdown(tool);
                });

                entry.arrowElement = contain(entry.arrow);
                append(entry.arrowElement);
                along += vertical ? 16 : 20;
            }

            impl.ApplyEnabled(entry, barEnabled && tool->IsEnabled());
            impl.entries.push_back(entry);
        }

        // More than the sum along a CommandBar: once a tool is in its
        // menu, it only comes back when there is room for it and for the
        // "..." button, which is as wide as the bar is high. A bar made
        // narrow and then given the size it asked for would otherwise keep
        // its last tool in the menu.
        impl.bestDip = vertical
            ? wxSize(wxRound(std::ceil(wxMax<double>(across, plainWidth))),
                     wxRound(std::ceil(along)))
            : wxSize(wxRound(std::ceil(along)) + 4 + BAR_DIP,
                     wxRound(std::ceil(plainHeight)));
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI tool bar tools", e);
    }
}

// ----------------------------------------------------------------------------
// changes made by the program
// ----------------------------------------------------------------------------

bool wxToolBar::DoInsertTool(size_t WXUNUSED(pos),
                             wxToolBarToolBase *WXUNUSED(tool))
{
    // Shown by the next Realize(), as with every tool bar of wx; and
    // without it once the bar has been realized, when the code which is
    // adding tools has finished.
    ScheduleRebuild();
    return true;
}

bool wxToolBar::DoDeleteTool(size_t WXUNUSED(pos), wxToolBarToolBase *tool)
{
    if ( !m_winui )
        return true;

    wxWinUIToolBarImpl& impl = *m_winui;
    for ( auto it = impl.entries.begin(); it != impl.entries.end(); ++it )
    {
        if ( it->tool != tool )
            continue;

        try
        {
            impl.Remove(it->element);
            impl.Remove(it->arrowElement);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI tool bar tool removal", e);
        }

        impl.entries.erase(it);
        break;
    }

    // The window of a control which is no longer a tool has no place.
    if ( tool->IsControl() && tool->GetControl() )
        tool->GetControl()->Hide();

    // The tools which were after the space are no longer at the end.
    if ( tool->IsStretchableSpace() )
        ScheduleRebuild();

    InvalidateBestSize();
    return true;
}

void wxToolBar::DoEnableTool(wxToolBarToolBase *tool, bool enable)
{
    wxWinUIToolBarImpl::Entry * const
        entry = m_winui ? m_winui->Find(tool) : nullptr;
    if ( entry )
        m_winui->ApplyEnabled(*entry, enable && IsEnabled());
}

void wxToolBar::DoEnable(bool enable)
{
    wxControl::DoEnable(enable);

    if ( !m_winui )
        return;

    for ( wxWinUIToolBarImpl::Entry& entry : m_winui->entries )
    {
        if ( entry.tool && entry.tool->IsButton() )
            m_winui->ApplyEnabled(entry, enable && entry.tool->IsEnabled());
    }
}

void wxToolBar::DoToggleTool(wxToolBarToolBase *tool, bool toggle)
{
    wxWinUIToolBarImpl::Entry * const
        entry = m_winui ? m_winui->Find(tool) : nullptr;
    if ( !entry || !entry->toggle )
        return;

    // Setting it is not a click: no event comes of it.
    const bool syncing = m_winui->syncing;
    m_winui->syncing = true;
    try
    {
        entry->toggle.IsChecked(toggle);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI tool bar tool state", e);
    }
    m_winui->syncing = syncing;
}

void wxToolBar::DoSetToggle(wxToolBarToolBase *WXUNUSED(tool),
                            bool WXUNUSED(toggle))
{
    // Another kind of button stands for the tool.
    if ( m_winui && m_winui->realized )
        Rebuild();
}

void wxToolBar::SetWindowStyleFlag(long style)
{
    const long previous = GetWindowStyleFlag();
    wxToolBarBase::SetWindowStyleFlag(style);

    // Icons, labels, tips and the direction of the bar are all in it.
    if ( m_winui && m_winui->realized && GetWindowStyleFlag() != previous )
    {
        Rebuild();
        InvalidateBestSize();
        SendSizeEventToParent();
    }
}

void wxToolBar::SetToolBitmapSize(const wxSize& size)
{
    wxToolBarBase::SetToolBitmapSize(size);

    if ( m_winui && m_winui->realized )
    {
        Rebuild();
        InvalidateBestSize();
    }
}

void wxToolBar::SetToolShortHelp(int toolid, const wxString& helpString)
{
    wxToolBarBase::SetToolShortHelp(toolid, helpString);

    wxWinUIToolBarImpl::Entry * const
        entry = m_winui ? m_winui->Find(FindById(toolid)) : nullptr;
    if ( !entry || !entry->element || !entry->tool->IsButton() ||
         HasFlag(wxTB_NO_TOOLTIPS) )
    {
        return;
    }

    try
    {
        MUXC::ToolTipService::SetToolTip(
            entry->element,
            helpString.empty()
                ? nullptr
                : winrt::box_value(wxWinUIToHString(helpString)));
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI tool bar tool tip", e);
    }
}

void wxToolBar::SetToolNormalBitmap(int toolid, const wxBitmapBundle& bitmap)
{
    WinUISetNormalBitmap(FindById(toolid), bitmap);
}

void wxToolBar::DoSetToolNormalBitmapByPos(size_t pos,
                                           const wxBitmapBundle& bitmap)
{
    wxCHECK_RET( pos < GetToolsCount(), wxT("invalid tool position") );

    WinUISetNormalBitmap(GetToolByPos(static_cast<int>(pos)), bitmap);
}

void wxToolBar::WinUISetNormalBitmap(wxToolBarToolBase *tool,
                                     const wxBitmapBundle& bitmap)
{
    if ( !tool )
        return;

    tool->SetNormalBitmap(bitmap);

    wxWinUIToolBarImpl::Entry * const
        entry = m_winui ? m_winui->Find(tool) : nullptr;
    if ( !entry )
        return;

    // A tool which had no picture has no element for one, and a tool which
    // has none any more must not keep the room of one.
    const bool wanted = !HasFlag(wxTB_NOICONS) && bitmap.IsOk();
    if ( wanted == static_cast<bool>(entry->icon) )
        m_winui->ApplyIcon(*entry, tool->IsEnabled() && IsEnabled());
    else if ( m_winui->realized )
        Rebuild();
}

void wxToolBar::SetToolDisabledBitmap(int toolid,
                                      const wxBitmapBundle& bitmap)
{
    wxToolBarToolBase * const tool = FindById(toolid);
    if ( !tool )
        return;

    tool->SetDisabledBitmap(bitmap);

    wxWinUIToolBarImpl::Entry * const
        entry = m_winui ? m_winui->Find(tool) : nullptr;
    if ( entry )
        m_winui->ApplyIcon(*entry, tool->IsEnabled() && IsEnabled());
}

void wxToolBar::WinUIUpdateLabel(wxToolBarToolBase *tool)
{
    wxWinUIToolBarImpl::Entry * const
        entry = m_winui ? m_winui->Find(tool) : nullptr;
    if ( !entry || !entry->element )
        return;

    const winrt::hstring label = wxWinUIToHString(
        wxControl::RemoveMnemonics(tool->GetLabel()));
    try
    {
        if ( entry->toggle )
            entry->toggle.Label(label);
        else if ( const auto button =
                    entry->element.try_as<MUXC::AppBarButton>() )
            button.Label(label);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI tool bar tool label", e);
    }

    InvalidateBestSize();
}

bool wxToolBar::MSWOnEffectiveLayoutDirectionChanged()
{
    if ( !m_winui || !m_winui->root )
        return false;

    try
    {
        m_winui->root.FlowDirection(
            GetLayoutDirection() == wxLayout_RightToLeft
                ? MUX::FlowDirection::RightToLeft
                : MUX::FlowDirection::LeftToRight);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI tool bar direction", e);
        return false;
    }

    return true;
}

void wxToolBar::OnDPIChanged(wxDPIChangedEvent& event)
{
    event.Skip();

    // The bitmaps are chosen for the scale of the window.
    if ( m_winui && m_winui->realized )
    {
        Rebuild();
        InvalidateBestSize();
    }
}

// ----------------------------------------------------------------------------
// what the user did
// ----------------------------------------------------------------------------

void wxToolBar::WinUIOnClick(wxToolBarToolBase *tool, bool checked)
{
    // Not a click when the program itself changes the state, and nothing
    // from a button whose tool is gone.
    if ( !m_winui || m_winui->syncing || !m_winui->Find(tool) ||
         !tool->IsEnabled() )
    {
        return;
    }

    const int toolid = tool->GetId();

    bool toggled = false;
    if ( tool->CanBeToggled() )
    {
        // A radio tool which is down stays down when clicked again.
        toggled = tool->GetKind() == wxITEM_RADIO ? true : checked;
        tool->Toggle(toggled);
        if ( toggled != checked )
            DoToggleTool(tool, toggled);
        UnToggleRadioGroup(tool);
    }

    // The handler may delete the tool, or this tool bar.
    const std::weak_ptr<wxToolBar *> weak = m_winui->owner;
    if ( OnLeftClick(toolid, toggled) )
        return;

    const auto owner = weak.lock();
    if ( !owner || !*owner || !m_winui->Find(tool) )
        return;

    // The program refused the change: back to what it was.
    if ( tool->CanBeToggled() )
    {
        tool->Toggle(!toggled);
        DoToggleTool(tool, !toggled);
    }
}

void wxToolBar::WinUIOnDropdown(wxToolBarToolBase *tool)
{
    if ( !m_winui || !m_winui->Find(tool) || !tool->IsEnabled() )
        return;

    wxCommandEvent event(wxEVT_TOOL_DROPDOWN, tool->GetId());
    event.SetEventObject(this);

    const std::weak_ptr<wxToolBar *> weak = m_winui->owner;
    if ( HandleWindowEvent(event) )
        return;

    const auto owner = weak.lock();
    if ( !owner || !*owner )
        return;

#if wxUSE_MENUS
    // As wxMSW does: the menu given to the tool, under it.
    wxWinUIToolBarImpl::Entry * const entry = m_winui->Find(tool);
    wxMenu * const menu = entry ? tool->GetDropdownMenu() : nullptr;
    if ( !menu )
        return;

    wxRect rect;
    if ( !m_winui->GetRect(entry->element, GetDPIScaleFactor(), &rect) )
        rect = wxRect(GetClientSize());

    PopupMenu(menu, rect.x, rect.GetBottom() + 1);
#endif // wxUSE_MENUS
}

// ----------------------------------------------------------------------------
// geometry
// ----------------------------------------------------------------------------

void wxToolBar::WinUIPlaceControls()
{
    if ( !m_winui || m_winui->placing )
        return;

    // Moving a control changes the layout again: once is enough.
    m_winui->placing = true;
    const double scale = GetDPIScaleFactor();
    for ( wxWinUIToolBarImpl::Entry& entry : m_winui->entries )
    {
        if ( !entry.place || !entry.tool )
            continue;

        wxControl * const control = entry.tool->GetControl();
        if ( !control )
            continue;

        // In the menu of the tools which do not fit, a window cannot follow
        // its place, which then takes no room in the menu.
        const bool overflowed =
            wxWinUIToolBarImpl::IsInOverflow(entry.element);
        try
        {
            const double height = overflowed ? 0 : entry.placeHeight;
            if ( entry.place.Height() != height )
                entry.place.Height(height);
        }
        catch ( const winrt::hresult_error& )
        {
        }

        wxRect rect;
        const bool shown = !overflowed &&
            m_winui->GetRect(entry.place, scale, &rect) && !rect.IsEmpty();
        if ( shown && control->GetRect() != rect )
            control->SetSize(rect);
        if ( control->IsShown() != shown )
            control->Show(shown);
    }
    m_winui->placing = false;
}

wxToolBarToolBase *
wxToolBar::FindToolForPosition(wxCoord x, wxCoord y) const
{
    if ( !m_winui )
        return nullptr;

    const double scale = GetDPIScaleFactor();
    for ( const wxWinUIToolBarImpl::Entry& entry : m_winui->entries )
    {
        wxRect rect;
        if ( !wxWinUIToolBarImpl::IsInOverflow(entry.element) &&
             m_winui->GetRect(entry.element, scale, &rect) &&
             rect.Contains(x, y) )
        {
            return entry.tool;
        }
    }

    return nullptr;
}

wxSize wxToolBar::DoGetBestSize() const
{
    const int thickness = FromDIP(BAR_DIP);
    const bool vertical = IsVertical();

    // What the tools take, known since they were made.
    if ( m_winui && m_winui->bestDip.x > 0 && m_winui->bestDip.y > 0 )
    {
        const wxSize best = FromDIP(m_winui->bestDip);
        return vertical ? wxSize(wxMax(thickness, best.x + FromDIP(8)),
                                 wxMax(thickness, best.y))
                        : wxSize(best.x, wxMax(thickness, best.y));
    }

    // Before that: a button for each tool.
    const int count = wxMax(1, static_cast<int>(GetToolsCount()));
    return vertical ? wxSize(thickness, count * thickness)
                    : wxSize(count * thickness, thickness);
}

// ----------------------------------------------------------------------------
// what the tests look at
// ----------------------------------------------------------------------------

#ifdef WXWINUI_TEST_SUPPORT

namespace
{

namespace MUXAPR = winrt::Microsoft::UI::Xaml::Automation::Provider;

wxWinUIToolBarImpl::Entry *
wxWinUIFindToolBarEntry(wxWinUIToolBarImpl *impl, int toolid)
{
    if ( !impl )
        return nullptr;

    for ( wxWinUIToolBarImpl::Entry& entry : impl->entries )
    {
        if ( entry.tool && !entry.tool->IsSeparator() &&
             entry.tool->GetId() == toolid )
        {
            return &entry;
        }
    }

    return nullptr;
}

} // anonymous namespace

bool wxWinUIToolBarTestAccess::InvokeTool(wxToolBar& toolbar,
                                          int toolid,
                                          bool dropdownPart)
{
    wxWinUIToolBarImpl::Entry * const
        entry = wxWinUIFindToolBarEntry(toolbar.m_winui.get(), toolid);
    if ( !entry )
        return false;

    // As a screen reader does it: through the automation peer of the
    // button, which refuses when the button is disabled.
    try
    {
        const MUX::UIElement target =
            dropdownPart ? MUX::UIElement(entry->arrow)
                         : MUX::UIElement(entry->element);
        if ( !target )
            return false;

        const auto peer =
            MUXAP::FrameworkElementAutomationPeer::CreatePeerForElement(
                target);
        if ( !peer || !peer.IsEnabled() )
            return false;

        if ( !dropdownPart && entry->toggle )
        {
            const auto provider = peer.try_as<MUXAPR::IToggleProvider>();
            if ( !provider )
                return false;
            provider.Toggle();
        }
        else
        {
            const auto provider = peer.try_as<MUXAPR::IInvokeProvider>();
            if ( !provider )
                return false;
            provider.Invoke();
        }
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }

    return true;
}

bool wxWinUIToolBarTestAccess::GetToolPeerState(const wxToolBar& toolbar,
                                                int toolid,
                                                PeerSnapshot *snapshot)
{
    wxWinUIToolBarImpl * const impl = toolbar.m_winui.get();
    wxWinUIToolBarImpl::Entry * const
        entry = wxWinUIFindToolBarEntry(impl, toolid);
    if ( !entry || !snapshot )
        return false;

    *snapshot = PeerSnapshot();
    try
    {
        snapshot->control = static_cast<bool>(entry->place);
        snapshot->dropdown = static_cast<bool>(entry->arrow);
        snapshot->hasIcon = static_cast<bool>(entry->icon);
        snapshot->usesDisabledBitmap = entry->usesDisabledBitmap;
        snapshot->overflowed =
            wxWinUIToolBarImpl::IsInOverflow(entry->element);
        impl->GetRect(entry->element, toolbar.GetDPIScaleFactor(),
                      &snapshot->bounds);

        if ( entry->icon )
            snapshot->iconOpacity = entry->icon.Opacity();

        if ( const auto control = entry->element.try_as<MUXC::Control>() )
            snapshot->enabled = control.IsEnabled();

        if ( entry->toggle )
        {
            const auto checked = entry->toggle.IsChecked();
            snapshot->toggled = checked && checked.Value();
            snapshot->label = wxWinUIFromHString(entry->toggle.Label());
            snapshot->showsText = entry->toggle.LabelPosition() ==
                MUXC::CommandBarLabelPosition::Default;
        }
        else if ( const auto button =
                    entry->element.try_as<MUXC::AppBarButton>() )
        {
            snapshot->label = wxWinUIFromHString(button.Label());
            snapshot->showsText = button.LabelPosition() ==
                MUXC::CommandBarLabelPosition::Default;
        }

        if ( entry->tool->IsButton() )
        {
            const auto tip = MUXC::ToolTipService::GetToolTip(entry->element);
            if ( tip )
            {
                snapshot->toolTip =
                    wxWinUIFromHString(winrt::unbox_value<winrt::hstring>(tip));
            }
        }
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }

    return true;
}

size_t wxWinUIToolBarTestAccess::GetPeerToolCount(const wxToolBar& toolbar)
{
    return toolbar.m_winui ? toolbar.m_winui->entries.size() : 0;
}

size_t
wxWinUIToolBarTestAccess::GetOverflowedToolCount(const wxToolBar& toolbar)
{
    size_t count = 0;
    if ( toolbar.m_winui )
    {
        for ( const wxWinUIToolBarImpl::Entry& entry :
                toolbar.m_winui->entries )
        {
            if ( wxWinUIToolBarImpl::IsInOverflow(entry.element) )
                ++count;
        }
    }

    return count;
}

size_t wxWinUIToolBarTestAccess::GetLiveCallbackStateCount()
{
    return gs_liveToolBars;
}

#endif // WXWINUI_TEST_SUPPORT

#endif // wxUSE_TOOLBAR
