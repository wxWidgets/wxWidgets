/////////////////////////////////////////////////////////////////////////////
// Name:        msw/radiobut.cpp
// Purpose:     wxRadioButton
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
#pragma implementation "radiobut.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_RADIOBTN

#ifndef WX_PRECOMP
#include "wx/radiobut.h"
#include "wx/settings.h"
#include "wx/brush.h"
#include "wx/dcscreen.h"
#endif

#include "wx/msw/private.h"

// ============================================================================
// wxRadioButton implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxRadioButton creation
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxRadioButton, wxControl)

void wxRadioButton::Init()
{
    m_focusJustSet = FALSE;
}

bool wxRadioButton::Create(wxWindow *parent,
                           wxWindowID id,
                           const wxString& label,
                           const wxPoint& pos,
                           const wxSize& size,
                           long style,
                           const wxValidator& validator,
                           const wxString& name)
{
    if ( !CreateControl(parent, id, pos, size, style, validator, name) )
        return FALSE;

    long msStyle = WS_TABSTOP;
    if ( HasFlag(wxRB_GROUP) )
        msStyle |= WS_GROUP;

    /*
       wxRB_SINGLE is a temporary workaround for the following problem: if you
       have 2 radiobuttons in the same group but which are not consecutive in
       the dialog, Windows can enter an infinite loop! The simplest way to
       reproduce it is to create radio button, then a panel and then another
       radio button: then checking the last button hangs the app.

       Ideally, we'd detect (and avoid) such situation automatically but for
       now, as I don't know how to do it, just allow the user to create
       BS_RADIOBUTTON buttons for such situations.
     */
    msStyle |= HasFlag(wxRB_SINGLE) ? BS_RADIOBUTTON : BS_AUTORADIOBUTTON;

    if ( HasFlag(wxCLIP_SIBLINGS) )
        msStyle |= WS_CLIPSIBLINGS;

    if ( !MSWCreateControl(_T("BUTTON"), msStyle, pos, size, label, 0) )
        return FALSE;

    // for compatibility with wxGTK, the first radio button in a group is
    // always checked (this makes sense anyhow as you need to ensure that at
    // least one button in the group is checked and this is the simlpest way to
    // do it)
    if ( HasFlag(wxRB_GROUP) )
        SetValue(TRUE);

    return TRUE;
}

// ----------------------------------------------------------------------------
// wxRadioButton functions
// ----------------------------------------------------------------------------

void wxRadioButton::SetValue(bool value)
{
    // BST_CHECKED is defined as 1, BST_UNCHECKED as 0, so we can just pass
    // value as is (we don't use BST_XXX here as they're not defined for Win16)
    (void)::SendMessage(GetHwnd(), BM_SETCHECK, (WPARAM)value, 0L);

    // if we set the value of one radio button we also must clear all the other
    // buttons in the same group: Windows doesn't do it automatically
    if ( value )
    {
        const wxWindowList& siblings = GetParent()->GetChildren();
        wxWindowList::compatibility_iterator nodeThis = siblings.Find(this);
        wxCHECK_RET( nodeThis, _T("radio button not a child of its parent?") );

        // if it's not the first item of the group ...
        if ( !HasFlag(wxRB_GROUP) )
        {
            // ... turn off all radio buttons before it
            for ( wxWindowList::compatibility_iterator nodeBefore = nodeThis->GetPrevious();
                  nodeBefore;
                  nodeBefore = nodeBefore->GetPrevious() )
            {
                wxRadioButton *btn = wxDynamicCast(nodeBefore->GetData(),
                                                   wxRadioButton);
                if ( !btn )
                {
                    // the radio buttons in a group must be consecutive, so
                    // there are no more of them
                    break;
                }

                btn->SetValue(FALSE);

                if ( btn->HasFlag(wxRB_GROUP) )
                {
                    // even if there are other radio buttons before this one,
                    // they're not in the same group with us
                    break;
                }
            }
        }

        // ... and also turn off all buttons after this one
        for ( wxWindowList::compatibility_iterator nodeAfter = nodeThis->GetNext();
              nodeAfter;
              nodeAfter = nodeAfter->GetNext() )
        {
            wxRadioButton *btn = wxDynamicCast(nodeAfter->GetData(),
                                               wxRadioButton);

            if ( !btn || btn->HasFlag(wxRB_GROUP) )
            {
                // no more buttons or the first button of the next group
                break;
            }

            btn->SetValue(FALSE);
        }
    }
}

bool wxRadioButton::GetValue() const
{
    // NB: this will also return TRUE for BST_INDETERMINATE value if we ever
    //     have 3-state radio buttons
    return ::SendMessage(GetHwnd(), BM_GETCHECK, 0, 0L) != 0;
}

// ----------------------------------------------------------------------------
// wxRadioButton event processing
// ----------------------------------------------------------------------------

void wxRadioButton::Command (wxCommandEvent& event)
{
    SetValue(event.m_commandInt != 0);
    ProcessCommand(event);
}

void wxRadioButton::SetFocus()
{
    // when the radio button receives a WM_SETFOCUS message it generates a
    // BN_CLICKED which is totally unexpected and leads to catastrophic results
    // if you pop up a dialog from the radio button event handler as, when the
    // dialog is dismissed, the focus is returned to the radio button which
    // generates BN_CLICKED which leads to showing another dialog and so on
    // without end!
    //
    // to avoid this, we drop the pseudo BN_CLICKED events generated when the
    // button gains focus
    m_focusJustSet = TRUE;

    wxControl::SetFocus();
}

bool wxRadioButton::MSWCommand(WXUINT param, WXWORD WXUNUSED(id))
{
    if ( param != BN_CLICKED )
        return FALSE;

    if ( m_focusJustSet )
    {
        // see above: we want to ignore this event
        m_focusJustSet = FALSE;
    }
    else // a real clicked event
    {
        bool isChecked = GetValue();

        if ( HasFlag(wxRB_SINGLE) )
        {
            // when we use a "manual" radio button, we have to check the button
            // ourselves -- but it's reset to unchecked state by the user code
            // (presumably when another button is pressed)
            if ( !isChecked )
                SetValue(TRUE);
        }

        wxCommandEvent event(wxEVT_COMMAND_RADIOBUTTON_SELECTED, GetId());
        event.SetEventObject( this );
        event.SetInt(isChecked);

        ProcessCommand(event);
    }

    return TRUE;
}

// ----------------------------------------------------------------------------
// wxRadioButton geometry
// ----------------------------------------------------------------------------

wxSize wxRadioButton::DoGetBestSize() const
{
    static int s_radioSize = 0;

    if ( !s_radioSize )
    {
        wxScreenDC dc;
        dc.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));

        s_radioSize = dc.GetCharHeight();
    }

    wxString str = GetLabel();

    int wRadio, hRadio;
    if ( !str.empty() )
    {
        GetTextExtent(str, &wRadio, &hRadio);
        wRadio += s_radioSize + GetCharWidth();

        if ( hRadio < s_radioSize )
            hRadio = s_radioSize;
    }
    else
    {
        wRadio = s_radioSize;
        hRadio = s_radioSize;
    }

    return wxSize(wRadio, hRadio);
}

long wxRadioButton::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
    if (nMsg == WM_SETFOCUS)
    {
        m_focusJustSet = TRUE;

        long ret = wxControl::MSWWindowProc(nMsg, wParam, lParam);

        m_focusJustSet = FALSE;

        return ret;
    }
    return wxControl::MSWWindowProc(nMsg, wParam, lParam);
}

#endif // wxUSE_RADIOBTN
