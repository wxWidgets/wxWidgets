/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/collpane.cpp
// Purpose:     wxCollapsiblePane shown by a WinUI Expander
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_COLLPANE

#include "wx/collpane.h"

#ifndef WX_PRECOMP
    #include "wx/panel.h"
    #include "wx/sizer.h"
    #include "wx/toplevel.h"
#endif

#include "wx/weakref.h"

#include "private.h"

#include <cmath>
#include <limits>

namespace
{

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

// The part of a template with this name, below an element.
MUX::FrameworkElement
FindNamedPart(const MUX::DependencyObject& parent, const wchar_t *name)
{
    const int count = MUX::Media::VisualTreeHelper::GetChildrenCount(parent);
    for ( int i = 0; i < count; ++i )
    {
        const auto child = MUX::Media::VisualTreeHelper::GetChild(parent, i);
        if ( const auto element = child.try_as<MUX::FrameworkElement>() )
        {
            if ( element.Name() == name )
                return element;
        }
        if ( const auto found = FindNamedPart(child, name) )
            return found;
    }
    return nullptr;
}

} // anonymous namespace

class wxWinUICollapsiblePaneImpl
{
public:
    wxWinUIControlHost host;
    MUXC::Expander expander{ nullptr };
    // Stands for the pane in the content of the Expander: it gives the
    // content area, which the Expander frames and fills, the height of the
    // pane the window of wx is placed over.
    MUXC::Border filler{ nullptr };
    winrt::event_token expanding{};
    winrt::event_token collapsed{};
    winrt::event_token loaded{};
    // The state is being set by the program: not a change made by the user.
    bool syncing = false;

    // What the Expander asks for its header and keeps around its content, in
    // DIP. They are those of its template once it has one, which is when it
    // is loaded; until then the header is given the height it has in the
    // default style and the width of its text, of its chevron and of what
    // the default style puts around them.
    bool measured = false;
    double headerWidth = 0;
    double headerHeight = 48;
    MUX::Thickness padding{ 16, 16, 16, 16 };
};

wxIMPLEMENT_DYNAMIC_CLASS(wxCollapsiblePane, wxControl);

wxCollapsiblePane::wxCollapsiblePane() = default;

wxCollapsiblePane::wxCollapsiblePane(wxWindow *parent,
                                     wxWindowID winid,
                                     const wxString& label,
                                     const wxPoint& pos,
                                     const wxSize& size,
                                     long style,
                                     const wxValidator& val,
                                     const wxString& name)
{
    Create(parent, winid, label, pos, size, style, val, name);
}

wxCollapsiblePane::~wxCollapsiblePane()
{
    if ( m_winui && m_winui->expander )
    {
        try
        {
            m_winui->expander.Expanding(m_winui->expanding);
            m_winui->expander.Collapsed(m_winui->collapsed);
            m_winui->expander.Loaded(m_winui->loaded);
        }
        catch ( const winrt::hresult_error& )
        {
        }
    }
}

