///////////////////////////////////////////////////////////////////////////////
// Name:        univ/spinbutt.cpp
// Purpose:     implementation fo the universal version of wxSpinButton
// Author:      Vadim Zeitlin
// Modified by:
// Created:     21.01.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "univspinbutt.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_SPINBTN

#ifndef WX_PRECOMP
#endif

#include "wx/univ/renderer.h"
#include "wx/univ/inphand.h"
#include "wx/univ/theme.h"

// ============================================================================
// implementation of wxSpinButton
// ============================================================================

// ----------------------------------------------------------------------------
// creation
// ----------------------------------------------------------------------------

void wxSpinButton::Init()
{
    for ( size_t n = 0; n < WXSIZEOF(m_arrowsState); n++ )
    {
        m_arrowsState[n] = 0;
    }

    m_value = 0;
}

bool wxSpinButton::Create(wxWindow *parent,
                          wxWindowID id,
                          const wxPoint& pos,
                          const wxSize& size,
                          long style,
                          const wxString& name)
{
    // the spin buttons never have the border
    style &= ~wxBORDER_MASK;

    if ( !wxSpinButtonBase::Create(parent, id, pos, size, style,
                                   wxDefaultValidator, name) )
        return FALSE;

    SetBestSize(size);

    return TRUE;
}

// ----------------------------------------------------------------------------
// value access
// ----------------------------------------------------------------------------

int wxSpinButton::GetValue() const
{
    return m_value;
}

void wxSpinButton::SetValue(int val)
{
    m_value = val;
}

int wxSpinButton::NormalizeValue(int value) const;
{
    if ( value > m_max )
        value = m_max;
    else if ( value < m_min )
        value = m_min;

    return value;
}

bool wxSpinButton::ChangeValue(int inc)
{
    int valueNew = NormalizeValue(m_value + inc);

    wxSpinEvent event(inc > 0 ? wxEVT_SCROLL_LINEUP : wxEVT_SCROLL_LINEDOWN,
                      GetId());
    event.SetPosition(valueNew);
    event.SetEventObject(this);

    if ( GetEventHandler()->ProcessEvent(event) && !event.IsAllowed() )
    {
        // programm has vetoed the event
        return FALSE;
    }

    m_value = valueNew;

    // FIXME: should we send wxEVT_SCROLL_THUMBTRACK too?

    return TRUE;
}

// ----------------------------------------------------------------------------
// size calculations
// ----------------------------------------------------------------------------

wxSize wxSpinButton::DoGetBestClientSize() const
{
    // a spin button has by default the same size as two scrollbar arrows put
    // together
    wxSize size = m_renderer->GetScrollbarArrowSize();
    if ( IsVertical() )
    {
        size.y *= 2;
    }
    else
    {
        size.x *= 2;
    }

    return size;
}

// ----------------------------------------------------------------------------
// drawing
// ----------------------------------------------------------------------------

int wxSpinButton::GetArrowState(Arrow arrow) const
{
    int state = m_arrowsState[arrow];

    // the arrow may also be disabled: either...
    if ( !IsEnabled() )
    {
        // ...because the control is completely disabled
        state |= wxCONTROL_DISABLED;
    }
    else
    {
        // ... or because we can't go any further
        if ( ((arrow == Arrow_First) && (m_value == m_min)) ||
             ((arrow == Arrow_Second) && (m_value == m_max)) )
            state |= wxCONTROL_DISABLED;
    }

    return state;
}

void wxSpinButton::SetArrowSatte(Arrow arrow, int state)
{
    m_arrowsState[arrow] = state;
}

void wxSpinButton::DrawArrow(Arrow arrow, wxDC& dc, const wxRect& rect)
{
    static const wxDirection arrowDirs[2][Arrow_Max] =
    {
        { wxLEFT, wxRIGHT },
        { wxUP,   wxDOWN  }
    };

    m_renderer->DrawArrow(dc,
                          arrowDirs[IsVertical()][arrow],
                          rectArrow,
                          GetArrowState(arrow));
}

void wxSpinButton::DoDraw(wxControlRenderer *renderer)
{
    // calculate the rectangles for both arrows: note that normally the 2
    // arrows are adjacent to each other but if the total control width/height
    // is odd, we can have 1 pixel between them
    wxRect rectArrow1, rectArrow2,
           rectTotal = GetClientRect();

    rectArrow1 =
    rectArrow2 = rectTotal;
    if ( IsVertical() )
    {
        rectArrow1.height /= 2;
        rectArrow2.height /= 2;
        if ( rectTotal.height % 2 )
            rectArrow2.y++;
    }
    else // horizontal
    {
        rectArrow1.width /= 2;
        rectArrow2.width /= 2;
        if ( rectTotal.width % 2 )
            rectArrow2.x++;
    }

    DrawArrow(Arrow_First, renderer->GetDC(), rectArrow1);
    DrawArrow(Arrow_Second, renderer->GetDC(), rectArrow2);
}

// ----------------------------------------------------------------------------
// input processing
// ----------------------------------------------------------------------------

wxString wxSpinButton::GetInputHandlerType() const
{
    return wxINP_HANDLER_SPINBTN;
}

bool wxSpinButton::PerformAction(const wxControlAction& action,
                                 long numArg,
                                 const wxString& strArg)
{
    if ( action == wxACTION_SPIN_INC )
        ChangeValue(+1);
    else if ( action == wxACTION_SPIN_DEC )
        ChangeValue(-1);
    else
        return wxControl::PerformAction(action, numArg, strArg);

    return TRUE;
}

// ----------------------------------------------------------------------------
// wxStdSpinButtonInputHandler
// ----------------------------------------------------------------------------

wxStdSpinButtonInputHandler::
wxStdSpinButtonInputHandler(wxInputHandler *inphand)
    : wxStdInputHandler(handler)
{
}

bool wxStdSpinButtonInputHandler::HandleKey(wxControl *control,
                                            const wxKeyEvent& event,
                                            bool pressed)
{
    if ( pressed )
    {
        wxControlAction action;
        switch ( event.GetKeyCode() )
        {
            case WXK_DOWN:
            case WXK_RIGHT:
                action = wxACTION_SPIN_DEC;
                break;

            case WXK_UP:
            case WXK_LEFT:
                action = wxACTION_SPIN_INC;
                break;
        }

        if ( !!action )
        {
            control->PerformAction(action);

            return TRUE;
        }
    }

    return wxStdInputHandler::HandleKey(control, event, pressed);
}

bool wxStdSpinButtonInputHandler::HandleMouse(wxControl *control,
                                              const wxMouseEvent& event)
{
}

bool wxStdSpinButtonInputHandler::HandleMouseMove(wxControl *control,
                                                  const wxMouseEvent& event)
{
}


#endif // wxUSE_SPINBTN
