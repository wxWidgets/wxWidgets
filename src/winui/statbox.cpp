/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/statbox.cpp
// Purpose:     wxWinUI wxStaticBox implementation
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_STATBOX

#include "wx/statbox.h"

#ifndef WX_PRECOMP
    #include "wx/sizer.h"
#endif

#include "private.h"
#include "wx/winui/private/appearance.h"
#include "wx/weakref.h"

#ifdef WXWINUI_TEST_SUPPORT
    #include "static-test-access.h"
#endif

#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>

class wxWinUIStaticBoxImpl
{
public:
    ~wxWinUIStaticBoxImpl()
    {
        Close();
    }

    void Close()
    {
        host.Close();
        title = nullptr;
        frame = nullptr;
        root = nullptr;
        automationRoot = nullptr;
    }

    wxWinUIControlHost host;
    winrt::Microsoft::UI::Xaml::Controls::Border root{ nullptr };
    winrt::Microsoft::UI::Xaml::Controls::Grid automationRoot{ nullptr };
    winrt::Microsoft::UI::Xaml::Controls::Border frame{ nullptr };
    winrt::Microsoft::UI::Xaml::Controls::TextBlock title{ nullptr };
    double frameTopDIP = 0.0;
    double titleGapDIP = 0.0;
    unsigned long long modelRevision = 1;
    bool usesThemeBrush = false;
    bool projectionRetryScheduled = false;
};

namespace
{

void wxWinUIBumpStaticBoxModelRevision(wxWinUIStaticBoxImpl& impl)
{
    if ( ++impl.modelRevision == 0 )
        ++impl.modelRevision;
}

} // anonymous namespace

wxStaticBox::wxStaticBox()
{
}

wxStaticBox::wxStaticBox(wxWindow *parent,
                         wxWindowID id,
                         const wxString& label,
                         const wxPoint& pos,
                         const wxSize& size,
                         long style,
                         const wxString& name)
{
    Create(parent, id, label, pos, size, style, name);
}

wxStaticBox::wxStaticBox(wxWindow *parent,
                         wxWindowID id,
                         wxWindow *label,
                         const wxPoint& pos,
                         const wxSize& size,
                         long style,
                         const wxString& name)
{
    Create(parent, id, label, pos, size, style, name);
}

wxStaticBox::~wxStaticBox()
{
    if ( m_winui )
        m_winui->Close();
}

