/////////////////////////////////////////////////////////////////////////////
// Name:        radiobut.cpp
// Purpose:     wxRadioButton
// Author:      AUTHOR
// Modified by: JS Lair (99/11/15) adding the cyclic group notion for radiobox
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_RADIOBTN

#include "wx/radiobut.h"
#include "wx/mac/uma.h"

IMPLEMENT_DYNAMIC_CLASS(wxRadioButton, wxControl)


bool wxRadioButton::Create( wxWindow *parent,
    wxWindowID id,
    const wxString& label,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxValidator& validator,
    const wxString& name )
{
    m_macIsUserPane = false;

    if ( !wxControl::Create( parent, id, pos, size, style, validator, name ) )
        return false;

    m_label = label;

    Rect bounds = wxMacGetBoundsForControl( this, pos, size );

    m_peer = new wxMacControl( this );
    OSStatus err = CreateRadioButtonControl(
        MAC_WXHWND(parent->MacGetTopLevelWindowRef()), &bounds, CFSTR(""),
        0, false /* no autotoggle */, m_peer->GetControlRefAddr() );
    verify_noerr( err );

    MacPostControlCreate( pos, size );

    m_cycle = this;

    if (HasFlag( wxRB_GROUP ))
    {
        AddInCycle( NULL );
    }
    else
    {
        // search backward for last group start
        wxRadioButton *chief = NULL;
        wxWindowList::compatibility_iterator node = parent->GetChildren().GetLast();
        while (node)
        {
            wxWindow *child = node->GetData();
            if (child->IsKindOf( CLASSINFO( wxRadioButton ) ))
            {
                chief = (wxRadioButton*)child;
                if (child->HasFlag( wxRB_GROUP ))
                    break;
            }

            node = node->GetPrevious();
        }

        AddInCycle( chief );
    }

    return true;
}

wxRadioButton::~wxRadioButton()
{
    RemoveFromCycle();
}

void wxRadioButton::SetValue(bool val)
{
    wxRadioButton *cycle;
    if (m_peer->GetValue() == val)
        return;

    m_peer->SetValue( val );
    if (val)
    {
        cycle = this->NextInCycle();
        if (cycle != NULL)
        {
            while (cycle != this)
            {
                cycle->SetValue( false );
                cycle = cycle->NextInCycle();
            }
        }
    }
}

bool wxRadioButton::GetValue() const
{
    return m_peer->GetValue();
}

void wxRadioButton::Command(wxCommandEvent& event)
{
    SetValue( (event.GetInt() != 0) );
    ProcessCommand( event );
}

wxInt32 wxRadioButton::MacControlHit( WXEVENTHANDLERREF WXUNUSED(handler), WXEVENTREF WXUNUSED(event) )
{
    // if already set -> no action
    if (GetValue())
        return noErr;

    wxRadioButton *cycle;
    cycle = this->NextInCycle();
    if (cycle != NULL)
    {
        while (cycle != this)
        {
            if (cycle->GetValue())
                cycle->SetValue( false );

            cycle = cycle->NextInCycle();
        }
    }

    SetValue( true );

    wxCommandEvent event2( wxEVT_COMMAND_RADIOBUTTON_SELECTED, m_windowId );
    event2.SetEventObject( this );
    event2.SetInt( true );
    ProcessCommand( event2 );

    return noErr;
}

wxRadioButton *wxRadioButton::AddInCycle(wxRadioButton *cycle)
{
    wxRadioButton *current;

    if (cycle == NULL)
    {
        m_cycle = this;
    }
    else
    {
        current = cycle;
        while (current->m_cycle != cycle)
            current = current->m_cycle;

        m_cycle = cycle;
        current->m_cycle = this;
    }

    return m_cycle;
}

void wxRadioButton::RemoveFromCycle()
{        
    if ((m_cycle == NULL) || (m_cycle == this))
        return;

    // Find the previous one and make it point to the next one
    wxRadioButton* prev = this;
    while (prev->m_cycle != this)
        prev = prev->m_cycle;

    prev->m_cycle = m_cycle;
}

#endif
