/////////////////////////////////////////////////////////////////////////////
// Name:        src/univ/control.cpp
// Purpose:     universal wxControl: adds handling of mnemonics
// Author:      Vadim Zeitlin
// Modified by:
// Created:     14.08.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

#ifdef __GNUG__
    #pragma implementation "control.h"
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_CONTROLS

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/control.h"
    #include "wx/dcclient.h"
#endif

#include "wx/univ/renderer.h"
#include "wx/univ/inphand.h"
#include "wx/univ/theme.h"

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(wxControl, wxWindow)

BEGIN_EVENT_TABLE(wxControl, wxControlBase)
    EVT_KEY_DOWN(wxControl::OnKeyDown)
    EVT_KEY_UP(wxControl::OnKeyUp)

    EVT_MOUSE_EVENTS(wxControl::OnMouse)

    EVT_SET_FOCUS(wxControl::OnFocus)
    EVT_KILL_FOCUS(wxControl::OnFocus)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// creation
// ----------------------------------------------------------------------------

void wxControl::Init()
{
    m_indexAccel = -1;
}

bool wxControl::Create(wxWindow *parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxValidator& validator,
                       const wxString& name)
{
    if ( !wxControlBase::Create(parent, id, pos, size, style, validator, name) )
        return FALSE;

    SetBackgroundColour(parent->GetBackgroundColour());

    m_handler = CreateInputHandler();

    return TRUE;
}

// ----------------------------------------------------------------------------
// mnemonics handling
// ----------------------------------------------------------------------------

void wxControl::SetLabel(const wxString& label)
{
    // the character following MNEMONIC_PREFIX is the accelerator for this
    // control unless it is MNEMONIC_PREFIX too - this allows to insert
    // literal MNEMONIC_PREFIX chars into the label
    static const wxChar MNEMONIC_PREFIX = _T('&');

    wxString labelOld = m_label;
    m_indexAccel = -1;
    m_label.clear();
    for ( const wxChar *pc = label; *pc != wxT('\0'); pc++ )
    {
        if ( *pc == MNEMONIC_PREFIX )
        {
            pc++; // skip it
            if ( *pc != MNEMONIC_PREFIX )
            {
                if ( m_indexAccel == -1 )
                {
                    // remember it (-1 is for MNEMONIC_PREFIX itself
                    m_indexAccel = pc - label.c_str() - 1;
                }
                else
                {
                    wxFAIL_MSG(_T("duplicate accel char in control label"));
                }
            }
        }

        m_label += *pc;
    }

    if ( m_label != labelOld )
    {
        Refresh();
    }
}

wxString wxControl::GetLabel() const
{
    return m_label;
}

// ----------------------------------------------------------------------------
// focus handling
// ----------------------------------------------------------------------------

void wxControl::OnFocus(wxFocusEvent& event)
{
    if ( m_handler->OnFocus(this, event) )
        Refresh();
}

// ----------------------------------------------------------------------------
// input processing
// ----------------------------------------------------------------------------

wxInputHandler *wxControl::CreateInputHandler() const
{
    return wxTheme::Get()->GetInputHandler(GetClassInfo()->GetClassName());
}

void wxControl::OnKeyDown(wxKeyEvent& event)
{
    PerformActions(m_handler->Map(this, event, TRUE), event);
}

void wxControl::OnKeyUp(wxKeyEvent& event)
{
    PerformActions(m_handler->Map(this, event, FALSE), event);
}

void wxControl::OnMouse(wxMouseEvent& event)
{
    if ( event.Moving() || event.Entering() || event.Leaving() )
    {
        // don't process it at all for static controls which are not supposed
        // to react to the mouse in any way at all
        if ( AcceptsFocus() && m_handler->OnMouseMove(this, event) )
            Refresh();
    }
    else // a click action
    {
        PerformActions(m_handler->Map(this, event), event);
    }
}

// ----------------------------------------------------------------------------
// the actions
// ----------------------------------------------------------------------------

void wxControl::PerformActions(const wxControlActions& actions,
                               const wxEvent& event)
{
    bool needsRefresh = FALSE;
    size_t count = actions.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        const wxControlAction& action = actions[n];
        if ( !action )
            continue;

        if ( PerformAction(action, event) )
            needsRefresh = TRUE;
    }

    if ( needsRefresh )
        Refresh();
}

bool wxControl::PerformAction(const wxControlAction& action,
                              const wxEvent& event)
{
    return FALSE;
}

#endif // wxUSE_CONTROLS
