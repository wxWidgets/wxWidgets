/////////////////////////////////////////////////////////////////////////////
// Name:        univ/bmpbuttn.cpp
// Purpose:     wxBitmapButton implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     25.08.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "univbmpbuttn.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_STATLINE

#ifndef WX_PRECOMP
    #include "wx/dc.h"
    #include "wx/bmpbuttn.h"
    #include "wx/validate.h"
#endif

#include "wx/univ/renderer.h"

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(wxBitmapButton, wxButton)

BEGIN_EVENT_TABLE(wxBitmapButton, wxButton)
    EVT_SET_FOCUS(wxBitmapButton::OnSetFocus)
    EVT_KILL_FOCUS(wxBitmapButton::OnKillFocus)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// wxBitmapButton
// ----------------------------------------------------------------------------

bool wxBitmapButton::Create(wxWindow *parent,
                            wxWindowID id,
                            const wxBitmap& bitmap,
                            const wxPoint &pos,
                            const wxSize &size,
                            long style,
                            const wxValidator& validator,
                            const wxString &name)
{
    if ( !wxButton::Create(parent, id, bitmap, _T(""),
                           pos, size, style, validator, name) )
        return FALSE;

    m_bmpNormal = bitmap;

    return TRUE;
}

bool wxBitmapButton::ChangeBitmap(const wxBitmap& bmp)
{
    wxBitmap bitmap = bmp.Ok() ? bmp : m_bmpNormal;
    if ( bitmap != m_bitmap )
    {
        m_bitmap = bitmap;

        return TRUE;
    }

    return FALSE;
}

bool wxBitmapButton::Enable(bool enable)
{
    if ( !wxButton::Enable(enable) )
        return FALSE;

    if ( !enable && ChangeBitmap(m_bmpDisabled) )
        Refresh();

    return TRUE;
}

bool wxBitmapButton::SetCurrent(bool doit)
{
    ChangeBitmap(doit ? m_bmpFocus : m_bmpNormal);

    return wxButton::SetCurrent(doit);
}

void wxBitmapButton::OnSetFocus(wxFocusEvent& event)
{
    if ( ChangeBitmap(m_bmpFocus) )
        Refresh();

    event.Skip();
}

void wxBitmapButton::OnKillFocus(wxFocusEvent& event)
{
    if ( ChangeBitmap(m_bmpNormal) )
        Refresh();

    event.Skip();
}

void wxBitmapButton::Press()
{
    ChangeBitmap(m_bmpSelected);

    wxButton::Press();
}

void wxBitmapButton::Release()
{
    ChangeBitmap(IsFocused() ? m_bmpFocus : m_bmpNormal);

    wxButton::Release();
}

#endif // wxUSE_STATLINE

