///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/fontpickerg.cpp
// Purpose:     wxGenericFontButton class implementation
// Author:      Francesco Montorsi
// Created:     15/04/2006
// Copyright:   (c) Francesco Montorsi
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_FONTPICKERCTRL

#ifndef WX_PRECOMP
    #include "wx/settings.h"
#endif // WX_PRECOMP

#include "wx/fontpicker.h"

#include "wx/fontdlg.h"
#include "wx/weakref.h"

#include <memory>


// ============================================================================
// implementation
// ============================================================================

wxIMPLEMENT_DYNAMIC_CLASS(wxGenericFontButton, wxButton);

// ----------------------------------------------------------------------------
// wxGenericFontButton
// ----------------------------------------------------------------------------

bool wxGenericFontButton::Create( wxWindow *parent, wxWindowID id,
                        const wxFont &initial, const wxPoint &pos,
                        const wxSize &size, long style,
                        const wxValidator& validator, const wxString &name)
{
    wxString label = (style & wxFNTP_FONTDESC_AS_LABEL) ?
                        wxString() : // label will be updated by UpdateFont
                        _("Choose font");

    // create this button
    if (!wxButton::Create( parent, id, label, pos,
                           size, style, validator, name ))
    {
        wxFAIL_MSG( wxT("wxGenericFontButton creation failed") );
        return false;
    }

    // and handle user clicks on it
    Bind(wxEVT_BUTTON, &wxGenericFontButton::OnButtonClick, this, GetId());

    InitFontData();

    m_selectedFont = initial.IsOk() ? initial : *wxNORMAL_FONT;
    UpdateFont();

    return true;
}

void wxGenericFontButton::InitFontData()
{
    m_data.SetAllowSymbols(true);
    m_data.SetColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
    m_data.EnableEffects(true);
}

void wxGenericFontButton::OnButtonClick(wxCommandEvent& WXUNUSED(ev))
{
    // update the wxFontData to be shown in the dialog
    m_data.SetInitialFont(m_selectedFont);

    // create the font dialog and display it
    const wxWeakRef<wxGenericFontButton> weakThis(this);
    std::unique_ptr<wxFontDialog> dialog(new wxFontDialog(this, m_data));
    wxWindow * const dialogParent = dialog->GetParent();
    const wxWeakRef<wxWindow> weakDialogParent(dialogParent);
    const wxWeakRef<wxDialog> weakDialog(dialog.get());
    const int rc = dialog->ShowModal();
    if ( !weakDialog )
    {
        // Destroying the resolved TLW also destroys its dialog child. The
        // unique_ptr must not delete the already-retired wx object.
        (void)dialog.release();
        return;
    }

    // Keep the cleanup valid for backends retaining a raw parent without
    // making the dialog an owned child.
    if ( dialogParent && !weakDialogParent &&
         dialog->GetParent() == dialogParent )
    {
        dialog->SetParent(nullptr);
    }
    if ( rc != wxID_OK )
        return;

    wxGenericFontButton *live = weakThis.get();
    if ( !live )
        return;

    // The accepted data is a value snapshot. Destroy the dialog before
    // publishing it: SetSelectedFont() and the public notification may delete
    // the TLW which owned the dialog.
    const wxFontData acceptedData = dialog->GetFontData();
    dialog.reset();

    live = weakThis.get();
    if ( !live )
        return;

    live->m_data = acceptedData;
    live->SetSelectedFont(live->m_data.GetChosenFont());

    // Updating the realized WinUI label can relayout application sizers.
    // Revalidate once more before publishing the accepted value.
    live = weakThis.get();
    if ( !live )
        return;

    wxFontPickerEvent event(live, live->GetId(), live->m_selectedFont);

    // The notification can delete the control and must be the last operation.
    live->GetEventHandler()->ProcessEvent(event);
}

void wxGenericFontButton::UpdateFont()
{
    if ( !m_selectedFont.IsOk() )
        return;

    const wxWeakRef<wxGenericFontButton> weakThis(this);
    SetForegroundColour(m_data.GetColour());
    if ( !weakThis )
        return;

    if (HasFlag(wxFNTP_USEFONT_FOR_LABEL))
    {
        // use currently selected font for the label...
        wxButton::SetFont(m_selectedFont);
        if ( !weakThis )
            return;
    }

    if (HasFlag(wxFNTP_FONTDESC_AS_LABEL))
    {
        SetLabel(wxString::Format(wxT("%s, %d"),
                 m_selectedFont.GetFaceName().c_str(),
                 m_selectedFont.GetPointSize()));
    }
}

#endif      // wxUSE_FONTPICKERCTRL
