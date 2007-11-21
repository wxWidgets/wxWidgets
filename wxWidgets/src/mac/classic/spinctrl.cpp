/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/classic/spinbutt.cpp
// Purpose:     wxSpinCtrl
// Author:      Robert
// Modified by: Mark Newsam (Based on GTK file)
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_SPINCTRL

#include "wx/spinctrl.h"

#ifndef WX_PRECOMP
    #include "wx/textctrl.h"
#endif //WX_PRECOMP

#include "wx/spinbutt.h"

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
        : wxTextCtrl(spin , -1, value)
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

    bool ProcessEvent(wxEvent &event)
    {
        // Hand button down events to wxSpinCtrl. Doesn't work.
        if (event.GetEventType() == wxEVT_LEFT_DOWN && m_spin->ProcessEvent( event ))
            return true;

        return wxTextCtrl::ProcessEvent( event );
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
    wxSpinCtrlButton(wxSpinCtrl *spin, int style)
        : wxSpinButton(spin )
    {
        m_spin = spin;

        SetWindowStyle(style | wxSP_VERTICAL);
    }

protected:
    void OnSpinButton(wxSpinEvent& eventSpin)
    {
#if defined(__WXMAC__) || defined(__WXMOTIF__)
      m_spin->SetTextValue(eventSpin.GetPosition());

      wxCommandEvent event(wxEVT_COMMAND_SPINCTRL_UPDATED, m_spin->GetId());
      event.SetEventObject(m_spin);
      event.SetInt(eventSpin.GetPosition());

      m_spin->GetEventHandler()->ProcessEvent(event);
#else
        m_spin->SetTextValue(eventSpin.GetPosition());
        eventSpin.Skip();
#endif
    }

private:
    wxSpinCtrl *m_spin;

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxSpinCtrlButton, wxSpinButton)
    EVT_SPIN(-1, wxSpinCtrlButton::OnSpinButton)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxSpinCtrl, wxControl)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxSpinCtrl creation
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
    if ( !wxControl::Create(parent, id, wxDefaultPosition, wxDefaultSize, style,
                            wxDefaultValidator, name) )
    {
        return false;
    }

    // the string value overrides the numeric one (for backwards compatibility
    // reasons and also because it is simpler to satisfy the string value which
    // comes much sooner in the list of arguments and leave the initial
    // parameter unspecified)
    if ( !value.empty() )
    {
        long l;
        if ( value.ToLong(&l) )
            initial = l;
    }

    wxSize csize = size ;
    m_text = new wxSpinCtrlText(this, value);
    m_btn = new wxSpinCtrlButton(this, style);

    m_btn->SetRange(min, max);
    m_btn->SetValue(initial);

    if ( size.y == -1 ) {
      csize.y = m_text->GetSize().y ;
    }
    DoSetSize(pos.x , pos.y , csize.x, csize.y);

    return true;
}

wxSpinCtrl::~wxSpinCtrl()
{
    // delete the controls now, don't leave them alive even though they would
    // still be eventually deleted by our parent - but it will be too late, the
    // user code expects them to be gone now
    delete m_text;
    m_text = NULL ;
    delete m_btn;
    m_btn = NULL ;
}

// ----------------------------------------------------------------------------
// geometry
// ----------------------------------------------------------------------------

wxSize wxSpinCtrl::DoGetBestSize() const
{
    wxSize sizeBtn = m_btn->GetBestSize(),
           sizeText = m_text->GetBestSize();

    return wxSize(sizeBtn.x + sizeText.x + MARGIN, sizeText.y);
}

void wxSpinCtrl::DoMoveWindow(int x, int y, int width, int height)
{
    wxControl::DoMoveWindow(x, y, width, height);

    // position the subcontrols inside the client area
    wxSize sizeBtn = m_btn->GetSize();

    wxCoord wText = width - sizeBtn.x;
    m_text->SetSize(0, 0, wText, height);
    m_btn->SetSize(0 + wText + MARGIN, 0, -1, -1);
}

// ----------------------------------------------------------------------------
// operations forwarded to the subcontrols
// ----------------------------------------------------------------------------

bool wxSpinCtrl::Enable(bool enable)
{
    if ( !wxControl::Enable(enable) )
        return false;
    return true;
}

bool wxSpinCtrl::Show(bool show)
{
    if ( !wxControl::Show(show) )
        return false;
    return true;
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
        return false;
    }

    if ( l < GetMin() || l > GetMax() )
    {
        // out of range
        return false;
    }

    *val = l;

    return true;
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
    // m_text->SetFocus();    Why???? TODO.
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

void wxSpinCtrl::SetSelection(long from, long to)
{
    // if from and to are both -1, it means (in wxWidgets) that all text should
    // be selected
    if ( (from == -1) && (to == -1) )
    {
        from = 0;
    }
    m_text->SetSelection(from, to);
}

#endif // wxUSE_SPINCTRL