bool wxStaticBox::Create(wxWindow *parent,
                         wxWindowID id,
                         const wxString& label,
                         const wxPoint& pos,
                         const wxSize& size,
                         long style,
                         const wxString& name)
{
    if ( !wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    wxControl::SetLabel(label);

    m_winui.reset(new wxWinUIStaticBoxImpl);
    if ( !m_winui->host.Initialize(this) )
    {
        m_winui.reset();
        return false;
    }

    wxWinUIStaticBoxImpl * const impl = m_winui.get();
    wxWeakRef<wxStaticBox> alive(this);
    if ( !UpdateWinUIContent() )
    {
        if ( alive && alive->m_winui.get() == impl )
            alive->m_winui.reset();
        return false;
    }
    if ( !alive || alive->m_winui.get() != impl )
        return false;

    alive->SetInitialSize(size);
    return alive && alive->m_winui.get() == impl;
}

bool wxStaticBox::Create(wxWindow *parent,
                         wxWindowID id,
                         wxWindow *label,
                         const wxPoint& pos,
                         const wxSize& size,
                         long style,
                         const wxString& name)
{
    wxCHECK_MSG( label, false, wxS("Label window can't be null") );

    if ( !Create(parent, id, wxString(), pos, size, style, name) )
        return false;

    wxWinUIStaticBoxImpl * const impl = m_winui.get();
    wxWeakRef<wxStaticBox> alive(this);
    m_labelWin = label;
    wxWinUIBumpStaticBoxModelRevision(*impl);
    m_labelWin->Reparent(this);
    if ( !alive || alive->m_winui.get() != impl )
        return false;

    alive->PositionLabelWindow();
    if ( !alive || alive->m_winui.get() != impl )
        return false;

    // Create(string) built the frame before m_labelWin existed. Rebuild once
    // after the owned label has its final size so the top edge contains the
    // correct gap from the very first rendered frame.
    if ( !alive->UpdateWinUIContent() )
        return false;
    if ( !alive || alive->m_winui.get() != impl )
        return false;

    alive->SetInitialSize(size);
    return alive && alive->m_winui.get() == impl;
}

void wxStaticBox::SetLabel(const wxString& label)
{
    wxControl::SetLabel(label);
    if ( m_winui )
        wxWinUIBumpStaticBoxModelRevision(*m_winui);
    InvalidateBestSize();
    UpdateWinUIContent();
}

bool wxStaticBox::SetBackgroundColour(const wxColour& colour)
{
    const bool rc = wxStaticBoxBase::SetBackgroundColour(colour);

    if ( rc && m_winui )
        wxWinUIBumpStaticBoxModelRevision(*m_winui);
    UpdateWinUIContent();
    return rc;
}

bool wxStaticBox::SetForegroundColour(const wxColour& colour)
{
    const bool rc =
        wxCompositeWindowSettersOnly<wxStaticBoxBase>::SetForegroundColour(
            colour);

    if ( rc && m_winui )
        wxWinUIBumpStaticBoxModelRevision(*m_winui);
    UpdateWinUIContent();
    return rc;
}

bool wxStaticBox::SetFont(const wxFont& font)
{
    const bool rc =
        wxCompositeWindowSettersOnly<wxStaticBoxBase>::SetFont(font);

    if ( rc && m_winui )
        wxWinUIBumpStaticBoxModelRevision(*m_winui);
    if ( rc && m_labelWin )
        PositionLabelWindow();

    if ( rc )
        InvalidateBestSize();
    UpdateWinUIContent();
    return rc;
}

void wxStaticBox::GetBordersForSizer(int *borderTop, int *borderOther) const
{
    const int margin = FromDIP(10);

    if ( m_labelWin )
        *borderTop = m_labelWin->GetBestSize().y + FromDIP(6);
    else if ( !GetLabel().empty() )
        *borderTop = GetCharHeight() + FromDIP(8);
    else
        *borderTop = margin;

    *borderOther = margin;
}

wxSize wxStaticBox::DoGetBestSize() const
{
    wxSize best;

    if ( GetSizer() )
        best = ClientToWindowSize(GetSizer()->CalcMin());

    const wxString label = GetLabelText(GetLabel());
    if ( !label.empty() )
    {
        wxSize labelSize = GetTextExtent(label);
        labelSize.x += FromDIP(30);
        labelSize.y += FromDIP(24);
        best.IncTo(labelSize);
    }

    if ( best.x <= 0 || best.y <= 0 )
        best = FromDIP(wxSize(80, 48));

    return best;
}

wxWindowList wxStaticBox::GetCompositeWindowParts() const
{
    wxWindowList parts;
    if ( m_labelWin )
        parts.push_back(m_labelWin);
    return parts;
}

void wxStaticBox::PositionLabelWindow()
{
    if ( !m_labelWin )
        return;

    wxWeakRef<wxStaticBox> alive(this);
    wxWindow * const label = m_labelWin;
    wxWeakRef<wxWindow> labelAlive(label);

    const wxSize bestSize = label->GetBestSize();
    wxStaticBox *owner = alive.get();
    wxWindow *currentLabel = labelAlive.get();
    if ( !owner || !currentLabel || owner->m_labelWin != currentLabel )
        return;

    // Compute the final position before SetSize(): its wxEVT_SIZE handler may
    // synchronously destroy either participant or replace the label window.
    const int labelX = owner->FromDIP(10);
    currentLabel->SetSize(bestSize);

    owner = alive.get();
    currentLabel = labelAlive.get();
    if ( !owner || !currentLabel || owner->m_labelWin != currentLabel )
        return;

    currentLabel->Move(labelX, 0);
}

wxString wxStaticBox::GetAccessKeyLabel() const
{
    if ( m_labelWin )
    {
        if ( const wxControl * const control =
                 wxDynamicCast(m_labelWin, wxControl) )
        {
            return control->GetLabel();
        }

        return m_labelWin->GetName();
    }

    return GetLabel();
}

bool wxStaticBox::UpdateWinUIContent()
{
    if ( !m_winui )
        return false;

    wxWinUIStaticBoxImpl * const impl = m_winui.get();
    wxWeakRef<wxStaticBox> alive(this);

    const auto queueRetry = [&]()
    {
        wxStaticBox * const owner = alive.get();
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != impl ||
             impl->projectionRetryScheduled )
        {
            return;
        }

        impl->projectionRetryScheduled = true;
        owner->CallAfter(
            [alive, impl]()
            {
                wxStaticBox * const current = alive.get();
                if ( !current || !current->m_winui ||
                     current->m_winui.get() != impl )
                {
                    return;
                }

                impl->projectionRetryScheduled = false;
                current->UpdateWinUIContent();
            });
    };

    // A property callback raised while SetContent() is attaching the candidate
    // may mutate the wx model and attempt a nested rebuild. The shared host
    // correctly rejects that nested content transaction; this bounded loop
    // observes the new revision after the outer swap and projects it next.
    // Continuous application churn is handed to one coalesced event-loop pass
    // instead of recursing or spinning synchronously.
    constexpr unsigned MaxSynchronousProjectionPasses = 8;
    for ( unsigned pass = 0; pass < MaxSynchronousProjectionPasses; ++pass )
    {
        try
        {
        wxStaticBox * const owner = alive.get();
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != impl )
        {
            return false;
        }

        const unsigned long long revision = impl->modelRevision;
        namespace MUX = winrt::Microsoft::UI::Xaml;
        namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;
        namespace MUXAP = winrt::Microsoft::UI::Xaml::Automation::Peers;
        namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

        MUXC::Border root;
        root.IsHitTestVisible(false);
        MUXC::Grid automationRoot = wxWinUICreateAccessibleGrid(
            MUXAP::AutomationControlType::Group, "wxStaticBox");
        automationRoot.IsHitTestVisible(false);
        automationRoot.Children().Append(root);

        MUXC::Grid layout;
        root.Child(layout);

        const auto makeThemedBorder =
            [](wxWinUIThemeBrushProperty property)
            {
                auto border = wxWinUICreateThemeBrushBorder(
                    "CardStrokeColorDefaultBrush", property);
                if ( !border )
                {
                    border = wxWinUICreateThemeBrushBorder(
                        "ControlStrokeColorDefaultBrush", property);
                }
                return border;
            };

        const wxString stringLabel =
            owner->m_labelWin
                ? wxString()
                : owner->GetLabelText(owner->GetLabel());
        const bool hasTitle =
            owner->m_labelWin || !stringLabel.empty();
        constexpr double titleLineDIP = 8.0;

        MUXC::Border frame = makeThemedBorder(
            wxWinUIThemeBrushProperty::BorderBrush);
        const bool usesThemeBrush = frame != nullptr;
        if ( !frame )
            frame = MUXC::Border();

        MUX::Thickness borderThickness{};
        borderThickness.Left = 1.0;
        borderThickness.Top = hasTitle ? 0.0 : 1.0;
        borderThickness.Right = 1.0;
        borderThickness.Bottom = 1.0;
        frame.BorderThickness(borderThickness);
        MUX::CornerRadius radius{};
        radius.TopLeft = 8.0;
        radius.TopRight = 8.0;
        radius.BottomRight = 8.0;
        radius.BottomLeft = 8.0;
        frame.CornerRadius(radius);

        MUX::Thickness frameMargin{};
        frameMargin.Top = hasTitle ? titleLineDIP : 0.0;
        frame.Margin(frameMargin);
        const double frameTopDIP = frameMargin.Top;
        layout.Children().Append(frame);

        MUXC::TextBlock title{ nullptr };
        double titleGapDIP = 0.0;
        if ( hasTitle )
        {
            // Draw the top edge as two independent theme-coloured segments.
            // This creates a genuine gap for the title and eliminates the old
            // opaque light/dark mask, so Mica, custom backgrounds and high
            // contrast all remain visible through the group header.
            MUXC::Grid titleRow;
            titleRow.VerticalAlignment(MUX::VerticalAlignment::Top);

            MUXC::ColumnDefinition leftColumn;
            leftColumn.Width(MUX::GridLengthHelper::FromPixels(8.0));
            titleRow.ColumnDefinitions().Append(leftColumn);

            MUXC::ColumnDefinition titleColumn;
            if ( owner->m_labelWin )
            {
                const int labelWidth =
                    owner->m_labelWin->GetBestSize().x;
                const wxStaticBox * const ownerAfterBestSize =
                    alive.get();
                if ( ownerAfterBestSize != owner ||
                     !ownerAfterBestSize->m_winui ||
                     ownerAfterBestSize->m_winui.get() != impl )
                {
                    return false;
                }
                titleGapDIP =
                    static_cast<double>(owner->ToDIP(labelWidth)) + 8.0;
                titleColumn.Width(
                    MUX::GridLengthHelper::FromPixels(
                        titleGapDIP));
            }
            else
            {
                titleColumn.Width(MUX::GridLengthHelper::Auto());
            }
            titleRow.ColumnDefinitions().Append(titleColumn);

            MUXC::ColumnDefinition rightColumn;
            rightColumn.Width(MUX::GridLengthHelper::FromValueAndType(
                1.0, MUX::GridUnitType::Star));
            titleRow.ColumnDefinitions().Append(rightColumn);

            const auto makeLine = [&]()
            {
                MUXC::Border line = makeThemedBorder(
                    wxWinUIThemeBrushProperty::Background);
                if ( !line )
                    line = MUXC::Border();
                line.Height(1.0);
                line.VerticalAlignment(MUX::VerticalAlignment::Top);
                MUX::Thickness margin{};
                margin.Top = titleLineDIP;
                line.Margin(margin);
                return line;
            };

            MUXC::Border leftLine = makeLine();
            MUXC::Grid::SetColumn(leftLine, 0);
            titleRow.Children().Append(leftLine);

            if ( !owner->m_labelWin )
            {
                title = MUXC::TextBlock();
                title.Text(wxWinUIToHString(stringLabel));
                MUX::Thickness margin{};
                margin.Left = 4.0;
                margin.Right = 4.0;
                title.Margin(margin);
                wxWinUIApplyFont(
                    title,
                    owner->m_hasFont ? owner->GetFont() : wxNullFont);
                wxWinUIApplyForeground(
                    title,
                    owner->m_hasFgCol
                        ? owner->GetForegroundColour()
                        : wxNullColour);
                MUXA::AutomationProperties::SetAccessibilityView(
                    title, MUXAP::AccessibilityView::Raw);
                MUXC::Grid::SetColumn(title, 1);
                titleRow.Children().Append(title);
            }

            MUXC::Border rightLine = makeLine();
            MUXC::Grid::SetColumn(rightLine, 2);
            titleRow.Children().Append(rightLine);
            layout.Children().Append(titleRow);
        }

        wxWinUIApplyBackground(
            root,
            owner->m_hasBgCol
                ? owner->GetBackgroundColour()
                : wxNullColour);
        // Name is owned by the shared slot adapter. The component only owns
        // its mnemonic and group-specific UIA metadata.
        wxWinUIApplyAccessKey(
            automationRoot, owner->GetAccessKeyLabel());

        // Build the replacement entirely in locals. Only publish our model
        // after the shared host's transactional content swap succeeded.
        const bool contentSet = impl->host.SetContent(automationRoot);
        wxStaticBox * const liveOwner = alive.get();
        if ( !liveOwner || !liveOwner->m_winui ||
             liveOwner->m_winui.get() != impl )
        {
            return false;
        }
        if ( !contentSet )
        {
            // A nested projection may have superseded this candidate while
            // the initial slot was being attached. Its model revision is the
            // proof that retrying is convergence, not an HRESULT spin.
            if ( impl->modelRevision != revision )
                continue;
            return false;
        }

        // Publish exactly the tree accepted by the host before checking for a
        // newer wx revision. This keeps peer/model identity truthful even when
        // another pass is immediately required.
        impl->root = root;
        impl->automationRoot = automationRoot;
        impl->frame = frame;
        impl->title = title;
        impl->frameTopDIP = frameTopDIP;
        impl->titleGapDIP = titleGapDIP;
        impl->usesThemeBrush = usesThemeBrush;
        impl->host.SetAutomationNameOverride(
            liveOwner->m_labelWin
                ? liveOwner->GetAccessKeyLabel()
                : wxString());

        if ( impl->modelRevision == revision )
            return true;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI StaticBox creation", e);
            return false;
        }
    }

    queueRetry();
    return true;
}

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUIStaticTestAccess::GetAppearance(
    const wxStaticBox& box, wxWinUIAppearanceSnapshot *snapshot)
{
    if ( !snapshot || !box.m_winui || !box.m_winui->root ||
         !box.m_winui->automationRoot )
        return false;

    try
    {
        *snapshot = wxWinUICaptureAppearance(
            box.m_winui->title, box.m_winui->root, box.m_winui->automationRoot);
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxWinUIStaticTestAccess::GetLayout(const wxStaticBox& box,
                                       double *frameTopDIP,
                                       double *titleGapDIP,
                                       bool *usesThemeBrush)
{
    if ( !box.m_winui || !box.m_winui->root || !box.m_winui->frame )
        return false;

    if ( frameTopDIP )
        *frameTopDIP = box.m_winui->frameTopDIP;
    if ( titleGapDIP )
        *titleGapDIP = box.m_winui->titleGapDIP;
    if ( usesThemeBrush )
        *usesThemeBrush = box.m_winui->usesThemeBrush;
    return true;
}

#endif // WXWINUI_TEST_SUPPORT

#endif // wxUSE_STATBOX
