/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/statbar.cpp
// Purpose:     wxWinUI wxStatusBar implementation
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_STATUSBAR

#include "wx/statusbr.h"

#ifndef WX_PRECOMP
    #include "wx/settings.h"
    #include "wx/window.h"
#endif

#include "private.h"

#include <vector>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXM = winrt::Microsoft::UI::Xaml::Media;

class wxWinUIStatusBarImpl
{
public:
    wxWinUIControlHost host;
    MUXC::Grid grid{ nullptr };
    std::vector<MUXC::TextBlock> fields;
};

// Note: wxIMPLEMENT_DYNAMIC_CLASS(wxStatusBar, wxWindow) is already provided by
// the common code in src/common/statbar.cpp, so it must not be repeated here.

wxStatusBar::wxStatusBar()
{
}

wxStatusBar::wxStatusBar(wxWindow *parent,
                         wxWindowID id,
                         long style,
                         const wxString& name)
{
    Create(parent, id, style, name);
}

wxStatusBar::~wxStatusBar() = default;

bool wxStatusBar::Create(wxWindow *parent,
                         wxWindowID id,
                         long style,
                         const wxString& name)
{
    if ( !wxControl::Create(parent, id, wxDefaultPosition, wxDefaultSize,
                            style | wxFULL_REPAINT_ON_RESIZE,
                            wxDefaultValidator, name) )
        return false;

    m_winui.reset(new wxWinUIStatusBarImpl);
    if ( !m_winui->host.Initialize(this) )
        return false;

    // Start with a single field like the other status bar implementations.
    SetFieldsCount(1);

    InvalidateBestSize();
    return true;
}

wxArrayInt wxStatusBar::GetAbsWidths() const
{
    int width = GetClientSize().x - 2 * m_borderX;
    if ( width < 0 )
        width = 0;

    return CalculateAbsWidths(width);
}

void wxStatusBar::RebuildContent()
{
    if ( !m_winui || !m_winui->host.IsOk() )
        return;

    try
    {
        const int count = GetFieldsCount();

        MUXC::Grid grid;
        grid.VerticalAlignment(MUX::VerticalAlignment::Stretch);
        grid.HorizontalAlignment(MUX::HorizontalAlignment::Stretch);

        const bool ellipsize = HasFlag(wxSTB_ELLIPSIZE_START |
                                       wxSTB_ELLIPSIZE_MIDDLE |
                                       wxSTB_ELLIPSIZE_END);

        // A subtle divider that reads on both light and dark Mica backdrops.
        const MUXM::SolidColorBrush divider = wxWinUIBrush(128, 128, 128, 90);

        m_winui->fields.clear();
        m_winui->fields.reserve(count);

        for ( int i = 0; i < count; ++i )
        {
            MUXC::ColumnDefinition col;
            const int paneWidth = i < GetFieldsCount() ? GetField(i).GetWidth()
                                                       : -1;
            if ( paneWidth > 0 )
                col.Width(MUX::GridLengthHelper::FromPixels(paneWidth));
            else
                col.Width(MUX::GridLengthHelper::FromValueAndType(
                    wxMax(1, -paneWidth), MUX::GridUnitType::Star));
            grid.ColumnDefinitions().Append(col);

            // Field container holding the text plus, on its right edge (except
            // for the last field), a short vertical divider that does NOT span
            // the full height -- like a modern status bar.
            MUXC::Grid cell;
            cell.VerticalAlignment(MUX::VerticalAlignment::Stretch);
            cell.HorizontalAlignment(MUX::HorizontalAlignment::Stretch);

            MUXC::TextBlock tb;
            tb.VerticalAlignment(MUX::VerticalAlignment::Center);
            tb.TextWrapping(MUX::TextWrapping::NoWrap);
            if ( ellipsize )
                tb.TextTrimming(MUX::TextTrimming::CharacterEllipsis);
            MUX::Thickness margin{};
            margin.Left = m_borderX + FromDIP(6);
            margin.Right = m_borderX + FromDIP(6);
            tb.Margin(margin);
            tb.Text(wxWinUIToHString(GetStatusText(i)));
            cell.Children().Append(tb);

            if ( i < count - 1 )
            {
                MUXC::Border sep;
                sep.Width(1);
                sep.HorizontalAlignment(MUX::HorizontalAlignment::Right);
                sep.VerticalAlignment(MUX::VerticalAlignment::Stretch);
                MUX::Thickness sepMargin{};
                sepMargin.Top = FromDIP(5);
                sepMargin.Bottom = FromDIP(5);
                sep.Margin(sepMargin);
                sep.Background(divider);
                cell.Children().Append(sep);
            }

            MUXC::Grid::SetColumn(cell, i);
            grid.Children().Append(cell);

            m_winui->fields.push_back(tb);
        }

        m_winui->grid = grid;
        m_winui->host.SetContent(grid);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI StatusBar content", e);
    }
}

void wxStatusBar::SetFieldsCount(int number, const int *widths)
{
    wxStatusBarBase::SetFieldsCount(number, widths);
    RebuildContent();
}

void wxStatusBar::SetStatusWidths(int n, const int widths[])
{
    wxStatusBarBase::SetStatusWidths(n, widths);
    RebuildContent();
}

void wxStatusBar::SetStatusStyles(int n, const int styles[])
{
    wxStatusBarBase::SetStatusStyles(n, styles);
    RebuildContent();
}

void wxStatusBar::DoUpdateStatusText(int number)
{
    if ( !m_winui )
        return;

    if ( number >= 0 && number < static_cast<int>(m_winui->fields.size()) )
    {
        try
        {
            m_winui->fields[number].Text(wxWinUIToHString(GetStatusText(number)));
            m_winui->host.ForceRender();
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI StatusBar text", e);
        }
    }
}

bool wxStatusBar::GetFieldRect(int n, wxRect& rect) const
{
    wxCHECK_MSG( (n >= 0) && (n < GetFieldsCount()), false,
                 wxT("invalid status bar field index") );

    const wxArrayInt widths = GetAbsWidths();
    if ( widths.IsEmpty() )
        return false;

    rect.x = m_borderX;
    for ( int i = 0; i < n; ++i )
        rect.x += widths[i];

    rect.y = m_borderY;
    rect.width = widths[n] - 2 * m_borderX;
    rect.height = GetClientSize().y - 2 * m_borderY;

    return true;
}

void wxStatusBar::SetMinHeight(int height)
{
    m_minHeight = height;
    InvalidateBestSize();
}

wxSize wxStatusBar::DoGetBestSize() const
{
    int width = 80;
    if ( GetParent() )
        GetParent()->GetClientSize(&width, nullptr);

    int height = (11 * GetCharHeight()) / 10 + 2 * m_borderY;
    height = wxMax(height, FromDIP(26));
    if ( m_minHeight > 0 )
        height = wxMax(height, m_minHeight);

    return wxSize(width, height);
}

#endif // wxUSE_STATUSBAR
