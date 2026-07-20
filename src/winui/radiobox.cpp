/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/radiobox.cpp
// Purpose:     wxWinUI wxRadioBox implementation
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_RADIOBOX

#include "wx/radiobox.h"
#include "wx/winui/winui.h"

#ifndef WX_PRECOMP
    #include "wx/event.h"
    #include "wx/arrstr.h"
#endif

#include "private.h"

#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif

#include <cmath>
#include <limits>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

#if wxUSE_EXTENDED_RTTI
// The XTI version is defined in src/common/radiocmn.cpp.
#else
wxIMPLEMENT_DYNAMIC_CLASS(wxRadioBox, wxControl);
#endif

namespace
{
// Whether new radio boxes draw the Win32-style titled frame.
bool gs_radioBoxBorder = true;
} // namespace

void wxWinUISetRadioBoxBorder(bool useBorder) { gs_radioBoxBorder = useBorder; }
bool wxWinUIGetRadioBoxBorder() { return gs_radioBoxBorder; }

class wxWinUIRadioBoxImpl
{
public:
    wxWinUIControlHost host;
    MUX::UIElement root{ nullptr };   // the element hosted (border or panel)
    std::vector<MUXC::RadioButton> buttons;
};

wxRadioBox::wxRadioBox()
{
}

wxRadioBox::wxRadioBox(wxWindow *parent, wxWindowID id, const wxString& title,
                       const wxPoint& pos, const wxSize& size,
                       int n, const wxString choices[], int majorDim,
                       long style, const wxValidator& validator,
                       const wxString& name)
{
    Create(parent, id, title, pos, size, n, choices, majorDim, style,
           validator, name);
}

wxRadioBox::wxRadioBox(wxWindow *parent, wxWindowID id, const wxString& title,
                       const wxPoint& pos, const wxSize& size,
                       const wxArrayString& choices, int majorDim,
                       long style, const wxValidator& validator,
                       const wxString& name)
{
    Create(parent, id, title, pos, size, choices, majorDim, style,
           validator, name);
}

wxRadioBox::~wxRadioBox() = default;

bool wxRadioBox::Create(wxWindow *parent, wxWindowID id, const wxString& title,
                        const wxPoint& pos, const wxSize& size,
                        int n, const wxString choices[], int majorDim,
                        long style, const wxValidator& validator,
                        const wxString& name)
{
    m_strings.Clear();
    for ( int i = 0; i < n; ++i )
        m_strings.Add(choices[i]);
    return DoCreate(parent, id, title, pos, size, majorDim, style,
                    validator, name);
}

bool wxRadioBox::Create(wxWindow *parent, wxWindowID id, const wxString& title,
                        const wxPoint& pos, const wxSize& size,
                        const wxArrayString& choices, int majorDim,
                        long style, const wxValidator& validator,
                        const wxString& name)
{
    m_strings = choices;
    return DoCreate(parent, id, title, pos, size, majorDim,
                    style, validator, name);
}

bool wxRadioBox::DoCreate(wxWindow *parent, wxWindowID id, const wxString& title,
                          const wxPoint& pos, const wxSize& size, int majorDim,
                          long style, const wxValidator& validator,
                          const wxString& name)
{
    style = (style & ~wxBORDER_MASK) | wxBORDER_NONE;

    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    wxControl::SetLabel(title);

    SetMajorDim(majorDim == 0 ? static_cast<int>(m_strings.GetCount()) : majorDim,
                style);

    const size_t count = m_strings.GetCount();
    m_itemEnabled.assign(count, true);
    m_itemShown.assign(count, true);
    m_selection = count ? 0 : wxNOT_FOUND;

    m_winui.reset(new wxWinUIRadioBoxImpl);
    if ( !m_winui->host.Initialize(this) )
        return false;

    try
    {
        RebuildItems();
        ApplyToolTip();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI RadioBox creation", e);
        return false;
    }

    SetInitialSize(size);
    return true;
}

unsigned int wxRadioBox::GetCount() const
{
    return m_strings.GetCount();
}

