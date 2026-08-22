/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/colordlg.cpp
// Purpose:     wxColourDialog using WinUI ContentDialog + ColorPicker
// Author:      wxWidgets development team
// Created:     2026-06-03
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/winui/winui.h"

#if wxUSE_COLOURDLG && wxUSE_WINUI3

#include "private.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/intl.h"
    #include "wx/utils.h"
#endif

// Not part of wx/wx.h, so include it explicitly even with precompiled headers.
#include "wx/colordlg.h"

#include "wx/evtloop.h"
#include "wx/modalhook.h"
#include "wx/msw/private.h"
#include "wx/scopeguard.h"
#include "wx/stockitem.h"
#include "wx/weakref.h"
#include "wx/winui/private/dialogcontracts.h"

#include <array>
#include <memory>
#include <winrt/Windows.Foundation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.h>

wxIMPLEMENT_DYNAMIC_CLASS(wxColourDialog, wxDialog);

namespace
{

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

winrt::Windows::UI::Color wxWinUIToColor(const wxColour& colour)
{
    winrt::Windows::UI::Color value{};
    value.A = colour.IsOk() ? colour.Alpha() : 255;
    value.R = colour.IsOk() ? colour.Red() : 0;
    value.G = colour.IsOk() ? colour.Green() : 0;
    value.B = colour.IsOk() ? colour.Blue() : 0;
    return value;
}

wxColour wxWinUIFromColor(const winrt::Windows::UI::Color& colour)
{
    return wxColour(colour.R, colour.G, colour.B, colour.A);
}

struct wxWinUICustomColourSlot
{
    MUXC::Button button{ nullptr };
    MUXC::Border swatch{ nullptr };
    winrt::event_token clickToken{};
};

// All peer callbacks see this heap state through a weak_ptr. In particular,
// they never retain or dereference the wxColourDialog after its hidden wx
// window has been destroyed.
class wxWinUIColourDialogState
{
public:
    wxWinUIColourDialogState(wxColourDialog *dialog,
                             const wxColourData& initialData)
        : dialog(dialog),
          workingData(initialData)
    {
        for ( int i = 0; i < wxColourData::NUM_CUSTOM; ++i )
        {
            if ( !workingData.GetCustomColour(i).IsOk() )
            {
                selectedCustom = i;
                break;
            }
        }
    }

    ~wxWinUIColourDialogState()
    {
        Disconnect();
    }

    void Disconnect()
    {
        if ( !active )
            return;

        // Invalidate first: even if WinUI has already queued a callback, it
        // can only observe an inert state while the handlers are revoked.
        active = false;

        try
        {
            if ( picker && colorChangedToken.value )
                picker.ColorChanged(colorChangedToken);
        }
        catch ( const winrt::hresult_error& )
        {
        }
        colorChangedToken = {};

        for ( wxWinUICustomColourSlot& slot : customSlots )
        {
            try
            {
                if ( slot.button && slot.clickToken.value )
                    slot.button.Click(slot.clickToken);
            }
            catch ( const winrt::hresult_error& )
            {
            }
            slot.clickToken = {};
        }

        try
        {
            if ( saveCustomButton && saveCustomToken.value )
                saveCustomButton.Click(saveCustomToken);
        }
        catch ( const winrt::hresult_error& )
        {
        }
        saveCustomToken = {};

        dialog.Release();
    }

    void NotifyColourChanged(const wxColour& colour)
    {
        wxColourDialog * const liveDialog = dialog.get();
        if ( !active || !liveDialog || liveDialog->IsBeingDeleted() )
            return;

        wxColourDialogEvent event(wxEVT_COLOUR_CHANGED,
                                  liveDialog,
                                  colour);
        liveDialog->HandleWindowEvent(event);
    }

