/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/radiobut.cpp
// Purpose:     wxRadioButton
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

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

#if wxUSE_RADIOBTN

#include "wx/radiobut.h"

#ifndef WX_PRECOMP
    #include "wx/settings.h"
    #include "wx/dcclient.h"
#endif

#include "wx/msw/private.h"
#include "wx/private/window.h"
#include "wx/renderer.h"
#include "wx/msw/uxtheme.h"

// ============================================================================
// wxRadioButton implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxRadioButton creation
// ----------------------------------------------------------------------------

void wxRadioButton::Init()
{
    m_isChecked = false;
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
        return false;

    WXDWORD exstyle = 0;
    WXDWORD msStyle = MSWGetStyle(style, &exstyle);

    if ( !MSWCreateControl(wxT("BUTTON"), msStyle, pos, size, label, exstyle) )
        return false;

    // for compatibility with wxGTK, the first radio button in a group is
    // always checked (this makes sense anyhow as you need to ensure that at
    // least one button in the group is checked and this is the simplest way to
    // do it)
    if ( HasFlag(wxRB_GROUP) )
        SetValue(true);

    return true;
}

// ----------------------------------------------------------------------------
// wxRadioButton functions
// ----------------------------------------------------------------------------

void wxRadioButton::SetValue(bool value)
{
    m_isChecked = value;

    if ( !IsOwnerDrawn() )
        ::SendMessage(GetHwnd(), BM_SETCHECK,
                      value ? BST_CHECKED : BST_UNCHECKED, 0);
    else // owner drawn buttons don't react to this message
        Refresh();

    if ( !value || HasFlag(wxRB_SINGLE) )
        return;

    // if we set the value of one radio button we also must clear all the other
    // buttons in the same group: Windows doesn't do it automatically
    //
    // moreover, if another radiobutton in the group currently has the focus,
    // we have to set it to this radiobutton, else the old radiobutton will be
    // reselected automatically, if a parent window loses the focus and regains
    // it.
    wxWindow * const focus = FindFocus();

    const wxWindowList& siblings = GetParent()->GetChildren();
    wxWindowList::compatibility_iterator nodeThis = siblings.Find(this);
    wxCHECK_RET( nodeThis, wxT("radio button not a child of its parent?") );

    // this will be set to true in the code below if the focus belongs to one
    // of the other buttons in the same group
    bool shouldSetFocus = false;

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
                // don't stop on non radio buttons, we could have intermixed
                // buttons and e.g. static labels
                continue;
            }

            if ( btn->HasFlag(wxRB_SINGLE) )
                {
                    // A wxRB_SINGLE button isn't part of this group
                    break;
                }

            if ( btn == focus )
                shouldSetFocus = true;

            btn->SetValue(false);

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

        if ( !btn )
            continue;

        if ( btn->HasFlag(wxRB_GROUP | wxRB_SINGLE) )
        {
            // no more buttons or the first button of the next group
            break;
        }

        if ( btn == focus )
            shouldSetFocus = true;

        btn->SetValue(false);
    }

    if ( shouldSetFocus )
    {
        // Change focus immediately, we can't do anything else in this case as
        // leaving it to the other radio button would put it in an impossible
        // state: a radio button can't have focus and be unchecked.
        SetFocus();
    }
    else
    {
        // We don't need to change the focus right now, so avoid doing it as it
        // could be unexpected (and also would result in focus set/kill events
        // that wouldn't be generated under the other platforms).
        if ( !GetParent()->IsDescendant(FindFocus()) )
        {
            // However tell the parent to focus this radio button when it
            // regains the focus the next time, to avoid focusing another one
            // and this changing the value. Note that this is not ideal: it
            // would be better to only change pending focus if it currently
            // points to a radio button, but this is much simpler to do, as
            // otherwise we'd need to not only check if the pending focus is a
            // radio button, but also if we'd give the focus to a radio button
            // when there is no current pending focus, so don't bother with it
            // until somebody really complains about it being a problem in
            // practice.
            GetParent()->WXSetPendingFocus(this);
        }
        //else: don't overwrite the pending focus if the window has the focus
        // currently, as this would make its state inconsistent and would be
        // useless anyhow, as the problem we're trying to solve here won't
        // happen in this case (we know that our focused sibling is not a radio
        // button in the same group, otherwise we would have set shouldSetFocus
        // to true above).
    }
}

