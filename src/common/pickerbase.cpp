///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/pickerbase.cpp
// Purpose:     wxPickerBase class implementation
// Author:      Francesco Montorsi
// Modified by:
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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_COLOURPICKERCTRL || \
    wxUSE_DIRPICKERCTRL    || \
    wxUSE_FILEPICKERCTRL   || \
    wxUSE_FONTPICKERCTRL

#include "wx/pickerbase.h"
#include "wx/tooltip.h"

#ifndef WX_PRECOMP
    #include "wx/textctrl.h"
#endif


// ============================================================================
// implementation
// ============================================================================

wxIMPLEMENT_ABSTRACT_CLASS(wxPickerBase, wxControl);

// ----------------------------------------------------------------------------
// wxPickerBase
// ----------------------------------------------------------------------------

bool wxPickerBase::CreateBase(wxWindow *parent,
                         wxWindowID id,
                         const wxString &text,
                         const wxPoint& pos,
                         const wxSize& size,
                         long style,
                         const wxValidator& validator,
                         const wxString& name)
{
    // remove any border style from our style as wxPickerBase's window must be
    // invisible (user styles must be set on the textctrl or the platform-dependent picker)
    style &= ~wxBORDER_MASK;

    if (!wxControl::Create(parent, id, pos, size, style | wxNO_BORDER | wxTAB_TRAVERSAL,
                           validator, name))
        return false;

    SetMinSize( size );

    m_sizer = new wxBoxSizer(wxHORIZONTAL);

    if (HasFlag(wxPB_USE_TEXTCTRL))
    {
        // NOTE: the style of this class (wxPickerBase) and the style of the
        //       attached text control are different: GetTextCtrlStyle() extracts
        //       the styles related to the textctrl from the styles passed here
        m_text = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                                wxDefaultPosition, wxDefaultSize,
                                GetTextCtrlStyle(style));
        if (!m_text)
        {
            wxFAIL_MSG( wxT("wxPickerBase's textctrl creation failed") );
            return false;
        }

        // set the maximum length allowed for this textctrl.
        // This is very important since any change to it will trigger an update in
        // the m_picker; for very long strings, this real-time synchronization could
        // become a CPU-blocker and thus should be avoided.
        // 32 characters will be more than enough for all common uses.
        m_text->SetMaxLength(32);

        // set the initial contents of the textctrl
        m_text->SetValue(text);

        m_text->Bind(wxEVT_TEXT, &wxPickerBase::OnTextCtrlUpdate, this);
        m_text->Bind(wxEVT_KILL_FOCUS, &wxPickerBase::OnTextCtrlKillFocus, this);
        m_text->Bind(wxEVT_DESTROY, &wxPickerBase::OnTextCtrlDelete, this);

        m_sizer->Add(m_text,
                     wxSizerFlags(1).CentreVertical().Border(wxRIGHT));
    }

    return true;
}

void wxPickerBase::PostCreation()
{
    // the picker grows in the major direction only if there is no text control
    m_sizer->Add(m_picker,
                 wxSizerFlags(HasTextCtrl() ? 0 : 1).CentreVertical());

    // For aesthetic reasons, make sure the picker is at least as high as the
    // associated text control and is always at least square, unless we are
    // explicitly using wxPB_SMALL style to force it to take as little space as
    // possible.
    if ( !HasFlag(wxPB_SMALL) )
    {
        const wxSize pickerBestSize(m_picker->GetBestSize());
        const wxSize textBestSize( HasTextCtrl() ? m_text->GetBestSize() : wxSize());
        wxSize pickerMinSize;
        pickerMinSize.y = wxMax(pickerBestSize.y, textBestSize.y);
        pickerMinSize.x = wxMax(pickerBestSize.x, pickerMinSize.y);
        if ( pickerMinSize != pickerBestSize )
            m_picker->SetMinSize(pickerMinSize);
    }

    SetSizer(m_sizer);

    SetInitialSize( GetMinSize() );

    Layout();
}

#if wxUSE_TOOLTIPS

void wxPickerBase::DoSetToolTip(wxToolTip *tip)
{
    // don't set the tooltip on us but rather on our two child windows
    // as otherwise it would appear only when the cursor is placed on the
    // small area around the child windows which belong to wxPickerBase
    m_picker->SetToolTip(tip);

    // do a copy as wxWindow will own the pointer we pass
    if ( m_text )
        m_text->SetToolTip(tip ? new wxToolTip(tip->GetTip()) : NULL);
}

#endif // wxUSE_TOOLTIPS

void wxPickerBase::DoSetGrowableFlagFor(wxSizerItem* item, bool grow)
{
    // We assume that our controls use either wxALIGN_CENTER_VERTICAL or wxGROW
    // style, this code would need to be changed in the unlikely event any
    // other style is used for them.
    int f = item->GetFlag();
    if ( grow )
    {
        f &= ~wxALIGN_CENTER_VERTICAL;
        f |= wxGROW;
    }
    else
    {
        f &= ~wxGROW;
        f |= wxALIGN_CENTER_VERTICAL;
    }

    item->SetFlag(f);
}

// ----------------------------------------------------------------------------
// wxPickerBase - event handlers
// ----------------------------------------------------------------------------

void wxPickerBase::OnTextCtrlKillFocus(wxFocusEvent& event)
{
    event.Skip();

    // don't leave the textctrl empty
    if (m_text && m_text->GetValue().empty())
        UpdateTextCtrlFromPicker();
}

void wxPickerBase::OnTextCtrlDelete(wxWindowDestroyEvent &)
{
    // the textctrl has been deleted; our pointer is invalid!
    m_text = NULL;
}

void wxPickerBase::OnTextCtrlUpdate(wxCommandEvent &)
{
    // for each text-change, update the picker
    UpdatePickerFromTextCtrl();
}

#endif // Any picker in use
