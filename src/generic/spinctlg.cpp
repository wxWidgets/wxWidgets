///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/spinctlg.cpp
// Purpose:     implements wxSpinCtrl as a composite control
// Author:      Vadim Zeitlin
// Modified by:
// Created:     29.01.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// License:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "spinctlg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if !wxUSE_SPINBTN
    #error "This file can only be compiled if wxSpinButton is available"
#endif // !wxUSE_SPINBTN

#ifndef WX_PRECOMP
    #include "wx/textctrl.h"
#endif //WX_PRECOMP

#include "wx/spinbutt.h"
#include "wx/spinctrl.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// the margin between the text control and the spin
static const wxCoord MARGIN = 2;

// ----------------------------------------------------------------------------
// wxSpinCtrlText: text control used by spin control
// ----------------------------------------------------------------------------

class wxSpinCtrlText : public wxTextCtrl
{
public:
    wxSpinCtrlText(wxSpinCtrl *spin, const wxString& value)
        : wxTextCtrl(spin->GetParent(), -1, value)
    {
        m_spin = spin;
    }

protected:
    void OnTextChange(wxCommandEvent& event)
    {
        int val;
        if ( m_spin->GetTextValue(&val) )
        {
            m_spin->GetSpinButton()->SetValue(val);
        }

        event.Skip();
    }

private:
    wxSpinCtrl *m_spin;

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxSpinCtrlText, wxTextCtrl)
    EVT_TEXT(-1, wxSpinCtrlText::OnTextChange)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// wxSpinCtrlButton: spin button used by spin control
// ----------------------------------------------------------------------------

class wxSpinCtrlButton : public wxSpinButton
{
public:
    wxSpinCtrlButton(wxSpinCtrl *spin)
        : wxSpinButton(spin->GetParent())
    {
        m_spin = spin;
    }

protected:
    void OnSpinButton(wxSpinEvent& event)
    {
        m_spin->SetTextValue(event.GetPosition());

        event.Skip();
    }

private:
    wxSpinCtrl *m_spin;

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxSpinCtrlButton, wxSpinButton)
    EVT_SPIN(-1, wxSpinCtrlButton::OnSpinButton)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxSpinCtrl ceration
// ----------------------------------------------------------------------------

void wxSpinCtrl::Init()
{
    m_text = NULL;
    m_btn = NULL;
}

bool wxSpinCtrl::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxString& value,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        int min,
                        int max,
                        int initial,
                        const wxString& name)
{
    if ( !wxControl::Create(parent, id, pos, size, style,
                            wxDefaultValidator, name) )
    {
        return FALSE;
    }

    m_text = new wxSpinCtrlText(this, value);
    m_btn = new wxSpinCtrlButton(this);

    m_btn->SetRange(min, max);
    m_btn->SetValue(initial);

    DoSetSize(pos.x, pos.y, size.x, size.y);

    // have to disable this window to avoid interfering it with message
    // processing to the text and the button... but pretend it is enabled to
    // make IsEnabled() return TRUE
    wxControl::Enable(FALSE); // don't use non virtual Disable() here!
    m_isEnabled = TRUE;

    return TRUE;
}

// ----------------------------------------------------------------------------
// geometry
// ----------------------------------------------------------------------------

wxSize wxSpinCtrl::DoGetBestClientSize() const
{
    wxSize sizeBtn = m_btn->GetBestSize(),
           sizeText = m_text->GetBestSize();

    return wxSize(sizeBtn.x + sizeText.x + MARGIN, sizeText.y);
}

void wxSpinCtrl::DoMoveWindow(int x, int y, int width, int height)
{
    wxControl::DoMoveWindow(x, y, width, height);

    // position the subcontrols inside the client area
    wxSize sizeBtn = m_btn->GetSize(),
           sizeText = m_text->GetSize();

    wxCoord wText = width - sizeBtn.x;
    m_text->SetSize(x, y, wText, height);
    m_btn->SetSize(x + wText + MARGIN, y, -1, height);
}

// ----------------------------------------------------------------------------
// operations forwarded to the subcontrols
// ----------------------------------------------------------------------------

bool wxSpinCtrl::Enable(bool enable)
{
    if ( !wxControl::Enable(enable) )
        return FALSE;

    m_btn->Enable(enable);
    m_text->Enable(enable);

    return TRUE;
}

bool wxSpinCtrl::Show(bool show)
{
    if ( !wxControl::Show(show) )
        return FALSE;

    m_btn->Show(show);
    m_text->Show(show);

    return TRUE;
}

// ----------------------------------------------------------------------------
// value and range access
// ----------------------------------------------------------------------------

bool wxSpinCtrl::GetTextValue(int *val) const
{
    long l;
    if ( !m_text->GetValue().ToLong(&l) )
    {
        // not a number at all
        return FALSE;
    }

    if ( l < GetMin() || l > GetMax() )
    {
        // out of range
        return FALSE;
    }

    *val = l;

    return TRUE;
}

int wxSpinCtrl::GetValue() const
{
    return m_btn ? m_btn->GetValue() : 0;
}

int wxSpinCtrl::GetMin() const
{
    return m_btn ? m_btn->GetMin() : 0;
}

int wxSpinCtrl::GetMax() const
{
    return m_btn ? m_btn->GetMax() : 0;
}

// ----------------------------------------------------------------------------
// changing value and range
// ----------------------------------------------------------------------------

void wxSpinCtrl::SetTextValue(int val)
{
    wxCHECK_RET( m_text, _T("invalid call to wxSpinCtrl::SetTextValue") );

    m_text->SetValue(wxString::Format(_T("%d"), val));

    // select all text
    m_text->SetSelection(0, -1);

    // and give focus to the control!
    m_text->SetFocus();
}

void wxSpinCtrl::SetValue(int val)
{
    wxCHECK_RET( m_btn, _T("invalid call to wxSpinCtrl::SetValue") );

    SetTextValue(val);

    m_btn->SetValue(val);
}

void wxSpinCtrl::SetValue(const wxString& text)
{
    wxCHECK_RET( m_text, _T("invalid call to wxSpinCtrl::SetValue") );

    long val;
    if ( text.ToLong(&val) && ((val > INT_MIN) && (val < INT_MAX)) )
    {
        SetValue((int)val);
    }
    else // not a number at all or out of range
    {
        m_text->SetValue(text);
        m_text->SetSelection(0, -1);
    }
}

void wxSpinCtrl::SetRange(int min, int max)
{
    wxCHECK_RET( m_btn, _T("invalid call to wxSpinCtrl::SetRange") );

    m_btn->SetRange(min, max);
}