    void UpdateCustomSlot(int index)
    {
        if ( index < 0 || index >= wxColourData::NUM_CUSTOM )
            return;

        wxWinUICustomColourSlot& slot = customSlots[index];
        if ( !slot.swatch || !slot.button )
            return;

        const wxColour colour = workingData.GetCustomColour(index);
        if ( colour.IsOk() )
        {
            slot.swatch.Background(
                wxWinUIBrush(colour.Red(), colour.Green(),
                             colour.Blue(), colour.Alpha()));
            slot.swatch.Opacity(1.0);
        }
        else
        {
            // Invalid custom colours remain invalid in workingData. White is
            // only their visual placeholder, matching the fixed-size native
            // MSW palette without silently changing the caller's data.
            slot.swatch.Background(wxWinUIBrush(255, 255, 255));
            slot.swatch.Opacity(0.35);
        }

        slot.swatch.BorderThickness(
            MUX::ThicknessHelper::FromUniformLength(
                index == selectedCustom ? 2.0 : 1.0));
        slot.swatch.BorderBrush(
            index == selectedCustom
                ? wxWinUIBrush(0, 120, 212)
                : wxWinUIBrush(128, 128, 128, 160));

        wxString accessibleName;
        accessibleName.Printf(_("Custom colour %d"), index + 1);
        if ( colour.IsOk() )
        {
            accessibleName << wxASCII_STR(": ")
                           << colour.GetAsString(wxC2S_HTML_SYNTAX);
        }
        else
        {
            accessibleName << wxASCII_STR(": ") << _("empty");
        }
        MUXA::AutomationProperties::SetName(
            slot.button, wxWinUIToHString(accessibleName));
    }

    void SelectCustom(int index)
    {
        wxColourDialog * const liveDialog = dialog.get();
        if ( !active || !liveDialog || liveDialog->IsBeingDeleted() ||
             index < 0 || index >= wxColourData::NUM_CUSTOM )
        {
            return;
        }

        const int previous = selectedCustom;
        selectedCustom = index;
        UpdateCustomSlot(previous);
        UpdateCustomSlot(selectedCustom);

        const wxColour colour = workingData.GetCustomColour(index);
        if ( !colour.IsOk() || !picker )
            return;

        try
        {
            // Updating Color is the same path used by the built-in spectrum
            // and inputs, so the normal ColorChanged callback emits the wx
            // event if (and only if) the selected value actually changed.
            picker.Color(wxWinUIToColor(colour));
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("ColourDialog custom colour", e);
        }
    }

    void SaveCurrentToCustom()
    {
        wxColourDialog * const liveDialog = dialog.get();
        if ( !active || !liveDialog || liveDialog->IsBeingDeleted() ||
                !picker )
            return;

        const wxColour colour = wxWinUIFromColor(picker.Color());
        workingData.SetCustomColour(selectedCustom, colour);
        UpdateCustomSlot(selectedCustom);
    }

    wxWeakRef<wxColourDialog> dialog;
    wxColourData workingData;
    MUXC::ColorPicker picker{ nullptr };
    std::array<wxWinUICustomColourSlot,
               wxColourData::NUM_CUSTOM> customSlots;
    MUXC::Button saveCustomButton{ nullptr };
    winrt::event_token colorChangedToken{};
    winrt::event_token saveCustomToken{};
    int selectedCustom = 0;
    bool active = true;
};

} // anonymous namespace

bool wxColourDialog::Create(wxWindow *parent, const wxColourData *data)
{
    m_winuiParent = GetParentForModalDialog(parent, 0);

    if ( data )
        m_colourData = *data;

    if ( m_title.empty() )
        m_title = _("Choose colour");

    // Although the visible UI is provided by wxWinUIDialogPresenter, keeping a
    // real hidden wxDialog gives this object the normal parent relationship,
    // a valid handle, and the standard wx event routing/lifetime contract.
    return wxDialog::Create(m_winuiParent, wxID_ANY, m_title,
                            wxDefaultPosition, wxDefaultSize,
                            wxDEFAULT_DIALOG_STYLE);
}

void wxColourDialog::SetTitle(const wxString& title)
{
    m_title = title;
    if ( GetHandle() )
        wxDialog::SetTitle(title);
}

wxString wxColourDialog::GetTitle() const
{
    return m_title;
}