wxString wxRadioBox::GetString(unsigned int n) const
{
    wxCHECK_MSG( n < m_strings.GetCount(), wxString(), wxT("invalid radiobox index") );
    return m_strings[n];
}

void wxRadioBox::SetString(unsigned int n, const wxString& s)
{
    wxCHECK_RET( n < m_strings.GetCount(), wxT("invalid radiobox index") );
    m_strings[n] = s;
    RebuildItems();
}

void wxRadioBox::SetSelection(int n)
{
    wxCHECK_RET( n == wxNOT_FOUND ||
                 (n >= 0 && static_cast<unsigned int>(n) < m_strings.GetCount()),
                 wxT("invalid radiobox index") );

    m_selection = n;
    if ( m_winui && n >= 0 && static_cast<size_t>(n) < m_winui->buttons.size() )
    {
        m_updating = true;
        m_winui->buttons[n].IsChecked(true);
        m_updating = false;
        m_winui->host.ForceRender();
    }
}

int wxRadioBox::GetSelection() const
{
    return m_selection;
}

bool wxRadioBox::Enable(unsigned int n, bool enable)
{
    wxCHECK_MSG( n < m_itemEnabled.size(), false, wxT("invalid radiobox index") );
    m_itemEnabled[n] = enable;
    if ( m_winui && n < m_winui->buttons.size() && m_winui->buttons[n] )
    {
        m_winui->buttons[n].IsEnabled(enable);
        m_winui->host.ForceRender();
    }
    return true;
}

bool wxRadioBox::Show(unsigned int n, bool show)
{
    wxCHECK_MSG( n < m_itemShown.size(), false, wxT("invalid radiobox index") );
    m_itemShown[n] = show;
    if ( m_winui && n < m_winui->buttons.size() && m_winui->buttons[n] )
    {
        m_winui->buttons[n].Visibility(show ? MUX::Visibility::Visible
                                            : MUX::Visibility::Collapsed);
        m_winui->host.ForceRender();
    }
    return true;
}

void wxRadioBox::DoEnable(bool enable)
{
    wxControl::DoEnable(enable);

    if ( !m_winui )
        return;

    for ( size_t i = 0; i < m_winui->buttons.size(); ++i )
    {
        // When disabling the whole box, disable every item; when re-enabling,
        // restore each item's own enabled state.
        const bool itemEnabled = enable &&
            (i < m_itemEnabled.size() ? m_itemEnabled[i] : true);
        m_winui->buttons[i].IsEnabled(itemEnabled);
    }
    m_winui->host.ForceRender();
}

bool wxRadioBox::IsItemEnabled(unsigned int n) const
{
    wxCHECK_MSG( n < m_itemEnabled.size(), false, wxT("invalid radiobox index") );
    return m_itemEnabled[n];
}

bool wxRadioBox::IsItemShown(unsigned int n) const
{
    wxCHECK_MSG( n < m_itemShown.size(), false, wxT("invalid radiobox index") );
    return m_itemShown[n];
}

wxSize wxRadioBox::DoGetBestSize() const
{
    // Measure the actual radio buttons (a single control measures reliably,
    // unlike the whole detached panel) to get the real per-item size including
    // the WinUI glyph + spacing + margins, then lay out the grid and frame.
    // WinUI radio buttons are wider than a hand-rolled estimate, which otherwise
    // caused the last column to be clipped.
    int itemW = 0;
    int itemH = FromDIP(30);

    if ( m_winui && !m_winui->buttons.empty() )
    {
        const float inf = std::numeric_limits<float>::infinity();
        for ( auto& button : m_winui->buttons )
        {
            try
            {
                button.Measure({ inf, inf });
                const auto d = button.DesiredSize();   // includes the margins
                itemW = wxMax(itemW, static_cast<int>(std::ceil(d.Width)));
                itemH = wxMax(itemH, static_cast<int>(std::ceil(d.Height)));
            }
            catch ( const winrt::hresult_error& )
            {
            }
        }
    }

    if ( itemW <= 0 )   // measuring failed: fall back to a generous estimate
    {
        int maxText = 0;
        for ( unsigned int i = 0; i < m_strings.GetCount(); ++i )
            maxText = wxMax(maxText, GetTextExtent(m_strings[i]).x);
        itemW = maxText + FromDIP(60);
    }

    const int cols = wxMax(1, GetColumnCount());
    const int rows = wxMax(1, GetRowCount());

    int w = cols * itemW;
    int h = rows * itemH;

    if ( gs_radioBoxBorder )
    {
        w += FromDIP(2 * 12 + 2);              // frame left/right padding + border
        h += FromDIP(14 + 12 + 2);             // frame top/bottom padding + border
        if ( !GetLabel().empty() )
            h += FromDIP(8);                   // frame top margin under the title
    }
    else if ( !GetLabel().empty() )
    {
        h += FromDIP(24);                      // plain header height
    }

    return wxSize(w, h);
}

