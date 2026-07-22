/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/dlgpresenter.cpp
// Purpose:     wxWinUIDialogPresenter: window/overlay presentation of the
//              WinUI-drawn common dialogs
// Author:      wxWidgets development team
// Created:     2026-07-20
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/winui/winui.h"

#if wxUSE_WINUI3

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/dialog.h"
    #include "wx/utils.h"
#endif

#include "wx/evtloop.h"

#include "wx/msw/private.h"

#include "private.h"

#include "wx/winui/private/tlwhost.h"

#include <cmath>
#include <limits>
#include <memory>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

// ----------------------------------------------------------------------------
// the global presentation setting
// ----------------------------------------------------------------------------

namespace
{

// In-window (ContentDialog over the parent's shared island) is the default
// presentation: the one-island-per-TLW architecture dims the whole client
// area correctly, wx-drawn regions included.
wxWinUIDialogPresentation gs_dialogPresentation =
    wxWinUIDialogPresentation::Overlay;
bool gs_dialogPresentationSet = false;

} // anonymous namespace

void wxWinUISetDialogPresentation(wxWinUIDialogPresentation presentation)
{
    gs_dialogPresentation = presentation;
    gs_dialogPresentationSet = true;
}

wxWinUIDialogPresentation wxWinUIGetDialogPresentation()
{
    // Unless the application chose explicitly, allow the classic
    // separate-window presentation to be turned on from the environment:
    // this makes it possible to compare both with any existing program.
    if ( !gs_dialogPresentationSet && wxGetEnv("WX_WINUI_DIALOG_WINDOW", nullptr) )
        return wxWinUIDialogPresentation::Window;

    return gs_dialogPresentation;
}

// ----------------------------------------------------------------------------
// wxWinUIDialogShell: the top-level window used by the Window presentation
// ----------------------------------------------------------------------------

namespace
{

// Padding around the dialog body, in DIPs, matching the ContentDialog metrics.
constexpr int wxWINUI_DIALOG_MARGIN = 24;
constexpr int wxWINUI_BUTTON_HEIGHT = 32;
constexpr int wxWINUI_BUTTON_MIN_WIDTH = 100;
constexpr int wxWINUI_BUTTON_SPACING = 8;

class wxWinUIDialogShell : public wxDialog
{
public:
    bool Create(wxWindow *parent, const wxString& title)
    {
        // wxRESIZE_BORDER is deliberately not used: the body is sized to its
        // content, exactly like a ContentDialog.
        if ( !wxDialog::Create(parent, wxID_ANY, title, wxDefaultPosition,
                               wxDefaultSize,
                               wxDEFAULT_DIALOG_STYLE & ~wxRESIZE_BORDER) )
            return false;

        return m_host.Initialize(this);
    }

    wxWinUIControlHost& GetHost() { return m_host; }

    // Dismiss the dialog with the given wx id, from a XAML callback.
    void DismissWith(int id)
    {
        // Never end the modal loop from inside a XAML event handler: let the
        // dispatcher unwind first.
        CallAfter([this, id]()
        {
            if ( IsModal() )
                EndModal(id);
            else
                Show(false);
        });
    }

private:
    wxWinUIControlHost m_host;
};

// Create a WinUI button for the dialog button bar.
MUXC::Button wxWinUIMakeDialogButton(const wxString& label, bool isDefault)
{
    MUXC::Button button;
    button.Content(winrt::box_value(wxWinUIToHString(
        wxWinUIRemoveMnemonics(label))));
    button.MinWidth(wxWINUI_BUTTON_MIN_WIDTH);
    button.Height(wxWINUI_BUTTON_HEIGHT);
    button.HorizontalAlignment(MUX::HorizontalAlignment::Stretch);

    if ( isDefault )
    {
        // The accent style is what a ContentDialog gives its default button.
        try
        {
            const auto resources = MUX::Application::Current().Resources();
            const auto key = winrt::box_value(winrt::hstring(L"AccentButtonStyle"));
            if ( resources.HasKey(key) )
                button.Style(resources.Lookup(key).as<MUX::Style>());
        }
        catch ( const winrt::hresult_error& )
        {
        }
    }

    return button;
}

} // anonymous namespace

