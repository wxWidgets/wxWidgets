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

    EVT_ACTIVATE(wxControl::OnActivate)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// creation
// ----------------------------------------------------------------------------

void wxControl::Init()
{
    m_indexAccel = -1;

    m_handler = (wxInputHandler *)NULL;
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

    return TRUE;
}

// ----------------------------------------------------------------------------
// mnemonics handling
// ----------------------------------------------------------------------------

/* static */
int wxControl::FindAccelIndex(const wxString& label, wxString *labelOnly)
{
    // the character following MNEMONIC_PREFIX is the accelerator for this
    // control unless it is MNEMONIC_PREFIX too - this allows to insert
    // literal MNEMONIC_PREFIX chars into the label
    static const wxChar MNEMONIC_PREFIX = _T('&');

    if ( labelOnly )
    {
        labelOnly->Empty();
        labelOnly->Alloc(label.length());
    }

    int indexAccel = -1;
    for ( const wxChar *pc = label; *pc != wxT('\0'); pc++ )
    {
        if ( *pc == MNEMONIC_PREFIX )
        {
            pc++; // skip it
            if ( *pc != MNEMONIC_PREFIX )
            {
                if ( indexAccel == -1 )
                {
                    // remember it (-1 is for MNEMONIC_PREFIX itself
                    indexAccel = pc - label.c_str() - 1;
                }
                else
                {
                    wxFAIL_MSG(_T("duplicate accel char in control label"));
                }
            }
        }

        if ( labelOnly )
        {
            *labelOnly += *pc;
        }
    }

    return indexAccel;
}

void wxControl::SetLabel(const wxString& label)
{
    wxString labelOld = m_label;
    m_indexAccel = FindAccelIndex(label, &m_label);

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
// focus/activation handling
// ----------------------------------------------------------------------------

void wxControl::OnFocus(wxFocusEvent& event)
{
    if ( m_handler && m_handler->HandleFocus(this, event) )
        Refresh();
    else
        event.Skip();
}

void wxControl::OnActivate(wxActivateEvent& event)
{
    if ( m_handler && m_handler->HandleActivation(this, event.GetActive()) )
        Refresh();
    else
        event.Skip();
}

// ----------------------------------------------------------------------------
// input processing
// ----------------------------------------------------------------------------

void wxControl::CreateInputHandler(const wxString& inphandler)
{
    m_handler = wxTheme::Get()->GetInputHandler(inphandler);
}

void wxControl::OnKeyDown(wxKeyEvent& event)
{
    if ( !m_handler || !m_handler->HandleKey(this, event, TRUE) )
        event.Skip();
}

void wxControl::OnKeyUp(wxKeyEvent& event)
{
    if ( !m_handler || !m_handler->HandleKey(this, event, FALSE) )
        event.Skip();
}

void wxControl::OnMouse(wxMouseEvent& event)
{
    if ( m_handler )
    {
        if ( event.Moving() || event.Entering() || event.Leaving() )
        {
            if ( m_handler->HandleMouseMove(this, event) )
                return;
        }
        else // a click action
        {
            if ( m_handler->HandleMouse(this, event) )
                return;
        }
    }

    event.Skip();
}

// ----------------------------------------------------------------------------
// the actions
// ----------------------------------------------------------------------------

bool wxControl::PerformAction(const wxControlAction& action,
                              long numArg,
                              const wxString& strArg)
{
    return FALSE;
}

#endif // wxUSE_CONTROLS
