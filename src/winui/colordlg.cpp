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
#include "wx/stockitem.h"

#include <winrt/Windows.Foundation.h>

wxIMPLEMENT_DYNAMIC_CLASS(wxColourDialog, wxDialog);

bool wxColourDialog::Create(wxWindow *parent, const wxColourData *data)
{
    m_winuiParent = GetParentForModalDialog(parent, 0);

    if ( data )
        m_colourData = *data;

    return true;
}

void wxColourDialog::SetTitle(const wxString& title)
{
    m_title = title;
}

wxString wxColourDialog::GetTitle() const
{
    return m_title;
}

int wxColourDialog::ShowModal()
{
    WX_HOOK_MODAL_DIALOG();

    wxWindow* const parent = m_winuiParent
        ? m_winuiParent
        : GetParentForModalDialog(nullptr, 0);
    HWND hwndParent = parent ? GetHwndOf(parent) : nullptr;

    if ( !hwndParent || !wxWinUI3Initialize() )
        return wxID_CANCEL;

    try
    {
        using namespace winrt::Microsoft::UI::Xaml::Controls;

        wxWinUIDialogPresenter presenter;
        if ( !presenter.Create(parent,
                               m_title.empty() ? wxString(_("Choose colour"))
                                               : m_title) )
        {
            return wxID_CANCEL;
        }

        ColorPicker picker;
        picker.IsAlphaEnabled(m_colourData.GetChooseAlpha());
        picker.IsMoreButtonVisible(true);

        const wxColour initial = m_colourData.GetColour();
        if ( initial.IsOk() )
        {
            winrt::Windows::UI::Color color{};
            color.A = initial.Alpha();
            color.R = initial.Red();
            color.G = initial.Green();
            color.B = initial.Blue();
            picker.Color(color);
        }

        winrt::event_token colorChangedToken = picker.ColorChanged(
            [this](ColorPicker const&, ColorChangedEventArgs const& args)
            {
                const auto c = args.NewColor();
                wxColourDialogEvent event(wxEVT_COLOUR_CHANGED, this,
                                          wxColour(c.R, c.G, c.B, c.A));
                ProcessWindowEvent(event);
            });
        wxUnusedVar(colorChangedToken);

        presenter.SetContent(picker);
        // The picker needs a fair amount of room: colour spectrum, sliders and
        // the value fields revealed by the "more" button.
        presenter.SetContentSize(
            wxSize(340, m_colourData.GetChooseAlpha() ? 460 : 420));

        presenter.AddButton(wxID_OK,
                            wxGetStockLabel(wxID_OK, wxSTOCK_FOR_BUTTON), true);
        presenter.AddButton(wxID_CANCEL,
                            wxGetStockLabel(wxID_CANCEL, wxSTOCK_FOR_BUTTON));

        wxColour chosen;
        if ( presenter.ShowModal() == wxID_OK )
        {
            const auto c = picker.Color();
            chosen = wxColour(c.R, c.G, c.B, c.A);
        }

        if ( chosen.IsOk() )
        {
            m_colourData.SetColour(chosen);
            return wxID_OK;
        }

        return wxID_CANCEL;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("ColourDialog ContentDialog", e);
    }

    return wxID_CANCEL;
}

#endif // wxUSE_COLOURDLG && wxUSE_WINUI3