// ----------------------------------------------------------------------------
// wxWinUIDialogPresenter
// ----------------------------------------------------------------------------

wxWinUIDialogPresenter::wxWinUIDialogPresenter() = default;
wxWinUIDialogPresenter::~wxWinUIDialogPresenter() = default;

bool wxWinUIDialogPresenter::Create(wxWindow *parent, const wxString& title)
{
    if ( !wxWinUI3Initialize() )
        return false;

    m_parent = parent;
    m_title = title;
    return true;
}

void wxWinUIDialogPresenter::SetContent(MUX::UIElement const& content)
{
    m_content = content;
}

void wxWinUIDialogPresenter::AddButton(int id, const wxString& label,
                                       bool isDefault)
{
    wxASSERT_MSG( m_buttons.size() < 3,
                  wxT("at most 3 dialog buttons are supported") );

    Button button;
    button.id = id;
    button.label = label;
    button.isDefault = isDefault;
    m_buttons.push_back(button);
}

int wxWinUIDialogPresenter::ShowModal()
{
    if ( m_buttons.empty() )
        AddButton(wxID_OK, _("OK"), true);

    return wxWinUIGetDialogPresentation() == wxWinUIDialogPresentation::Overlay
        ? ShowAsOverlay()
        : ShowAsWindow();
}

// ----------------------------------------------------------------------------
// Window presentation: a real top-level dialog hosting a single island
// ----------------------------------------------------------------------------

int wxWinUIDialogPresenter::ShowAsWindow()
{
    wxWinUIDialogShell *shell = new wxWinUIDialogShell;
    if ( !shell->Create(m_parent, m_title) )
    {
        shell->Destroy();
        return wxID_CANCEL;
    }

    int result = wxID_CANCEL;

    try
    {
        // Body above, right-aligned button bar below, both inset by the
        // standard dialog margin.
        MUXC::Grid root;
        root.RequestedTheme(wxWinUIGetCurrentElementTheme());
        root.Padding(MUX::ThicknessHelper::FromUniformLength(
            wxWINUI_DIALOG_MARGIN));
        root.RowSpacing(wxWINUI_DIALOG_MARGIN);

        MUXC::RowDefinition bodyRow;
        bodyRow.Height(MUX::GridLengthHelper::FromValueAndType(
            1, MUX::GridUnitType::Star));
        MUXC::RowDefinition buttonRow;
        buttonRow.Height(MUX::GridLengthHelper::Auto());
        root.RowDefinitions().Append(bodyRow);
        root.RowDefinitions().Append(buttonRow);

        if ( m_content )
        {
            MUXC::Grid::SetRow(m_content.as<MUX::FrameworkElement>(), 0);
            root.Children().Append(m_content);
        }

        MUXC::StackPanel buttons;
        buttons.Orientation(MUXC::Orientation::Horizontal);
        buttons.Spacing(wxWINUI_BUTTON_SPACING);
        buttons.HorizontalAlignment(MUX::HorizontalAlignment::Right);
        MUXC::Grid::SetRow(buttons, 1);

        for ( const Button& definition : m_buttons )
        {
            const int id = definition.id;
            MUXC::Button button =
                wxWinUIMakeDialogButton(definition.label, definition.isDefault);

            button.Click(
                [this, shell, id](winrt::Windows::Foundation::IInspectable const&,
                                  MUX::RoutedEventArgs const&)
                {
                    if ( m_onAccept && !m_onAccept(id) )
                        return;   // validation failed: stay open

                    shell->DismissWith(id);
                });

            buttons.Children().Append(button);
        }

        root.Children().Append(buttons);
        shell->GetHost().SetContent(root);

        // Size the dialog from the caller's content hint plus our chrome.
        wxSize client = m_contentSize;
        client.y += wxWINUI_BUTTON_HEIGHT + wxWINUI_DIALOG_MARGIN;
        client.x = wxMax(client.x, static_cast<int>(m_buttons.size()) *
                            (wxWINUI_BUTTON_MIN_WIDTH + wxWINUI_BUTTON_SPACING));
        client.x += 2 * wxWINUI_DIALOG_MARGIN;
        client.y += 2 * wxWINUI_DIALOG_MARGIN;

        shell->SetClientSize(shell->FromDIP(client));
        shell->CentreOnParent();

        result = shell->ShowModal();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI dialog window", e);
    }

    shell->Destroy();
    return result;
}

