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

    EVT_PAINT(wxControl::OnPaint)
    EVT_ERASE_BACKGROUND(wxControl::OnErase)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// creation
// ----------------------------------------------------------------------------

void wxControl::Init()
{
    m_indexAccel = -1;
    m_isCurrent = FALSE;
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
// state flags
// ----------------------------------------------------------------------------

bool wxControl::IsFocused() const
{
    wxControl *self = wxConstCast(this, wxControl);
    return self->FindFocus() == self;
}

bool wxControl::IsPressed() const
{
    return FALSE;
}

bool wxControl::IsDefault() const
{
    return FALSE;
}

bool wxControl::IsCurrent() const
{
    return m_isCurrent;
}

void wxControl::SetCurrent(bool doit)
{
    m_isCurrent = doit;
}

int wxControl::GetStateFlags() const
{
    int flags = 0;
    if ( !IsEnabled() )
        flags |= wxCONTROL_DISABLED;

    // the following states are only possible if our application is active - if
    // it is not, even our default/focused controls shouldn't appear as such
    if ( wxTheApp->IsActive() )
    {
        if ( IsCurrent() )
            flags |= wxCONTROL_CURRENT;
        if ( IsFocused() )
            flags |= wxCONTROL_FOCUSED;
        if ( IsPressed() )
            flags |= wxCONTROL_PRESSED;
        if ( IsDefault() )
            flags |= wxCONTROL_ISDEFAULT;
    }

    return flags;
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
// background pixmap
// ----------------------------------------------------------------------------

void wxControl::SetBackground(const wxBitmap& bitmap,
                              int alignment,
                              wxStretch stretch)
{
    m_bitmapBg = bitmap;
    m_alignBgBitmap = alignment;
    m_stretchBgBitmap = stretch;
}

const wxBitmap& wxControl::GetBackgroundBitmap(int *alignment,
                                               wxStretch *stretch) const
{
    if ( m_bitmapBg.Ok() )
    {
        if ( alignment )
            *alignment = m_alignBgBitmap;
        if ( stretch )
            *stretch = m_stretchBgBitmap;
    }

    return m_bitmapBg;
}

// ----------------------------------------------------------------------------
// painting
// ----------------------------------------------------------------------------

// the event handler executed when the window background must be painted
void wxControl::OnErase(wxEraseEvent& event)
{
    wxControlRenderer renderer(this, *event.GetDC(),
                               wxTheme::Get()->GetRenderer());

    if ( !DoDrawBackground(&renderer) )
    {
        // not processed
        event.Skip();
    }
}

// the event handler executed when the window must be repainted
void wxControl::OnPaint(wxPaintEvent& event)
{
    // get the DC to use and create renderer on it
    wxPaintDC dc(this);
    wxControlRenderer renderer(this, dc, wxTheme::Get()->GetRenderer());

    // do draw the control!
    DoDraw(&renderer);
}

bool wxControl::DoDrawBackground(wxControlRenderer *renderer)
{
    if ( !m_bitmapBg.Ok() )
        return FALSE;

    renderer->DrawBackgroundBitmap();

    return TRUE;
}

void wxControl::DoDraw(wxControlRenderer *renderer)
{
    renderer->DrawBorder();
}

// ----------------------------------------------------------------------------
// focus handling
// ----------------------------------------------------------------------------

void wxControl::OnFocus(wxFocusEvent& event)
{
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
        if ( PerformAction(actions[n], event) )
            needsRefresh = TRUE;
    }

    if ( needsRefresh )
        Refresh();
}

bool wxControl::PerformAction(const wxControlAction& action,
                              const wxEvent& event)
{
    if ( (action == wxACTION_FOCUS) && AcceptsFocus() )
    {
        SetFocus();

        return TRUE;
    }

    return FALSE;
}

#endif // wxUSE_CONTROLS