bool wxCollapsiblePane::Create(wxWindow *parent,
                               wxWindowID winid,
                               const wxString& label,
                               const wxPoint& pos,
                               const wxSize& size,
                               long style,
                               const wxValidator& val,
                               const wxString& name)
{
    if ( !wxControl::Create(parent, winid, pos, size, style, val, name) )
        return false;

    m_label = label;

    // As in the generic control: placed by Layout(), hidden while collapsed.
    m_pane = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                         wxTAB_TRAVERSAL | wxNO_BORDER,
                         wxS("wxCollapsiblePanePane"));
    m_pane->Hide();

    m_winui.reset(new wxWinUICollapsiblePaneImpl);
    if ( !m_winui->host.Initialize(this) )
        return false;

    try
    {
        MUXC::Expander expander;
        expander.Header(winrt::box_value(
            wxWinUIToHString(wxControl::RemoveMnemonics(label))));
        expander.IsExpanded(false);
        // The least height of an Expander is the height of its header (48 in
        // its style, where the header reads it). A control given to a wx
        // window loses the minimum sizes of its style, unless it has them as
        // local values: without this one the header was only as high as its
        // chevron, 32, with nothing above and under its text.
        expander.MinHeight(48);
        expander.HorizontalAlignment(MUX::HorizontalAlignment::Stretch);
        expander.HorizontalContentAlignment(MUX::HorizontalAlignment::Stretch);
        expander.VerticalAlignment(MUX::VerticalAlignment::Top);

        MUXC::Border filler;
        filler.Height(0);
        filler.IsHitTestVisible(false);
        expander.Content(filler);

        // The state changes inside an event of XAML: what follows for wx
        // (the pane shown, the window resized, the event of the program)
        // is done once that event is over.
        const wxWeakRef<wxCollapsiblePane> weak(this);
        const auto toggled = [weak](bool expanded)
        {
            wxCollapsiblePane * const self = weak.get();
            if ( !self || !self->m_winui || self->m_winui->syncing )
                return;
            self->CallAfter([weak, expanded]()
            {
                if ( wxCollapsiblePane * const live = weak.get() )
                    live->WinUIUserToggled(expanded);
            });
        };
        m_winui->expanding = expander.Expanding(
            [toggled](const MUXC::Expander&,
                      const MUXC::ExpanderExpandingEventArgs&)
            {
                toggled(true);
            });
        m_winui->collapsed = expander.Collapsed(
            [toggled](const MUXC::Expander&,
                      const MUXC::ExpanderCollapsedEventArgs&)
            {
                toggled(false);
            });

        // The sizes of the header are known for good once it has its
        // template: what is laid out around this control follows.
        m_winui->loaded = expander.Loaded(
            [weak](const winrt::Windows::Foundation::IInspectable&,
                   const MUX::RoutedEventArgs&)
            {
                wxCollapsiblePane * const self = weak.get();
                if ( !self )
                    return;
                self->CallAfter([weak]()
                {
                    wxCollapsiblePane * const live = weak.get();
                    if ( !live || !live->m_winui )
                        return;
                    const wxSize before = live->GetBestSize();
                    live->m_winui->measured = false;
                    live->InvalidateBestSize();
                    live->Layout();
                    if ( live->GetBestSize() != before )
                        live->PostSizeEventToParent();
                });
            });

        m_winui->expander = expander;
        m_winui->filler = filler;
        if ( !m_winui->host.SetContent(expander) )
            return false;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI Expander creation", e);
        return false;
    }

    Bind(wxEVT_SIZE, &wxCollapsiblePane::OnSize, this);

    InvalidateBestSize();
    SetInitialSize(size);
    return true;
}