void wxRadioBox::RebuildItems()
{
    if ( !m_winui || !m_winui->host.IsOk() )
        return;

    m_updating = true;
    try
    {
        const int count = static_cast<int>(m_strings.GetCount());
        const bool specifyRows = (GetWindowStyle() & wxRA_SPECIFY_ROWS) != 0;
        const int cols = wxMax(1, GetColumnCount());
        const int rows = wxMax(1, GetRowCount());

        // A group name unique to this radio box keeps its buttons mutually
        // exclusive without interfering with other radio boxes/buttons.
        const winrt::hstring groupName =
            wxWinUIToHString(wxString::Format("wxRadioBox_%p",
                                              static_cast<void *>(this)));

        MUXC::Grid grid;
        for ( int c = 0; c < cols; ++c )
        {
            MUXC::ColumnDefinition col;
            col.Width(MUX::GridLengthHelper::Auto());
            grid.ColumnDefinitions().Append(col);
        }
        for ( int r = 0; r < rows; ++r )
        {
            MUXC::RowDefinition row;
            row.Height(MUX::GridLengthHelper::Auto());
            grid.RowDefinitions().Append(row);
        }

        m_winui->buttons.clear();
        m_winui->buttons.reserve(count);

        for ( int i = 0; i < count; ++i )
        {
            // wx fills row-major for wxRA_SPECIFY_COLS and column-major for
            // wxRA_SPECIFY_ROWS; honour that exactly so the layout matches the
            // other toolkits.
            int row, colpos;
            if ( specifyRows )
            {
                row = i % rows;
                colpos = i / rows;
            }
            else
            {
                row = i / cols;
                colpos = i % cols;
            }

            MUXC::RadioButton button;
            button.Content(winrt::box_value(wxWinUIToHString(
                wxControl::GetLabelText(m_strings[i]))));
            button.GroupName(groupName);
            if ( i < static_cast<int>(m_itemEnabled.size()) )
                button.IsEnabled(m_itemEnabled[i]);
            if ( i < static_cast<int>(m_itemShown.size()) && !m_itemShown[i] )
                button.Visibility(MUX::Visibility::Collapsed);
            if ( i == m_selection )
                button.IsChecked(true);

            MUX::Thickness m{};
            m.Left = FromDIP(2);
            m.Right = FromDIP(12);
            m.Top = FromDIP(2);
            m.Bottom = FromDIP(2);
            button.Margin(m);

            button.Checked(
                [this, i](winrt::Windows::Foundation::IInspectable const&,
                          MUX::RoutedEventArgs const&)
                {
                    if ( !m_winui || m_updating )
                        return;
                    if ( i == m_selection )
                        return;
                    m_selection = i;
                    SendSelectionEvent();
                });

            MUXC::Grid::SetRow(button, row);
            MUXC::Grid::SetColumn(button, colpos);
            grid.Children().Append(button);
            m_winui->buttons.push_back(button);
        }

        const wxString title = wxControl::GetLabelText(GetLabel());

        if ( gs_radioBoxBorder )
        {
            // Match the wxStaticBox "group box" look exactly: a thin rounded
            // frame whose top line is masked by an opaque, theme-coloured title
            // chip (the classic notched group box).
            const bool dark = wxWinUIIsDarkTheme();

            MUXC::Grid root;

            MUXC::Border frame;
            MUX::Thickness bt{};
            bt.Left = bt.Top = bt.Right = bt.Bottom = 1;
            frame.BorderThickness(bt);
            MUX::CornerRadius cr{};
            cr.TopLeft = cr.TopRight = cr.BottomLeft = cr.BottomRight = 8;
            frame.CornerRadius(cr);
            frame.BorderBrush(dark ? wxWinUIBrush(255, 255, 255, 38)
                                   : wxWinUIBrush(0, 0, 0, 38));
            MUX::Thickness fm{};
            fm.Top = title.empty() ? 0.0 : FromDIP(8);
            frame.Margin(fm);
            MUX::Thickness fp{};
            fp.Left = fp.Right = FromDIP(12);
            fp.Top = FromDIP(title.empty() ? 10 : 14);
            fp.Bottom = FromDIP(12);
            frame.Padding(fp);
            frame.Child(grid);
            root.Children().Append(frame);

            if ( !title.empty() )
            {
                MUXC::Border labelBg;
                labelBg.Background(dark ? wxWinUIBrush(32, 32, 32)
                                        : wxWinUIBrush(243, 243, 243));
                labelBg.VerticalAlignment(MUX::VerticalAlignment::Top);
                labelBg.HorizontalAlignment(MUX::HorizontalAlignment::Left);
                MUX::Thickness lm{};
                lm.Left = FromDIP(8);
                labelBg.Margin(lm);
                MUX::Thickness lp{};
                lp.Left = lp.Right = FromDIP(4);
                labelBg.Padding(lp);

                MUXC::TextBlock text;
                text.Text(wxWinUIToHString(title));
                labelBg.Child(text);
                root.Children().Append(labelBg);
            }

            m_winui->root = root;
        }
        else
        {
            // Flat look: optional title above the items, no frame.
            MUXC::StackPanel panel;
            panel.Orientation(MUXC::Orientation::Vertical);
            if ( !title.empty() )
            {
                MUXC::TextBlock header;
                header.Text(wxWinUIToHString(title));
                MUX::Thickness hm{};
                hm.Bottom = FromDIP(6);
                header.Margin(hm);
                panel.Children().Append(header);
            }
            panel.Children().Append(grid);
            m_winui->root = panel;
        }

        m_winui->host.SetContent(m_winui->root);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI RadioBox item update", e);
    }
    m_updating = false;
    m_winui->host.ForceRender();
}