// ----------------------------------------------------------------------------
// Overlay presentation: a ContentDialog over the parent
// ----------------------------------------------------------------------------

int wxWinUIDialogPresenter::ShowAsOverlay()
{
    // The dialog lives on the parent's shared per-TLW island: its smoke
    // layer covers the whole client area (wx-drawn HWNDs included, since the
    // island band composes above them), and the island hit-test treats an
    // open popup as capturing everything, so the modality is airtight.
    wxWinUITopLevelHost * const host =
        m_parent ? wxWinUITopLevelHost::ForWindow(m_parent, true) : nullptr;
    if ( !host || !host->GetXamlRoot() )
        return ShowAsWindow();   // no island: degrade to a real window

    try
    {
        MUXC::ContentDialog dialog;
        dialog.XamlRoot(host->GetXamlRoot());
        dialog.RequestedTheme(wxWinUIGetCurrentElementTheme());
        dialog.Title(winrt::box_value(wxWinUIToHString(m_title)));

        if ( m_content )
            dialog.Content(m_content);

        // ContentDialog exposes exactly three button slots, in this order.
        const size_t count = m_buttons.size();
        if ( count > 0 )
        {
            dialog.PrimaryButtonText(wxWinUIToHString(
                wxWinUIRemoveMnemonics(m_buttons[0].label)));
        }
        if ( count > 1 )
        {
            dialog.SecondaryButtonText(wxWinUIToHString(
                wxWinUIRemoveMnemonics(m_buttons[1].label)));
        }
        if ( count > 2 )
        {
            dialog.CloseButtonText(wxWinUIToHString(
                wxWinUIRemoveMnemonics(m_buttons[2].label)));
        }

        for ( size_t i = 0; i < count; ++i )
        {
            if ( !m_buttons[i].isDefault )
                continue;

            switch ( i )
            {
                case 0:
                    dialog.DefaultButton(MUXC::ContentDialogButton::Primary);
                    break;
                case 1:
                    dialog.DefaultButton(MUXC::ContentDialogButton::Secondary);
                    break;
                default:
                    dialog.DefaultButton(MUXC::ContentDialogButton::Close);
                    break;
            }
        }

        // Let the accept handler veto a dismissal.
        //
        // CRUCIAL lifetime rule: the ContentDialog lives on the SHARED
        // island and survives this function (closing animation, delayed
        // XAML callbacks), while the presenter is a stack local of the
        // caller's ShowModal().  The handlers must therefore never capture
        // "this": they share a heap state instead, and are revoked before
        // returning.  (With the old per-dialog transient island the island
        // died here, taking the callbacks with it -- on the shared island a
        // late callback into a dead presenter was a use-after-free crashing
        // a few seconds after the dialog was dismissed.)
        struct AcceptState
        {
            std::function<bool (int)> onAccept;
        };
        auto acceptState = std::make_shared<AcceptState>();
        acceptState->onAccept = m_onAccept;

        winrt::event_token primaryToken{};
        winrt::event_token secondaryToken{};

        if ( m_onAccept )
        {
            if ( count > 0 )
            {
                const int id = m_buttons[0].id;
                primaryToken = dialog.PrimaryButtonClick(
                    [acceptState, id](MUXC::ContentDialog const&,
                               MUXC::ContentDialogButtonClickEventArgs const& e)
                    {
                        if ( acceptState->onAccept &&
                                 !acceptState->onAccept(id) )
                            e.Cancel(true);
                    });
            }
            if ( count > 1 )
            {
                const int id = m_buttons[1].id;
                secondaryToken = dialog.SecondaryButtonClick(
                    [acceptState, id](MUXC::ContentDialog const&,
                               MUXC::ContentDialogButtonClickEventArgs const& e)
                    {
                        if ( acceptState->onAccept &&
                                 !acceptState->onAccept(id) )
                            e.Cancel(true);
                    });
            }
        }

        // Behave app-modally: the other top-level windows are disabled while
        // the dialog is up (the parent stays enabled as it hosts the island,
        // whose hit-test captures all input for the open dialog).
        MUXC::ContentDialogResult dialogResult =
            MUXC::ContentDialogResult::None;
        {
            wxWindowDisabler disabler(wxGetTopLevelParent(m_parent));

            bool done = false;
            wxEventLoop loop;

            // The exit must not run while the XAML click dispatch is still
            // on the stack (see below); the shared cell lets the deferred
            // lambda no-op safely if we already returned without running
            // the loop.
            auto loopCell = std::make_shared<wxEventLoop *>(nullptr);

            auto operation = dialog.ShowAsync();
            operation.Completed(
                [&dialogResult, &done, loopCell](
                    winrt::Windows::Foundation::IAsyncOperation<
                        MUXC::ContentDialogResult> const& async,
                    winrt::Windows::Foundation::AsyncStatus status)
                {
                    if ( status ==
                             winrt::Windows::Foundation::AsyncStatus::Completed )
                        dialogResult = async.GetResults();

                    done = true;

                    // CRUCIAL: Completed fires SYNCHRONOUSLY from inside the
                    // XAML button-click dispatch (ContentDialog::
                    // OnCommandButtonClicked).  Exiting the nested wx loop
                    // right here would let ShowModal() return -- and the
                    // caller destroy its wx dialog object -- while that
                    // native XAML stack is still unwinding and about to
                    // touch the dialog again: a use-after-free crashing a
                    // few seconds later.  Defer the exit to the next event
                    // loop turn, once the XAML stack has fully returned.
                    wxTheApp->CallAfter([loopCell]()
                    {
                        if ( *loopCell && (*loopCell)->IsRunning() )
                            (*loopCell)->Exit();
                    });
                });

            if ( !done )
            {
                *loopCell = &loop;
                loop.Run();
                *loopCell = nullptr;
            }
        }

        // The dialog is dismissed but not dead (the shared island keeps it
        // through its closing animation): disarm every callback that could
        // still fire, and detach the caller-owned content -- the accept
        // callback typically references the wx dialog object, which the
        // caller destroys right after ShowModal() returns.
        acceptState->onAccept = nullptr;
        try
        {
            if ( primaryToken.value )
                dialog.PrimaryButtonClick(primaryToken);
            if ( secondaryToken.value )
                dialog.SecondaryButtonClick(secondaryToken);
            dialog.Content(nullptr);
        }
        catch ( const winrt::hresult_error& )
        {
        }

        switch ( dialogResult )
        {
            case MUXC::ContentDialogResult::Primary:
                return count > 0 ? m_buttons[0].id : wxID_OK;
            case MUXC::ContentDialogResult::Secondary:
                return count > 1 ? m_buttons[1].id : wxID_CANCEL;
            case MUXC::ContentDialogResult::None:
                break;
        }

        // Dismissed through the close button (or Escape): that is the last
        // button when there are three, and a cancellation otherwise.
        return count > 2 ? m_buttons[2].id : wxID_CANCEL;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI dialog overlay", e);
    }

    return wxID_CANCEL;
}

#endif // wxUSE_WINUI3
