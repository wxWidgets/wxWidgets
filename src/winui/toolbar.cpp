/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/toolbar.cpp
// Purpose:     wxWinUI wxToolBar implementation (WinUI CommandBar)
// Author:      wxWidgets development team
// Created:     2026-07-20
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_TOOLBAR

#include "wx/toolbar.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
#endif

#include "private.h"

#include <vector>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

wxIMPLEMENT_DYNAMIC_CLASS(wxToolBar, wxControl);

// ----------------------------------------------------------------------------
// wxWinUIToolBarTool
// ----------------------------------------------------------------------------

class wxWinUIToolBarTool : public wxToolBarToolBase
{
public:
    wxWinUIToolBarTool(wxToolBar *tbar,
                       int toolid,
                       const wxString& label,
                       const wxBitmapBundle& bmpNormal,
                       const wxBitmapBundle& bmpDisabled,
                       wxItemKind kind,
                       wxObject *clientData,
                       const wxString& shortHelp,
                       const wxString& longHelp)
        : wxToolBarToolBase(tbar, toolid, label, bmpNormal, bmpDisabled, kind,
                            clientData, shortHelp, longHelp)
    {
    }

    wxWinUIToolBarTool(wxToolBar *tbar, wxControl *control, const wxString& label)
        : wxToolBarToolBase(tbar, control, label)
    {
    }

    // The realised WinUI element (AppBarButton, AppBarToggleButton or
    // AppBarSeparator), null until Realize().
    MUX::FrameworkElement element{ nullptr };
};

// ----------------------------------------------------------------------------
// wxWinUIToolBarImpl
// ----------------------------------------------------------------------------

class wxWinUIToolBarImpl
{
public:
    wxWinUIControlHost host;

    // Horizontal bars use a CommandBar, vertical ones a StackPanel of the
    // same AppBar elements (CommandBar itself is horizontal-only).
    MUXC::CommandBar bar{ nullptr };
    MUXC::StackPanel panel{ nullptr };
};

// ----------------------------------------------------------------------------
// wxToolBar
// ----------------------------------------------------------------------------

wxToolBar::wxToolBar()
{
}

wxToolBar::wxToolBar(wxWindow *parent,
                     wxWindowID id,
                     const wxPoint& pos,
                     const wxSize& size,
                     long style,
                     const wxString& name)
{
    Create(parent, id, pos, size, style, name);
}

wxToolBar::~wxToolBar() = default;