void wxRadioBox::ApplyToolTip()
{
#if wxUSE_TOOLTIPS
    if ( m_winui && m_winui->root )
        wxWinUISetToolTip(m_winui->root, m_tooltipText);
#endif // wxUSE_TOOLTIPS
}

#if wxUSE_TOOLTIPS
void wxRadioBox::DoSetToolTipText(const wxString& tip)
{
    m_tooltipText = tip;
    ApplyToolTip();
}

void wxRadioBox::DoSetToolTip(wxToolTip *tip)
{
    m_tooltipText = tip ? tip->GetTip() : wxString();
    delete tip;
    ApplyToolTip();
}
#endif // wxUSE_TOOLTIPS

int wxRadioBox::FindSelectedItem() const
{
    if ( !m_winui )
        return wxNOT_FOUND;

    for ( size_t i = 0; i < m_winui->buttons.size(); ++i )
    {
        const auto checked = m_winui->buttons[i].IsChecked();
        if ( checked && checked.Value() )
            return static_cast<int>(i);
    }

    return wxNOT_FOUND;
}

void wxRadioBox::SendSelectionEvent()
{
    wxCommandEvent event(wxEVT_RADIOBOX, GetId());
    event.SetEventObject(this);
    event.SetInt(m_selection);
    if ( m_selection != wxNOT_FOUND )
        event.SetString(m_strings[m_selection]);
    ProcessCommand(event);
}

#endif // wxUSE_RADIOBOX