int wxColourDialog::ShowModal()
{
    const wxWeakRef<wxDialog> externalLifetimeSelf(this);
    WinUIBeginExternalModalLifetime();
    wxScopeGuard externalLifetime = wxMakeGuard(
        [externalLifetimeSelf]()
        {
            if ( wxDialog * const live = externalLifetimeSelf.get() )
                live->WinUIEndExternalModalLifetime();
        });
    wxUnusedVar(externalLifetime);

    WX_HOOK_MODAL_DIALOG();
    WinUIArmExternalModalLifetime();
    if ( IsBeingDeleted() )
        return wxID_CANCEL;

    wxWindow* const parent = GetParentForModalDialog();

    // Parentless colour dialogs are valid. Window presentation owns its own
    // TLW; Overlay naturally falls back to it when there is no parent island.
    if ( !wxWinUI3Initialize() )
        return wxID_CANCEL;

    try
    {
        wxWinUIDialogPresenter presenter;
        if ( !presenter.Create(parent,
                               m_title.empty() ? wxString(_("Choose colour"))
                                               : m_title) )
        {
            return wxID_CANCEL;
        }
        presenter.SetLifetimeOwner(this);

        auto state = std::make_shared<wxWinUIColourDialogState>(
            this, m_colourData);
        wxScopeGuard cleanup = wxMakeGuard(
            [state]()
            {
                state->Disconnect();
            });
        wxUnusedVar(cleanup);

        state->picker = MUXC::ColorPicker();
        state->picker.IsAlphaEnabled(
            state->workingData.GetChooseAlpha());
        state->picker.IsAlphaSliderVisible(
            state->workingData.GetChooseAlpha());
        state->picker.IsAlphaTextInputVisible(
            state->workingData.GetChooseAlpha());

        // With the More button hidden, WinUI exposes its numeric/hex entry
        // grid immediately. This is the closest direct projection of the
        // native CC_FULLOPEN/ChooseFull contract. In compact mode the user can
        // still reveal these fields with WinUI's More button.
        const bool chooseFull = state->workingData.GetChooseFull();
        state->picker.IsMoreButtonVisible(!chooseFull);

        const wxColour initial = state->workingData.GetColour();
        if ( initial.IsOk() )
            state->picker.Color(wxWinUIToColor(initial));

        const std::weak_ptr<wxWinUIColourDialogState> weakState(state);
        state->colorChangedToken = state->picker.ColorChanged(
            [weakState](MUXC::ColorPicker const&,
                        MUXC::ColorChangedEventArgs const& args)
            {
                const auto liveState = weakState.lock();
                if ( !liveState || !liveState->active )
                    return;

                liveState->NotifyColourChanged(
                    wxWinUIFromColor(args.NewColor()));
            });

        MUXC::StackPanel content;
        content.Spacing(8);
        content.Children().Append(state->picker);

        MUXC::Grid customGrid;
        for ( int column = 0; column < 8; ++column )
        {
            MUXC::ColumnDefinition definition;
            definition.Width(MUX::GridLengthHelper::Auto());
            customGrid.ColumnDefinitions().Append(definition);
        }
        for ( int row = 0; row < 2; ++row )
        {
            MUXC::RowDefinition definition;
            definition.Height(MUX::GridLengthHelper::Auto());
            customGrid.RowDefinitions().Append(definition);
        }

        for ( int i = 0; i < wxColourData::NUM_CUSTOM; ++i )
        {
            wxWinUICustomColourSlot& slot = state->customSlots[i];

            slot.swatch = MUXC::Border();
            slot.swatch.Width(26);
            slot.swatch.Height(22);
            slot.swatch.CornerRadius(
                MUX::CornerRadiusHelper::FromUniformRadius(3));

            slot.button = MUXC::Button();
            slot.button.Width(38);
            slot.button.Height(34);
            slot.button.Padding(
                MUX::ThicknessHelper::FromUniformLength(3));
            slot.button.Margin(
                MUX::ThicknessHelper::FromUniformLength(2));
            slot.button.Content(slot.swatch);
            MUXC::Grid::SetColumn(slot.button, i % 8);
            MUXC::Grid::SetRow(slot.button, i / 8);

            slot.clickToken = slot.button.Click(
                [weakState, i](
                    winrt::Windows::Foundation::IInspectable const&,
                    MUX::RoutedEventArgs const&)
                {
                    if ( const auto liveState = weakState.lock() )
                    {
                        try
                        {
                            liveState->SelectCustom(i);
                        }
                        catch ( const winrt::hresult_error& e )
                        {
                            wxWinUILogException(
                                "ColourDialog custom slot", e);
                        }
                    }
                });

            customGrid.Children().Append(slot.button);
        }

        state->saveCustomButton = MUXC::Button();
        state->saveCustomButton.HorizontalAlignment(
            MUX::HorizontalAlignment::Right);
        state->saveCustomButton.Content(winrt::box_value(
            wxWinUIToHString(wxWinUIRemoveMnemonics(
                _("&Add to custom colours")))));
        state->saveCustomToken = state->saveCustomButton.Click(
            [weakState](
                winrt::Windows::Foundation::IInspectable const&,
                MUX::RoutedEventArgs const&)
            {
                if ( const auto liveState = weakState.lock() )
                {
                    try
                    {
                        liveState->SaveCurrentToCustom();
                    }
                    catch ( const winrt::hresult_error& e )
                    {
                        wxWinUILogException(
                            "ColourDialog save custom colour", e);
                    }
                }
            });

        MUXC::StackPanel customPanel;
        customPanel.Spacing(6);
        customPanel.Children().Append(customGrid);
        customPanel.Children().Append(state->saveCustomButton);

        MUXC::TextBlock customHeader;
        customHeader.Text(wxWinUIToHString(_("Custom colours")));

        MUXC::Expander customExpander;
        customExpander.Header(customHeader);
        customExpander.Content(customPanel);
        customExpander.IsExpanded(chooseFull);
        content.Children().Append(customExpander);

        for ( int i = 0; i < wxColourData::NUM_CUSTOM; ++i )
            state->UpdateCustomSlot(i);

        MUXC::ScrollViewer scroller;
        scroller.Content(content);
        scroller.HorizontalScrollBarVisibility(
            MUXC::ScrollBarVisibility::Disabled);
        scroller.VerticalScrollBarVisibility(
            MUXC::ScrollBarVisibility::Auto);

        wxSize workAreaDIP(1024, 720);
        const HWND contextHwnd =
            parent ? GetHwndOf(parent) : GetHwndOf(this);
        const HMONITOR monitor =
            ::MonitorFromWindow(contextHwnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFO monitorInfo = { sizeof(MONITORINFO) };
        if ( monitor && ::GetMonitorInfo(monitor, &monitorInfo) )
        {
            const wxSize workAreaPx(
                monitorInfo.rcWork.right - monitorInfo.rcWork.left,
                monitorInfo.rcWork.bottom - monitorInfo.rcWork.top);
            const UINT dpi = contextHwnd
                ? ::GetDpiForWindow(contextHwnd)
                : 96;
            workAreaDIP =
                wxWinUIPhysicalWorkAreaToDIP(workAreaPx, dpi);
        }

        const wxSize bodySize =
            wxWinUIComputeColourDialogBodySize(
                chooseFull,
                state->workingData.GetChooseAlpha(),
                workAreaDIP);
        scroller.MaxWidth(bodySize.x);
        scroller.MaxHeight(bodySize.y);
        presenter.SetContent(scroller);
        presenter.SetContentSize(bodySize);

        presenter.AddButton(wxID_OK,
                            wxGetStockLabel(wxID_OK, wxSTOCK_FOR_BUTTON), true);
        presenter.AddButton(wxID_CANCEL,
                            wxGetStockLabel(wxID_CANCEL, wxSTOCK_FOR_BUTTON));

        const int result = presenter.ShowModal();

        if ( result == wxID_OK )
        {
            state->workingData.SetColour(
                wxWinUIFromColor(state->picker.Color()));

            // The dialog may have been destroyed by a colour-changed handler.
            // Only a still-live instance receives the atomic transaction.
            if ( wxColourDialog * const liveDialog = state->dialog.get();
                 liveDialog && !liveDialog->IsBeingDeleted() )
                liveDialog->GetColourData() = state->workingData;
            else
                return wxID_CANCEL;

            state->Disconnect();
            return wxID_OK;
        }

        state->Disconnect();
        return wxID_CANCEL;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("ColourDialog ContentDialog", e);
    }

    return wxID_CANCEL;
}

#endif // wxUSE_COLOURDLG && wxUSE_WINUI3
