///////////////////////////////////////////////////////////////////////////////
// Name:        msw/caret.cpp
// Purpose:     MSW implementation of wxCaret
// Author:      Vadim Zeitlin
// Modified by:
// Created:     23.05.99
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "caret.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/window.h"
    #include "wx/log.h"
#endif // WX_PRECOMP

#include "wx/caret.h"

#include "wx/msw/private.h"

// ===========================================================================
// implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// blink time
// ---------------------------------------------------------------------------

//static
int wxCaretBase::GetBlinkTime()
{
    int blinkTime = ::GetCaretBlinkTime();
    if ( !blinkTime )
    {
        wxLogLastError("GetCaretBlinkTime");
    }

    return blinkTime;
}

//static
void wxCaretBase::SetBlinkTime(int milliseconds)
{
    if ( !::SetCaretBlinkTime(milliseconds) )
    {
        wxLogLastError("SetCaretBlinkTime");
    }
}

// ---------------------------------------------------------------------------
// creating/destroying the caret
// ---------------------------------------------------------------------------

bool wxCaret::MSWCreateCaret()
{
    wxASSERT_MSG( GetWindow(), "caret without window cannot be created" );
    wxASSERT_MSG( IsOk(),  "caret of zero size cannot be created" );

    if ( !m_hasCaret )
    {
        if ( !::CreateCaret(GetWinHwnd(GetWindow()), 0, m_width, m_height) )
        {
            wxLogLastError("CreateCaret");
        }
        else
        {
            m_hasCaret = TRUE;
        }
    }

    return m_hasCaret;
}

void wxCaret::OnSetFocus()
{
    if ( m_countVisible > 0 )
    {
        if ( MSWCreateCaret() )
        {
            // the caret was recreated but it doesn't remember its position and
            // it's not shown

            DoMove();
            DoShow();
        }
    }
    //else: caret is invisible, don't waste time creating it
}

void wxCaret::OnKillFocus()
{
    if ( m_hasCaret )
    {
        m_hasCaret = FALSE;

        if ( !::DestroyCaret() )
        {
            wxLogLastError("DestroyCaret");
        }
    }
}

// ---------------------------------------------------------------------------
// showing/hiding the caret
// ---------------------------------------------------------------------------

void wxCaret::DoShow()
{
    wxASSERT_MSG( GetWindow(), "caret without window cannot be shown" );
    wxASSERT_MSG( IsOk(), "caret of zero size cannot be shown" );

    if ( !m_hasCaret )
    {
        (void)MSWCreateCaret();
    }

    if ( !::ShowCaret(GetWinHwnd(GetWindow())) )
    {
        wxLogLastError("ShowCaret");
    }
}

void wxCaret::DoHide()
{
    if ( m_hasCaret )
    {
        if ( !::HideCaret(GetWinHwnd(GetWindow())) )
        {
            wxLogLastError("HideCaret");
        }
    }
}

// ---------------------------------------------------------------------------
// moving the caret
// ---------------------------------------------------------------------------

void wxCaret::DoMove()
{
    if ( m_hasCaret )
    {
        if ( !::SetCaretPos(m_x, m_y) )
        {
            wxLogLastError("SetCaretPos");
        }
    }
    //else: we don't have caret right now, nothing to do (this does happen)
}