void wxCollapsiblePane::UpdateHeaderMetrics() const
{
    if ( !m_winui || m_winui->measured )
        return;

    // Until the template is there: the text, 16 before it, and the chevron
    // (32) with 8 on each side of it.
    const double scale = GetDPIScaleFactor();
    m_winui->headerWidth =
        GetTextExtent(wxControl::RemoveMnemonics(m_label)).x / scale + 64;

    try
    {
        if ( !m_winui->expander.IsLoaded() )
            return;

        const auto header = FindNamedPart(m_winui->expander, L"ExpanderHeader");
        if ( !header )
            return;

        const float unbounded = std::numeric_limits<float>::infinity();
        header.Measure({ unbounded, unbounded });
        const auto size = header.DesiredSize();
        if ( size.Height > 0 && size.Width > 0 )
        {
            m_winui->headerWidth = size.Width;
            m_winui->headerHeight = size.Height;
            m_winui->measured = true;
        }
        header.InvalidateMeasure();

        const MUX::Thickness padding = m_winui->expander.Padding();
        if ( padding.Left > 0 || padding.Top > 0 ||
             padding.Right > 0 || padding.Bottom > 0 )
        {
            m_winui->padding = padding;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI Expander measure", e);
    }
}

wxSize wxCollapsiblePane::DoGetBestClientSize() const
{
    if ( !m_winui )
        return wxSize(0, 0);

    UpdateHeaderMetrics();

    const double scale = GetDPIScaleFactor();
    const auto px = [scale](double dip)
    {
        return static_cast<int>(std::ceil(dip * scale));
    };

    wxSize best(px(m_winui->headerWidth), px(m_winui->headerHeight));
    if ( !m_collapsed && m_pane )
    {
        // The content of an Expander: its padding around the pane and the
        // line which closes its frame.
        const wxSize pane = m_pane->GetBestSize();
        const MUX::Thickness& padding = m_winui->padding;
        best.x = wxMax(best.x,
                       pane.x + px(padding.Left + padding.Right + 2));
        best.y += pane.y + px(padding.Top + padding.Bottom + 1);
    }
    return best;
}

bool wxCollapsiblePane::Layout()
{
    if ( !m_winui || !m_pane )
        return false;

    UpdateHeaderMetrics();

    const double scale = GetDPIScaleFactor();
    const auto px = [scale](double dip)
    {
        return static_cast<int>(std::lround(dip * scale));
    };

    const wxSize client = GetClientSize();
    const MUX::Thickness& padding = m_winui->padding;
    const int header = px(m_winui->headerHeight);
    const int left = px(padding.Left + 1);
    const int top = header + px(padding.Top);
    const int width = client.x - left - px(padding.Right + 1);
    const int height = client.y - top - px(padding.Bottom + 1);

    try
    {
        // The content area of the Expander is as high as the pane.
        m_winui->filler.Height(wxMax(0, height) / scale);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI Expander layout", e);
    }

    if ( !m_collapsed )
    {
        m_pane->SetSize(left, top, wxMax(0, width), wxMax(0, height));
        m_pane->Layout();
    }
    return true;
}

void wxCollapsiblePane::OnSize(wxSizeEvent& event)
{
    Layout();
    event.Skip();
}

void wxCollapsiblePane::OnStateChange(const wxSize& sizeNew)
{
    SetSize(sizeNew);

    if ( HasFlag(wxCP_NO_TLW_RESIZE) )
    {
        // the program handles the resizing itself
        return;
    }

    wxTopLevelWindow * const top =
        wxDynamicCast(wxGetTopLevelParent(this), wxTopLevelWindow);
    if ( !top )
        return;

    wxSizer * const sizer = top->GetSizer();
    if ( !sizer )
        return;

    const wxSize newBestSize = sizer->ComputeFittingClientSize(top);
    top->SetMinClientSize(newBestSize);

    // a maximized window is never resized, whatever happens
    if ( !top->IsMaximized() )
        top->SetClientSize(newBestSize);
}

void wxCollapsiblePane::Collapse(bool collapse)
{
    if ( m_collapsed == collapse || !m_winui || !m_pane )
        return;

    m_collapsed = collapse;
    InvalidateBestSize();
    m_pane->Show(!collapse);

    try
    {
        m_winui->syncing = true;
        m_winui->expander.IsExpanded(!collapse);
        m_winui->syncing = false;
    }
    catch ( const winrt::hresult_error& e )
    {
        m_winui->syncing = false;
        wxWinUILogException("WinUI Expander state", e);
    }

    OnStateChange(GetBestSize());
    Layout();
}

void wxCollapsiblePane::WinUIUserToggled(bool expanded)
{
    if ( !m_winui || !m_pane || m_collapsed == !expanded )
        return;

    m_collapsed = !expanded;
    InvalidateBestSize();
    m_pane->Show(expanded);

    // Already so when the Expander itself told it; not when the change comes
    // by another way, as from a test.
    try
    {
        if ( m_winui->expander.IsExpanded() != expanded )
        {
            m_winui->syncing = true;
            m_winui->expander.IsExpanded(expanded);
            m_winui->syncing = false;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        m_winui->syncing = false;
        wxWinUILogException("WinUI Expander state", e);
    }

    OnStateChange(GetBestSize());
    Layout();

    // made by the user: the program is told
    wxCollapsiblePaneEvent event(this, GetId(), m_collapsed);
    GetEventHandler()->ProcessEvent(event);
}

void wxCollapsiblePane::SetLabel(const wxString& label)
{
    m_label = label;
    if ( !m_winui )
        return;

    try
    {
        m_winui->expander.Header(winrt::box_value(
            wxWinUIToHString(wxControl::RemoveMnemonics(label))));
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI Expander label", e);
    }

    m_winui->measured = false;
    InvalidateBestSize();
    Layout();
}

#endif // wxUSE_COLLPANE