bool wxToolBar::Create(wxWindow *parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name)
{
    style = (style & ~wxBORDER_MASK) | wxBORDER_NONE;

    if ( !wxToolBarBase::Create(parent, id, pos, size, style,
                                wxDefaultValidator, name) )
        return false;

    FixupStyle();

    m_winui.reset(new wxWinUIToolBarImpl);
    if ( !m_winui->host.Initialize(this) )
        return false;

    try
    {
        if ( IsVertical() )
        {
            m_winui->panel = MUXC::StackPanel();
            m_winui->panel.Orientation(MUXC::Orientation::Vertical);
            m_winui->host.SetContent(m_winui->panel);
        }
        else
        {
            m_winui->bar = MUXC::CommandBar();
            m_winui->bar.IsOpen(false);
            m_winui->bar.IsSticky(false);

            MUXC::CommandBarDefaultLabelPosition labelPos =
                MUXC::CommandBarDefaultLabelPosition::Collapsed;
            if ( HasFlag(wxTB_HORZ_TEXT) )
                labelPos = MUXC::CommandBarDefaultLabelPosition::Right;
            else if ( HasFlag(wxTB_TEXT) )
                labelPos = MUXC::CommandBarDefaultLabelPosition::Bottom;
            m_winui->bar.DefaultLabelPosition(labelPos);

            if ( HasFlag(wxTB_NO_TOOLTIPS) )
            {
                // Nothing special: we simply don't set tooltips below.
            }

            m_winui->host.SetContent(m_winui->bar);
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI CommandBar creation", e);
        return false;
    }

    SetInitialSize(size);
    return true;
}

wxToolBarToolBase *wxToolBar::CreateTool(int toolid,
                                         const wxString& label,
                                         const wxBitmapBundle& bmpNormal,
                                         const wxBitmapBundle& bmpDisabled,
                                         wxItemKind kind,
                                         wxObject *clientData,
                                         const wxString& shortHelp,
                                         const wxString& longHelp)
{
    return new wxWinUIToolBarTool(this, toolid, label, bmpNormal, bmpDisabled,
                                  kind, clientData, shortHelp, longHelp);
}

wxToolBarToolBase *wxToolBar::CreateTool(wxControl *control,
                                         const wxString& label)
{
    return new wxWinUIToolBarTool(this, control, label);
}

bool wxToolBar::DoInsertTool(size_t WXUNUSED(pos),
                             wxToolBarToolBase *WXUNUSED(tool))
{
    // The peer is (re)built in Realize().
    return true;
}

bool wxToolBar::DoDeleteTool(size_t WXUNUSED(pos),
                             wxToolBarToolBase *tool)
{
    static_cast<wxWinUIToolBarTool *>(tool)->element = nullptr;
    RebuildPeer();
    return true;
}

void wxToolBar::DoEnableTool(wxToolBarToolBase *tool, bool enable)
{
    auto& element = static_cast<wxWinUIToolBarTool *>(tool)->element;
    if ( !element )
        return;

    try
    {
        if ( auto control = element.try_as<MUXC::Control>() )
            control.IsEnabled(enable);
    }
    catch ( const winrt::hresult_error& )
    {
    }
}

void wxToolBar::DoToggleTool(wxToolBarToolBase *tool, bool toggle)
{
    auto& element = static_cast<wxWinUIToolBarTool *>(tool)->element;
    if ( !element )
        return;

    try
    {
        if ( auto button = element.try_as<MUXC::AppBarToggleButton>() )
            button.IsChecked(toggle);
    }
    catch ( const winrt::hresult_error& )
    {
    }
}

void wxToolBar::DoSetToggle(wxToolBarToolBase *WXUNUSED(tool),
                            bool WXUNUSED(toggle))
{
    // Changing the kind of an existing tool is not supported.
}

bool wxToolBar::Realize()
{
    if ( !wxToolBarBase::Realize() )
        return false;

    RebuildPeer();

    InvalidateBestSize();
    return true;
}

void wxToolBar::RebuildPeer()
{
    if ( !m_winui || (!m_winui->bar && !m_winui->panel) )
        return;

    try
    {
        if ( m_winui->bar )
            m_winui->bar.PrimaryCommands().Clear();
        else
            m_winui->panel.Children().Clear();

        for ( wxToolBarToolsList::compatibility_iterator node = m_tools.GetFirst();
              node;
              node = node->GetNext() )
        {
            wxWinUIToolBarTool * const
                tool = static_cast<wxWinUIToolBarTool *>(node->GetData());

            MUX::FrameworkElement element{ nullptr };

            if ( tool->IsSeparator() || tool->IsStretchable() )
            {
                MUXC::AppBarSeparator separator;
                element = separator;
                if ( m_winui->bar )
                    m_winui->bar.PrimaryCommands().Append(separator);
            }
            else if ( tool->IsControl() )
            {
                // Embedded wx controls have their own HWND (and island): they
                // cannot live inside this island.  Leave a gap so the control,
                // positioned by the application, remains visible.
                wxLogDebug("wxToolBar: control tools are not supported by the "
                           "WinUI toolbar, the control will not be positioned");
                tool->element = nullptr;
                continue;
            }
            else // a normal, check or radio button
            {
                const int toolid = tool->GetId();

                MUXC::IconElement icon{ nullptr };
                const wxBitmap bitmap =
                    tool->GetNormalBitmapBundle().GetBitmapFor(this);
                if ( bitmap.IsOk() )
                {
                    if ( auto source = wxWinUIWriteableBitmapFromBitmap(bitmap) )
                    {
                        MUXC::ImageIcon imageIcon;
                        imageIcon.Source(source);
                        icon = imageIcon;
                    }
                }

                const winrt::hstring label = wxWinUIToHString(
                    wxWinUIRemoveMnemonics(tool->GetLabel()));

                if ( tool->CanBeToggled() )
                {
                    MUXC::AppBarToggleButton button;
                    if ( icon )
                        button.Icon(icon);
                    button.Label(label);
                    button.IsChecked(tool->IsToggled());
                    button.IsEnabled(tool->IsEnabled());
                    button.Click(
                        [this, toolid](winrt::Windows::Foundation::IInspectable const&,
                                       MUX::RoutedEventArgs const&)
                        {
                            OnToolClicked(toolid);
                        });
                    element = button;
                    if ( m_winui->bar )
                        m_winui->bar.PrimaryCommands().Append(button);
                }
                else
                {
                    MUXC::AppBarButton button;
                    if ( icon )
                        button.Icon(icon);
                    button.Label(label);
                    button.IsEnabled(tool->IsEnabled());
                    button.Click(
                        [this, toolid](winrt::Windows::Foundation::IInspectable const&,
                                       MUX::RoutedEventArgs const&)
                        {
                            OnToolClicked(toolid);
                        });
                    element = button;
                    if ( m_winui->bar )
                        m_winui->bar.PrimaryCommands().Append(button);
                }

                if ( !HasFlag(wxTB_NO_TOOLTIPS) && !tool->GetShortHelp().empty() )
                    wxWinUISetToolTip(element, tool->GetShortHelp());
            }

            if ( m_winui->panel && element )
                m_winui->panel.Children().Append(element.as<MUX::UIElement>());

            tool->element = element;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI CommandBar rebuild", e);
    }

    m_winui->host.ForceRender();
}

void wxToolBar::OnToolClicked(int toolid)
{
    wxToolBarToolBase * const tool = FindById(toolid);
    if ( !tool )
        return;

    bool toggled = tool->IsToggled();
    if ( tool->CanBeToggled() )
    {
        if ( tool->GetKind() == wxITEM_RADIO )
        {
            // A radio tool is always selected by a click (the WinUI toggle
            // button may have unchecked itself: force it back on).
            toggled = true;
            tool->Toggle(true);
            DoToggleTool(tool, true);
            UnToggleRadioGroup(tool);
        }
        else
        {
            // The WinUI control has already flipped its state.
            toggled = !toggled;
            tool->Toggle(toggled);
            DoToggleTool(tool, toggled);
        }
    }

    if ( !OnLeftClick(toolid, toggled) && tool->CanBeToggled() )
    {
        // The handler refused the change: restore the previous state.
        tool->Toggle(!toggled);
        DoToggleTool(tool, !toggled);
    }
}

wxToolBarToolBase *wxToolBar::FindToolForPosition(wxCoord x, wxCoord y) const
{
    // Approximate: assume evenly-sized items.
    const int itemExtent = FromDIP(48);
    const int index = IsVertical()
        ? (itemExtent > 0 ? y / itemExtent : 0)
        : (itemExtent > 0 ? x / itemExtent : 0);

    if ( index < 0 || static_cast<size_t>(index) >= m_tools.GetCount() )
        return nullptr;

    wxToolBarToolsList::compatibility_iterator node = m_tools.Item(index);
    return node ? node->GetData() : nullptr;
}

wxSize wxToolBar::DoGetBestSize() const
{
    // The standard closed CommandBar is 48 DIPs tall (64 with bottom labels).
    const int thickness =
        FromDIP(HasFlag(wxTB_TEXT) && !HasFlag(wxTB_HORZ_TEXT) ? 64 : 48);

    int length = 0;
    const int itemExtent = FromDIP(48);
    for ( wxToolBarToolsList::compatibility_iterator node = m_tools.GetFirst();
          node;
          node = node->GetNext() )
    {
        const wxToolBarToolBase * const tool = node->GetData();
        length += tool->IsSeparator() ? FromDIP(12) : itemExtent;
    }
    if ( length == 0 )
        length = itemExtent;

    // Leave room for the CommandBar "see more" affordance.
    if ( !IsVertical() )
        length += FromDIP(48);

    return IsVertical() ? wxSize(thickness, length)
                        : wxSize(length, thickness);
}

#endif // wxUSE_TOOLBAR