bool wxRadioButton::GetValue() const
{
    if ( !IsOwnerDrawn() )
    {
        wxASSERT_MSG( m_isChecked ==
                        (::SendMessage(GetHwnd(), BM_GETCHECK, 0, 0L) != 0),
                      wxT("wxRadioButton::m_isChecked is out of sync?") );
    }

    return m_isChecked;
}

// ----------------------------------------------------------------------------
// wxRadioButton event processing
// ----------------------------------------------------------------------------

void wxRadioButton::Command (wxCommandEvent& event)
{
    SetValue(event.GetInt() != 0);
    ProcessCommand(event);
}

bool wxRadioButton::MSWCommand(WXUINT param, WXWORD WXUNUSED(id))
{
    if ( param != BN_CLICKED )
        return false;

    if ( !m_isChecked )
    {
        // we need to manually update the button state as we use BS_RADIOBUTTON
        // and not BS_AUTORADIOBUTTON
        SetValue(true);

        wxCommandEvent event(wxEVT_RADIOBUTTON, GetId());
        event.SetEventObject( this );
        event.SetInt(true); // always checked

        ProcessCommand(event);
    }

    return true;
}

// ----------------------------------------------------------------------------
// wxRadioButton geometry
// ----------------------------------------------------------------------------

wxSize wxRadioButton::DoGetBestSize() const
{
    static wxPrivate::DpiDependentValue<wxCoord> s_radioSize;

    if ( s_radioSize.HasChanged(this) )
    {
        wxClientDC dc(const_cast<wxRadioButton*>(this));
        dc.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));

        s_radioSize.SetAtNewDPI(dc.GetCharHeight());
    }

    wxCoord& radioSize = s_radioSize.Get();
    wxString str = GetLabel();

    int wRadio, hRadio;
    if ( !str.empty() )
    {
        GetTextExtent(GetLabelText(str), &wRadio, &hRadio);
        wRadio += radioSize + GetCharWidth();

        if ( hRadio < radioSize )
            hRadio = radioSize;
    }
    else
    {
        wRadio = radioSize;
        hRadio = radioSize;
    }

    return wxSize(wRadio, hRadio);
}

WXDWORD wxRadioButton::MSWGetStyle(long style, WXDWORD *exstyle) const
{
    WXDWORD msStyle = wxControl::MSWGetStyle(style, exstyle);

    if ( HasFlag(wxRB_GROUP) )
        msStyle |= WS_GROUP;

    // we use BS_RADIOBUTTON and not BS_AUTORADIOBUTTON because the use of the
    // latter can easily result in the application entering an infinite loop
    // inside IsDialogMessage()
    //
    // we used to use BS_RADIOBUTTON only for wxRB_SINGLE buttons but there
    // doesn't seem to be any harm to always use it and it prevents some hangs,
    // see #9786
    msStyle |= BS_RADIOBUTTON;

    if ( style & wxCLIP_SIBLINGS )
        msStyle |= WS_CLIPSIBLINGS;
    if ( style & wxALIGN_RIGHT )
        msStyle |= BS_LEFTTEXT | BS_RIGHT;


    return msStyle;
}

// ----------------------------------------------------------------------------
// owner drawn radio button stuff
// ----------------------------------------------------------------------------

int wxRadioButton::MSWGetButtonStyle() const
{
    return BS_RADIOBUTTON;
}

void wxRadioButton::MSWOnButtonResetOwnerDrawn()
{
    // ensure that controls state is consistent with internal state
    ::SendMessage(GetHwnd(), BM_SETCHECK,
                  m_isChecked ? BST_CHECKED : BST_UNCHECKED, 0);
}

int wxRadioButton::MSWGetButtonCheckedFlag() const
{
    return m_isChecked ? wxCONTROL_CHECKED : wxCONTROL_NONE;
}

void wxRadioButton::MSWDrawButtonBitmap(wxDC& dc, const wxRect& rect, int flags)
{
    wxRendererNative::Get().DrawRadioBitmap(this, dc, rect, flags);
}

#endif // wxUSE_RADIOBTN
