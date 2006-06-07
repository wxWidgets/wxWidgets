///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/pickerbase.cpp
// Purpose:     wxPickerBase class implementation
// Author:      Francesco Montorsi
// Modified by:
// Created:     15/04/2006
// RCS-ID:      $Id$
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

#include "wx/pickerbase.h"

#ifndef WX_PRECOMP
    #include "wx/textctrl.h"
#endif

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_ABSTRACT_CLASS(wxPickerBase, wxWindow)

// ----------------------------------------------------------------------------
// wxPickerBase
// ----------------------------------------------------------------------------

wxPickerBase::~wxPickerBase()
{
    // destroy the windows we are managing: these are not automatically
    // destroyed by wxWindow because they are not built as our children
    // but rather as children of the parent of the wxPickerBase class
    // (since wxPickerBase does not represent a real window)
    if (m_text) m_text->Destroy();
    if (m_picker) m_picker->Destroy();
}

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

    if (HasFlag(wxPB_USE_TEXTCTRL))
    {
        // NOTE: the style of this class (wxPickerBase) and the style of the
        //       attached text control are different: GetTextCtrlStyle() extracts
        //       the styles related to the textctrl from the styles passed here
        m_text = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxPoint(0, 0),
                                wxSize(40, size.GetHeight()), GetTextCtrlStyle(style));
        if (!m_text)
        {
            wxFAIL_MSG( wxT("wxPickerBase's textctrl creation failed") );
            return false;
        }

        // set the maximum lenght allowed for this textctrl.
        // This is very important since any change to it will trigger an update in
        // the m_picker; for very long strings, this real-time synchronization could
        // become a CPU-blocker and thus should be avoided.
        // 32 characters will be more than enough for all common uses.
        m_text->SetMaxLength(32);

        // set the initial contents of the textctrl
        m_text->SetValue(text);

        m_text->Connect(wxEVT_COMMAND_TEXT_UPDATED,
                wxCommandEventHandler(wxPickerBase::OnTextCtrlUpdate),
                NULL, this);
        m_text->Connect(wxEVT_KILL_FOCUS,
                wxFocusEventHandler(wxPickerBase::OnTextCtrlKillFocus),
                NULL, this);

        m_text->Connect(wxEVT_DESTROY,
                wxWindowDestroyEventHandler(wxPickerBase::OnTextCtrlDelete),
                NULL, this);
    }

    return true;
}

void wxPickerBase::OnTextCtrlKillFocus(wxFocusEvent &)
{
    wxASSERT(m_text);

    // don't leave the textctrl empty
    if (m_text->GetValue().empty())
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

int wxPickerBase::GetTextCtrlWidth(int given)
{
    // compute the width of m_text like a wxBoxSizer(wxHORIZONTAL) would do
    // NOTE: the proportion of m_picker is fixed to 1
    return ((given - m_margin) / (m_textProportion + 1)) * m_textProportion;
}

void wxPickerBase::DoSetSizeHints(int minW, int minH, int maxW, int maxH, int incW, int incH)
{
    wxControl::DoSetSizeHints(minW, minH, maxW, maxH, incW, incH);

    if (m_text)
    {
        // compute minWidth and maxWidth of the ausiliary textctrl
        int textCtrlMinW = -1, textCtrlMaxW = -1;
        if (minW != -1)
        {
            textCtrlMinW = GetTextCtrlWidth(minW);
            minW -= textCtrlMinW + m_margin;
        }

        if (maxW != -1)
        {
            textCtrlMaxW = GetTextCtrlWidth(maxW);
            maxW -= textCtrlMaxW + m_margin;
        }

        m_text->SetSizeHints(textCtrlMinW, minH, textCtrlMaxW, maxH, incW, incH);
    }

    if (m_picker)
        m_picker->SetSizeHints(minW, minH, maxW, maxH, incW, incH);
}

void wxPickerBase::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
    wxControl::DoSetSize(x, y, width, height, sizeFlags);

    int pickerx = 0;
    if (m_text)
    {
        // compute width of the ausiliary textctrl
        int textCtrlW = GetTextCtrlWidth(width);

        // set the m_text's position relatively to this window
        m_text->SetSize(0, 0, textCtrlW, height, sizeFlags);

        // change position of the real picker
        pickerx += textCtrlW + m_margin;
        width -= textCtrlW + m_margin;
    }

    if (m_picker)
        m_picker->SetSize(pickerx, 0, width, height, sizeFlags);
}

wxSize wxPickerBase::DoGetBestSize() const
{
    wxSize ret = m_picker->GetBestSize();

    if (m_text)
    {
        wxSize sz = m_text->GetBestSize();

        ret.SetWidth( ret.GetWidth() + sz.GetWidth() + m_margin );
        ret.SetHeight( wxMax(ret.GetHeight(), sz.GetHeight()) );
    }

    return ret;
}

void wxPickerBase::SetInternalMargin(int newmargin)
{
    m_margin = newmargin;
}
